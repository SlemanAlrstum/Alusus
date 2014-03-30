/**
 * @file Core/Basic/IdentifiableObject.cpp
 * Contains the implementation of class Basic::IdentifiableObject.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Basic
{

//==============================================================================
// Member Functions

/**
 * Returns a pointer to the type information object associated with this
 * class. This function (along with getMyTypeInfo) is automatically
 * defined by the TYPE_INFO macro.
 */
TypeInfo * IdentifiableObject::getTypeInfo()
{
  static Core::Basic::TypeInfo *type_info = 0;
  if (type_info == 0) {
    const Char *my_type = STR("IdentifiableObject");
    const Char *typeNamespace = STR("Core.Basic");
    const Char *moduleName = STR("Core");
    const Char *url = STR("alusus.net");
    const Char *uniqueName = STR("alusus.net#Core#Core.Basic.IdentifiableObject");
    type_info = reinterpret_cast<Core::Basic::TypeInfo*>(GLOBAL_STORAGE->getObject(uniqueName));
    if (type_info == 0) {
      type_info = new Core::Basic::TypeInfo(my_type, typeNamespace, moduleName, url, 0);
      GLOBAL_STORAGE->setObject(uniqueName, reinterpret_cast<void*>(type_info));
    }
  }
  return type_info;
}


/**
 * Check if this object is of the given type, or a derived type.
 *
 * @return Returns true if the given type info is for the class from which
 *         this object is instantiated, or for the class from which this
 *         object's class is derived, false otherwise.
 */
Bool IdentifiableObject::isDerivedFrom(TypeInfo * info) const
{
  TypeInfo * i = this->getMyTypeInfo();
  while (i != 0) {
    if (i == info) return true;
    i = i->getBaseTypeInfo();
  }
  return false;
}

} } // namespace
