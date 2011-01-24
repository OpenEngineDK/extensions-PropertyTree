//
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#include "PropertyTreeNode.h"


namespace OpenEngine {
namespace Utils {

using namespace Math;
using namespace std;

template <>
Vector<3,float> ConvertFromString<Vector<3,float> >(string s) {
    Vector<3,float> v;
    istringstream istream(s);
    istream >> v[0];
    istream >> v[1];
    istream >> v[2];
    return v;
}

template <>
Math::Vector<4,float> ConvertFromString<Math::Vector<4,float> >(string s) {
    Vector<4,float> v;
    istringstream istream(s);
    istream >> v[0];
    istream >> v[1];
    istream >> v[2];
    istream >> v[3];
    return v;

}


template <>
Vector<3,float> ConvertFromSpecialNode<Vector<3,float> >(PropertyTreeNode* n,
                                                         Vector<3,float> def) {
    Vector<3,float> v = def;
    if (n->IsArray()) {
        v[0] = n->GetIdx(0,v[0]);
        v[1] = n->GetIdx(1,v[0]);
        v[2] = n->GetIdx(2,v[0]);
    }
    return v;
}

template <>
Vector<4,float> ConvertFromSpecialNode<Vector<4,float> >(PropertyTreeNode* n,
                                                         Vector<4,float> def) {
    Vector<4,float> v = def;
    if (n->IsArray()) {
        v[0] = n->GetIdx(0,v[0]);
        v[1] = n->GetIdx(1,v[0]);
        v[2] = n->GetIdx(2,v[0]);
        v[3] = n->GetIdx(3,v[0]);
    }
    return v;
}


template <>
string ConvertToString<Vector<3,float> >(Vector<3,float> v) {
    ostringstream ostream;
    ostream << v[0] << " ";
    ostream << v[1] << " ";
    ostream << v[2] << " ";
    return ostream.str();
}

template <>
string ConvertToString<Vector<4,float> >(Vector<4,float> v) {
    ostringstream ostream;
    ostream << v[0] << " ";
    ostream << v[1] << " ";
    ostream << v[2] << " ";
    ostream << v[3] << " ";
    return ostream.str();
}

template <>
bool ConvertToSpecial<Vector<3,float> >(PropertyTreeNode* n, Vector<3,float> v) {
    n->kind = PropertyTreeNode::ARRAY;

    n->GetNodeIdx(0)->Set(v[0]);
    n->GetNodeIdx(1)->Set(v[1]);
    n->GetNodeIdx(2)->Set(v[2]);

    return true;
}

template <>
bool ConvertToSpecial<Vector<4,float> >(PropertyTreeNode* n, Vector<4,float> v) {
    n->kind = PropertyTreeNode::ARRAY;

    n->GetNodeIdx(0)->Set(v[0]);
    n->GetNodeIdx(1)->Set(v[1]);
    n->GetNodeIdx(2)->Set(v[2]);
    n->GetNodeIdx(3)->Set(v[3]);

    return true;
}


PropertyTreeNode::~PropertyTreeNode() {
    for(map<string, PropertyTreeNode*>::iterator itr = subNodes.begin();
        itr != subNodes.end();
        itr++) {
        PropertyTreeNode *n = itr->second;
        delete n;
    }
    for (vector<PropertyTreeNode*>::iterator itr = subNodesArray.begin();
         itr != subNodesArray.end();
         itr++) {
        PropertyTreeNode *n = *itr;
        delete n;

    }
}

string PropertyTreeNode::ToString(int tabs) {

    ostringstream ost;
    if (kind == MAP) {
        ost << "map { " << endl;

        for(map<string, PropertyTreeNode*>::iterator itr = subNodes.begin();
            itr != subNodes.end();
            itr++) {
            string key = itr->first;
            PropertyTreeNode* n = itr->second;

            ost << key << " = " << n->ToString();
            ost << endl;
        }
        ost << "}";
    } else if (kind == SCALAR) {
        ost << value;
    } else if (kind == ARRAY) {
        ost << "array [" << endl;
        int i=0;
        for (vector<PropertyTreeNode*>::iterator itr = subNodesArray.begin();
             itr != subNodesArray.end();
             itr++) {
            PropertyTreeNode *n = *itr;
            ost << i++ << " = " << n->ToString();
        }
        ost << "]";
    } else {
        ost << "UNKNOWN";
    }

    return ost.str();
}



void PropertyTreeNode::Refresh(bool recursive) {
    list<string> clearList;
    for (map<string,string>::iterator itr = localCache.begin();
         itr != localCache.end();
         itr++) {
        string key = itr->first;

        if (tree->HaveKey(nodePath, key)) {
            clearList.push_back(key);

        }
    }
    for (list<string>::iterator itr = clearList.begin();
         itr != clearList.end();
         itr++) {
        localCache.erase(*itr);
    }
    if (recursive) {
        for (map<string,PropertyTreeNode*>::iterator itr = subNodes.begin();
             itr != subNodes.end();
             itr++) {
            PropertyTreeNode* node = itr->second;
            node->Refresh(recursive);
        }
    }

}

PropertyTreeNode* PropertyTreeNode::GetNodePath(string nodePath) {
    using namespace boost;
    vector<string> paths;
    split(paths,nodePath,is_any_of("."));
    PropertyTreeNode* node = this;
    for (vector<string>::iterator itr = paths.begin();
         itr != paths.end();
         itr++) {
        string pathPart = *itr;
        node = node->GetNode(pathPart);
    }
    return node;
}

 PropertyTreeNode* PropertyTreeNode::GetNodeIdx(unsigned int i) {
     kind = PropertyTreeNode::ARRAY;
     if (i >= subNodesArray.size())
         subNodesArray.push_back(new PropertyTreeNode(tree,this,""));
     return subNodesArray[i];
 }


PropertyTreeNode* PropertyTreeNode::GetNode(string key) {
    kind = PropertyTreeNode::MAP;
    string p = key;
    if (nodePath.compare("") != 0)
        p = nodePath + "." + key;
    if (!subNodes.count(key)) {
        subNodes[key] = new PropertyTreeNode(tree,this,p);
    }

    return subNodes[key];
}
unsigned int PropertyTreeNode::GetSize() {
    return subNodesArray.size();
}


bool PropertyTreeNode::HaveNode(string kp) {
    return subNodes.count(kp);
}


void PropertyTreeNode::SetDirty() {
    tree->AddToDirtySet(this);
    if (parent)
        parent->SetDirty();

}
PropertyTreeNode* PropertyTreeNode::GetParent() {
    return parent;
}

void PropertyTreeNode::SetValue(string v) {
    isSet = true;
    if (value.compare(v) != 0) {
        value = v;
        SetDirty();
    }
}



} // NS Utils
} // NS OpenEngine

