// 
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------


#ifndef _OE_PROPERTY_TREE_NODE2_H_
#define _OE_PROPERTY_TREE_NODE2_H_

#include "PropertyTree.h"
#include <string>
#include <map>
#include <sstream>
#include <Math/Vector.h>
#include <Logging/Logger.h>
#include <boost/algorithm/string.hpp>

namespace OpenEngine {
namespace Utils {

class PropertyTreeNode;

using namespace std;
    template<class T>
    string ConvertToString(T val) {
        ostringstream ostream;
        ostream << val;
        return ostream.str();        
    }
    
    template <>
    string ConvertToString<Math::Vector<3,float> >(Math::Vector<3,float>);


    template <class T>
    bool ConvertToSpecial(PropertyTreeNode* n, T val) {
        return false;
    }

    template <>
    bool ConvertToSpecial<Math::Vector<3,float> >(PropertyTreeNode* n, 
                                                  Math::Vector<3,float> v);
    

    template <class T>
    T ConvertFromString(string s) {
        istringstream istream(s);
        T val;
        istream >> val;
        return val;
    }

    template <>
    Math::Vector<3,float> ConvertFromString<Math::Vector<3,float> >(string s);   

    template <class T>
    T ConvertFromSpecialNode(PropertyTreeNode* n, T def) {
        return def;
    }

    template <>
    Math::Vector<3,float> ConvertFromSpecialNode<Math::Vector<3,float> >
    (PropertyTreeNode* n, Math::Vector<3,float> def);


/**
 * Short description.
 *
 * @class PropertyTreeNode PropertyTreeNode.h ons/PropertyTree/Utils/PropertyTreeNode.h
 */
class PropertyTreeNode {    

    friend class PropertyTree;
    
    //private:

public:
    PropertyTree* tree;
    string nodePath;
    string value;

    bool isSet;

    map<string,string> localCache;
    map<string, PropertyTreeNode*> subNodes;
    vector<PropertyTreeNode*> subNodesArray;
public:
    enum Kind {SCALAR,MAP,ARRAY};

    Kind kind;

    PropertyTreeNode(PropertyTree* t, string p) 
        : tree(t)
        , nodePath(p)
        , isSet(false)
        , kind(SCALAR)
    {
    }
    void Refresh(bool recursive=false);
    void SetValue(string v) {
        isSet = true;
        value = v;
    }
public:    
    ~PropertyTreeNode();

    bool IsArray() {
        return (kind == ARRAY);
    }

    template <class T>
    T GetIdx(int i, T def) {
        PropertyTreeNode* node = GetNodeIdx(i);
        return node->Get(def);
    }

    template <class T>
    T GetPath(string keyPath, T def) {
        PropertyTreeNode* node = GetNodePath(keyPath);
        return node->Get(def);
    }

    template <class T>
    void SetPath(string keyPath, T val) {        
        PropertyTreeNode* node = GetNodePath(keyPath);
        node->Set(val);
    }    

    template <class T>
    T Get( T def) {
        if (isSet) {
            T val = ConvertFromString<T>(value);
            return val;            
        } else {
            return ConvertFromSpecialNode<T>(this, def);
        }       
    }
    
    template <class T>
    void Set(T val) {
        if (!ConvertToSpecial<T>(this, val)) {
            isSet = true;
            value = ConvertToString(val);
        }
    }

    unsigned int GetSize();
    
    PropertyTreeNode* GetNodePath(string nodePath);

    PropertyTreeNode* GetNodeIdx(unsigned int i);
    PropertyTreeNode* GetNode(string key);

    bool HaveNode(string kp);

};
} // NS Utils
} // NS OpenEngine

#endif // _OE_PROPERTY_TREE_NODE2_H_
