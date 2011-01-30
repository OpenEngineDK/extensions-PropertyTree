// 
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------


#ifndef _OE_PROPERTY_BINDER_H_
#define _OE_PROPERTY_BINDER_H_

#include <Core/IListener.h>
#include <Utils/PropertyTree.h>

namespace OpenEngine {
namespace Utils {

/**
 * Short description.
 *
 * @class PropertyBinder PropertyBinder.h ons/PropertyTree/Utils/PropertyBinder.h
 */
template <class C, class T>
class PropertyBinder : public Core::IListener<PropertiesChangedEventArg> {
private:
    PropertyTreeNode* node;
    C& instance;
    T def;
    void (C::*setFunc)(T);
public:
    PropertyBinder(PropertyTreeNode* n,
                   C& inst,
                   void (C::*sFun)(T),
                   T def)
        : node(n)
        , instance(inst)
        , def(def)
        , setFunc(sFun) {
        node->PropertiesChangedEvent().Attach(*this);
        (instance.*setFunc)(node->Get<T>(def));
    }

    void Handle(PropertiesChangedEventArg arg) {
        (instance.*setFunc)(node->Get<T>(def));
    }
};
} // NS Utils
} // NS OpenEngine

#endif // _OE_PROPERTY_BINDER_H_
