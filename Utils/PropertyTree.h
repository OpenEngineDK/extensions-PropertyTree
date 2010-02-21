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

namespace OpenEngine {
namespace Utils {

struct PropertiesChangedEventArg {
        
};

/**
 * A class for loading properties from files
 *
 * @class PropertyTree PropertyTree.h PropertyTree/Utils/PropertyTree.h
 */
class PropertyTree {
private:
    YAML::Node doc;

    time_t last_timestamp;
    std::string filename;
    Core::Event<PropertiesChangedEventArg> changedEvent;

    bool HaveKey(std::string key);
    std::string GetString(std::string key);

    const YAML::Node* NodeForKeyPath(std::string key);

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
    


/**
 * Returns the property for key, or def if it doesn't exist.
 */    
    template <class T>
    T Get(std::string key, T def) {
        T val = def;
        //if (HaveKey(key)) {
        if (const YAML::Node *node = NodeForKeyPath(key)) {
            *node >> val;
        }
        return val;
    }
        

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
