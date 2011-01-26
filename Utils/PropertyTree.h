// 
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------


#ifndef _OE_PROPERTY_TREE2_H_
#define _OE_PROPERTY_TREE2_H_

#include <string>
#include <set>
#include "yaml/yaml.h"

#include <Core/Event.h>
#include <Core/EngineEvents.h>
#include <Core/IListener.h>
#include <Utils/DateTime.h>

namespace OpenEngine {
namespace Utils {

class PropertyTreeNode;

using namespace std;

class PropertiesChangedEventArg {
public:    
    enum ChangeFlag {
        VALUE = 1 << 0,
        TYPE = 1 << 1
    };

private:
    PropertyTreeNode* node;
    ChangeFlag flags;
    
public:    
    PropertiesChangedEventArg(PropertyTreeNode* n, ChangeFlag f=VALUE) 
  : node(n), flags(f) {}
    PropertyTreeNode* GetNode() { return node; }
    bool IsValueChange() { return flags & VALUE; }
    bool IsTypeChange() { return flags & TYPE; }
};

/**
 * Short description.
 *
 * @class PropertyTree PropertyTree.h ons/PropertyTree/Utils/PropertyTree.h
 */
class PropertyTree : public Core::IListener<Core::ProcessEventArg> {
    friend class PropertyTreeNode;    
protected:    
    void AddToDirtySet(PropertyTreeNode* n, PropertiesChangedEventArg::ChangeFlag f);

private:
    set<pair<PropertyTreeNode*,PropertiesChangedEventArg::ChangeFlag> > dirtySet;
    PropertyTreeNode* root;
    YAML::Node doc;

    std::string filename;

    PropertyTreeNode* LoadYamlMap(PropertyTreeNode* r, const YAML::Node& n);
    PropertyTreeNode* LoadYamlSeq(PropertyTreeNode* r, const YAML::Node& n);
    PropertyTreeNode* LoadYamlNode(PropertyTreeNode* r, const YAML::Node& n);

    Timer reloadTimer;
    DateTime lastTimestamp;

    Core::Event<PropertiesChangedEventArg> changedEvent;
public:
    enum PropertyType {
        UNKNOWN,
        FLOAT,
        VEC3F
    };


    const YAML::Node* NodeForKeyPath(string key);

    template<class T>
    void GetPath(string p, string k, T* val) {
        const YAML::Node* node = NodeForKeyPath(p);
        const YAML::Node* valNode = node->FindValue(k);
        *valNode >> *val;
    }

    bool HaveKey(std::string p, std::string k);

public:
    void LoadFromFile(std::string fname);    
    void SaveToFile(std::string file);

    void Save();

    PropertyTree();
    PropertyTree(std::string fname);
    PropertyTreeNode* GetRootNode();
    void Reload(bool skipTS=false);
    void ReloadIfNeeded();
    void Print();

    void Handle(Core::ProcessEventArg arg);
    
    Core::IEvent<PropertiesChangedEventArg>& PropertiesChangedEvent();
};
} // NS Utils
} // NS OpenEngine

#endif // _OE_PROPERTY_TREE2_H_
