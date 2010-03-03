//
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#ifndef _PROPERTY_TREE_H_
#define _PROPERTY_TREE_H_

#include <Core/Event.h>
#include <string>
#include <sstream>
#include "yaml/yaml.h"

#include <Math/Vector.h>

#include <Core/IModule.h>

namespace OpenEngine {
namespace Utils {

struct PropertiesChangedEventArg {

};

class PropertyTreeNode {
    friend class PropertyTree;
protected:
    PropertyTreeNode(const YAML::Node* n) : _node(n) {}

private:
    const YAML::Node* _node;

    const YAML::Node* NodeForKeyPath(std::string key);

public:
    PropertyTreeNode GetNode(std::string key) {
        return PropertyTreeNode(_node->FindValue(key));
    }

    bool HaveNode(std::string key) {
        return _node->FindValue(key);
    }

    PropertyTreeNode GetNode(unsigned int key) {
        return PropertyTreeNode(_node->FindValue(key));
    }

    unsigned int GetSize() {
        return _node->size();
    }

    template <class T>
    T Get() {
        T val;
        *_node >> val;
        return val;
    }
    
    template <class T>
    T Get(std::string key, T def) {
        T val = def;
        if (const YAML::Node* n = _node->FindValue(key))
            *n >> val;
        return val;
    }
    template <class T>
    T GetPath(std::string key, T def) {
        T val = def;
        if (const YAML::Node* n = NodeForKeyPath(key))
            *n >> val;
        return val;
    }
    
};

/**
 * A class for loading properties from files
 *
 * @class PropertyTree PropertyTree.h PropertyTree/Utils/PropertyTree.h
 */
class PropertyTree : public Core::IModule {
private:
    YAML::Node doc;

    time_t last_timestamp;
    std::string filename;
    Core::Event<PropertiesChangedEventArg> changedEvent;

    bool HaveKey(std::string key);
    std::string GetString(std::string key);

    const YAML::Node* NodeForKeyPath(std::string key);

    Timer reloadTimer;

    PropertyTreeNode* root;
public:

/**
 * Creates a PropertyTree assioated with a file, but doesnt load it.
 */
    PropertyTree(std::string path);

/**
 * Checks if the file is changed, and reloads if nessesary.
 */

    void ReloadIfNeeded();
    void Reload();

    void Handle(Core::InitializeEventArg arg);
    void Handle(Core::ProcessEventArg arg);
    void Handle(Core::DeinitializeEventArg arg);

    bool HaveNode(std::string key);
    PropertyTreeNode GetNode(std::string key);

/**
 * Returns the property for key, or def if it doesn't exist.
 */
    template <class T>
    T Get(std::string key, T def) {
        T val = def;
        if (HaveNode(key)) {
            PropertyTreeNode n = GetNode(key);
            val = n.Get<T>();
        }
        return val;
    }
    template <class T>
    T GetIdxMap(std::string key, unsigned int idx, std::string mkey, T def) {
        T val = def;
        //if (HaveKey(key)) {
        if (const YAML::Node *node = NodeForKeyPath(key)) {
            const YAML::Node& n2 = (*node)[idx];
            if (n2.FindValue(mkey))
                n2[mkey] >> val;
        }
        return val;
    }
    

    unsigned int GetSize(std::string key);

    Core::IEvent<PropertiesChangedEventArg>& PropertiesChangedEvent() {return changedEvent; }
};

void operator >> (const YAML::Node& node, Math::Vector<3,float>& v);
//  {
//     node[0] >> v[0];
//     node[1] >> v[1];
//     node[2] >> v[2];
// }


} // NS Utils
} // NS OpenEngine

#endif
