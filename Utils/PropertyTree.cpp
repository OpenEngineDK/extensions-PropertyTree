//
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#include "PropertyTree.h"
#include "PropertyTreeNode.h"

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <Logging/Logger.h>
#include <Resources/File.h>
#include <Utils/Convert.h>

namespace OpenEngine {
namespace Utils {

using namespace std;

PropertyTree::PropertyTree() {
    root = new PropertyTreeNode(this, NULL,  "");
}

PropertyTree::PropertyTree(string fname) : filename(fname) {
    root = new PropertyTreeNode(this, NULL, "");
    Reload(true);
}

PropertyTreeNode* PropertyTree::GetRootNode() {
    return root;
}


void PropertyTree::AddToDirtySet(PropertyTreeNode* n,
                                 PropertiesChangedEventArg::ChangeFlag f) {
    dirtySet.insert(make_pair<>(n,f));
}
    
bool PropertyTree::HaveKey(std::string p, std::string k) {
    const YAML::Node* node = NodeForKeyPath(p);
    if (!node)
        return false;

    const YAML::Node* valNode = node->FindValue(k);

    return valNode;
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

PropertyTreeNode* PropertyTree::LoadYamlSeq(PropertyTreeNode* r, const YAML::Node& n) {
    r->kind = PropertyTreeNode::ARRAY;
    
    int i=0;
    for(YAML::Iterator it=n.begin();it!=n.end();++it) {        
        PropertyTreeNode* n = r->GetNodeIdx(i++);
        
        const YAML::Node& valNode = *it;

        LoadYamlNode(n, valNode);
    }


    return r;
}

PropertyTreeNode* PropertyTree::LoadYamlMap(PropertyTreeNode* r, const YAML::Node& n) {
    r->kind = PropertyTreeNode::MAP;
    for(YAML::Iterator it=n.begin();it!=n.end();++it) {
        std::string key;
        it.first() >> key;
        PropertyTreeNode* n = r->GetNode(key);
        

        const YAML::Node& valNode = it.second();
        LoadYamlNode(n, valNode);

    }
    return r;

}


PropertyTreeNode* PropertyTree::LoadYamlNode(PropertyTreeNode* r, const YAML::Node& n) {
    if (n.GetType() == YAML::CT_MAP) {
        LoadYamlMap(r, n);
    } else if (n.GetType() == YAML::CT_SEQUENCE) {
        LoadYamlSeq(r, n);        
    } else if (n.GetType() == YAML::CT_SCALAR) {        
        r->kind = PropertyTreeNode::SCALAR;
        string v;
        n >> v;

        r->SetValue(v);
    }
    return r;
}


void PropertyTree::LoadFromFile(string file) {
    ifstream fin(file.c_str());

    YAML::Parser parser(fin);
    parser.GetNextDocument(doc);

    LoadYamlNode(root,doc);

    fin.close();
    
    reloadTimer.Start();
}


void PropertyTree::Reload(bool skipTS) {
    if (!skipTS) {
        DateTime newTimestamp = Resources::File::GetLastModified(filename);    
        lastTimestamp = newTimestamp;
    }
    LoadFromFile(filename);
    logger.info << "Reloading" << logger.end;

    PropertiesChangedEventArg arg(root);
    changedEvent.Notify(arg);
}


class Emitter  {
public:
    YAML::Emitter out;
    PropertyTree *tree;
    bool comments;

    Emitter(PropertyTree* t, bool comments) : tree(t), comments(comments) {}
    
    void EmitArray(PropertyTreeNode* node) {
        if (node->GetType() == PropertyTree::VEC3F) {
            out << YAML::Flow;
        }
        out << YAML::BeginSeq;
        for (vector<PropertyTreeNode*>::iterator itr = node->subNodesArray.begin();
             itr != node->subNodesArray.end();
             itr++) {
            Emit(*itr);
        }
        out << YAML::EndSeq;
    }
    void EmitMap(PropertyTreeNode* node) {
        out << YAML::BeginMap;
        for (map<string,PropertyTreeNode*>::iterator itr = node->subNodes.begin();
             itr != node->subNodes.end();
             itr++) {
            out << YAML::Key << itr->first;
            out << YAML::Value;
            Emit(itr->second);
        }
        out << YAML::EndMap;
    }

    void Emit(PropertyTreeNode* node) {
        if (node->kind == PropertyTreeNode::MAP) {
            EmitMap(node);
        } else if (node->kind == PropertyTreeNode::ARRAY) {
            EmitArray(node);
        
        } else if (node->kind == PropertyTreeNode::SCALAR) {
            out << node->value;
            if (comments && !node->HaveBeenRead())
                out << YAML::Comment("Never read");
        }        
    }
    
};

void PropertyTree::Save() {
    SaveToFile(filename);
}

void PropertyTree::SaveWithComments() {
    SaveToFile(filename, true);
}


void PropertyTree::SaveToFile(string file, bool comments) {
    
    Emitter e(this,comments);
    
    e.Emit(root);
    
    ofstream of(file.c_str());
    of << e.out.c_str();
    of.close();

}


void PropertyTree::Handle(Core::ProcessEventArg arg) {
    if (reloadTimer.GetElapsedIntervals(1000000)) {
        reloadTimer.Reset();
        ReloadIfNeeded();
    }
    if (dirtySet.size()) {
        for(set<pair<PropertyTreeNode*,PropertiesChangedEventArg::ChangeFlag> >::iterator itr = dirtySet.begin();
            itr != dirtySet.end();
            itr++) {
            PropertyTreeNode* n = itr->first;
            PropertiesChangedEventArg::ChangeFlag flag = itr->second;
            PropertiesChangedEventArg arg(n, flag);
            n->PropertiesChangedEvent().Notify(arg);            
        }
        dirtySet.clear();
    }

}

void PropertyTree::ReloadIfNeeded() {
    DateTime newTimestamp = Resources::File::GetLastModified(filename);
    if (newTimestamp != lastTimestamp) {
        Reload();
    }
}

    
Core::IEvent<PropertiesChangedEventArg>& PropertyTree::PropertiesChangedEvent() {
    return changedEvent;
}


} // NS Utils
} // NS OpenEngine

