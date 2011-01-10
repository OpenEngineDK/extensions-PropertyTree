//
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#include "PropertyTree.h"
#include <Resources/File.h>
#include <boost/algorithm/string.hpp>

#include <fstream>

namespace OpenEngine {
namespace Utils {

using namespace std;
using namespace Core;


const YAML::Node* PropertyTreeNode::NodeForKeyPath(string key) {
    using namespace boost;
    vector<string> paths;
    split(paths,key,is_any_of("."));

    const YAML::Node* node = _node;
    for (vector<string>::iterator itr = paths.begin();
         itr != paths.end();
         itr++) {
        string pathPart = *itr;
        node = node->FindValue(pathPart);
        if (!node)
            return NULL;
    }
    return node;
}


PropertyTree::PropertyTree(string fname) : filename(fname), root(NULL) {
    Reload(true);
}




void PropertyTree::ReloadIfNeeded() {
    DateTime newTimestamp = Resources::File::GetLastModified(filename);
    if (newTimestamp != lastTimestamp) {
        Reload();
    }
}

bool PropertyTree::HaveNode(std::string key) {
    return NodeForKeyPath(key);
}

PropertyTreeNode PropertyTree::GetNode(std::string key) {
    return PropertyTreeNode(NodeForKeyPath(key));
}

const YAML::Node* PropertyTree::NodeForKeyPath(string key) {
    using namespace boost;
    vector<string> paths;
    split(paths,key,is_any_of("."));

    const YAML::Node* node = &doc;
    for (vector<string>::iterator itr = paths.begin();
         itr != paths.end();
         itr++) {
        string pathPart = *itr;
        node = node->FindValue(pathPart);
        if (!node)
            return NULL;
    }
    return node;
}

void PropertyTree::Reload(bool skipTS) {
    if (!skipTS) {
        DateTime newTimestamp = Resources::File::GetLastModified(filename);    
        lastTimestamp = newTimestamp;
    }
    ifstream fin(filename.c_str());

    YAML::Parser parser(fin);
    parser.GetNextDocument(doc);
    delete root;
    root = new PropertyTreeNode(&doc);

    PropertiesChangedEventArg arg;
    changedEvent.Notify(arg);
}


unsigned int PropertyTree::GetSize(std::string key) {    
    if (const YAML::Node *node = NodeForKeyPath(key))
        return node->size();
    return 0;
}

bool PropertyTree::HaveKey(string key) {
    return false;
}

string PropertyTree::GetString(string key) {
    return "";
}

void PropertyTree::Handle(InitializeEventArg arg) {    
    reloadTimer.Start();
}
void PropertyTree::Handle(ProcessEventArg arg) {
    if (reloadTimer.GetElapsedIntervals(1000000)) {
        reloadTimer.Reset();
        ReloadIfNeeded();
    }
}
void PropertyTree::Handle(DeinitializeEventArg arg) {}

void operator >> (const YAML::Node& node, Math::Vector<3,float>& v) {
     node[0] >> v[0];
     node[1] >> v[1];
     node[2] >> v[2];
}

void operator >> (const YAML::Node& node, Math::Vector<4,float>& v) {
     node[0] >> v[0];
     node[1] >> v[1];
     node[2] >> v[2];
     node[3] >> v[3];
}



} // NS Utils
} // NS OpenEngine
