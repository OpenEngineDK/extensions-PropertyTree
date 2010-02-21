//
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#include "PropertyTree.h"
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>

#include <fstream>

namespace OpenEngine {
namespace Utils {

using namespace std;
using namespace boost::filesystem;

PropertyTree::PropertyTree(string fname) : filename(fname) {
}

void PropertyTree::ReloadIfNeeded() {


    time_t new_timestamp = last_write_time(filename);
    if (new_timestamp != last_timestamp) {
        Reload();
    }
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

void PropertyTree::Reload() {
    time_t new_timestamp = last_write_time(filename);
    last_timestamp = new_timestamp;

    ifstream fin(filename.c_str());

    YAML::Parser parser(fin);
    parser.GetNextDocument(doc);

    PropertiesChangedEventArg arg;
    changedEvent.Notify(arg);
}

bool PropertyTree::HaveKey(string key) {
    return false;
}

string PropertyTree::GetString(string key) {
    return "";
}



} // NS Utils
} // NS OpenEngine
