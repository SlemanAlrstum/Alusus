/**
 * @file Core/Data/IndirectReference.cpp
 * Contains the implementation of class Core::Data::IndirectReference.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data
{

//==============================================================================
// Member Functions

Bool IndirectReference::compare(Reference const *r) const
{
  const IndirectReference *ir = io_cast<IndirectReference>(r);
  if (ir == 0) return false;
  else if (ir->getKey() != 0 && this->key != 0 && this->key->compare(ir->getKey().get())) {
    return Reference::compare(r);
  } else if (ir->getKey() == 0 && this->key == 0) return Reference::compare(r);
  else return false;
}


Bool IndirectReference::setShared(Provider *provider, IdentifiableObject *parent,
                                  SharedPtr<IdentifiableObject> const &obj, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndirectReference::set"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::IndirectReference::set"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  IdentifiableObject *ref;
  if (!provider->tryGetPlainValue(this->key.get(), ref)) {
    index = -1;
    return false;
  }
  if (ref == 0) {
    index = -1;
    return false;
  }
  if (ref->isA<String>()) {
    index = -1;
    MapSharedContainer *container = parent->getInterface<MapSharedContainer>();
    if (container == 0) return false;
    container->set(static_cast<String*>(ref)->get(), obj);
    return true;
  } else if (ref->isA<Integer>()) {
    index = -1;
    Int i = static_cast<Integer*>(ref)->get();
    SharedContainer *container = parent->getInterface<SharedContainer>();
    if (container == 0) return false;
    if (i >= 0 && i < container->getCount()) {
      container->set(i, obj);
      return true;
    } else if (i == container->getCount()) {
      ListSharedContainer *listContainer = parent->getInterface<ListSharedContainer>();
      if (listContainer == 0) return false;
      listContainer->add(obj);
      return true;
    } else {
      return false;
    }
  } else if (ref->isA<Reference>()) {
    return static_cast<Reference*>(ref)->setShared(provider, parent, obj, index);
  } else {
    index = -1;
    return false;
  }
}


Bool IndirectReference::setPlain(Provider *provider, IdentifiableObject *parent,
                                 IdentifiableObject *obj, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndirectReference::setPlain"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::IndirectReference::setPlain"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  IdentifiableObject *ref;
  if (!provider->tryGetPlainValue(this->key.get(), ref)) {
    index = -1;
    return false;
  }
  if (ref == 0) {
    index = -1;
    return false;
  }
  if (ref->isA<String>()) {
    index = -1;
    MapPlainContainer *container = parent->getInterface<MapPlainContainer>();
    if (container == 0) return false;
    container->set(static_cast<String*>(ref)->get(), obj);
    return true;
  } else if (ref->isA<Integer>()) {
    index = -1;
    Int i = static_cast<Integer*>(ref)->get();
    PlainContainer *container = parent->getInterface<PlainContainer>();
    if (container == 0) return false;
    if (i >= 0 && i < container->getCount()) {
      container->set(i, obj);
      return true;
    } else if (i == container->getCount()) {
      ListPlainContainer *listContainer = parent->getInterface<ListPlainContainer>();
      if (listContainer == 0) return false;
      listContainer->add(obj);
      return true;
    } else {
      return false;
    }
  } else if (ref->isA<Reference>()) {
    return static_cast<Reference*>(ref)->setPlain(provider, parent, obj, index);
  } else {
    index = -1;
    return false;
  }
}


Bool IndirectReference::remove(Provider *provider, IdentifiableObject *parent, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndirectReference::remove"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::IndirectReference::remove"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  IdentifiableObject *ref;
  if (!provider->tryGetPlainValue(this->key.get(), ref)) {
    index = -1;
    return false;
  }
  if (ref == 0) {
    index = -1;
    return false;
  }
  if (ref->isA<String>()) {
    index = -1;
    MapSharedContainer *container;
    MapPlainContainer *plainContainer;
    if ((container = parent->getInterface<MapSharedContainer>()) != 0) {
      Int i = container->findIndex(static_cast<String*>(ref)->get());
      if (i == -1) return false;
      container->remove(i);
      return true;
    } else if ((plainContainer = parent->getInterface<MapPlainContainer>()) != 0) {
      Int i = plainContainer->findIndex(static_cast<String*>(ref)->get());
      if (i == -1) return false;
      plainContainer->remove(i);
      return true;
    } else {
      return false;
    }
  } else if (ref->isA<Integer>()) {
    index = -1;
    Int i = static_cast<Integer*>(ref)->get();
    SharedContainer *container;
    PlainContainer *plainContainer;
    if ((container = parent->getInterface<SharedContainer>()) != 0) {
      if (i < 0 || i >= container->getCount()) return false;
      container->remove(i);
      return true;
    } else if ((plainContainer = parent->getInterface<PlainContainer>()) != 0) {
      if (i < 0 || i >= plainContainer->getCount()) return false;
      plainContainer->remove(i);
      return true;
    } else {
      return false;
    }
  } else if (ref->isA<Reference>()) {
    return static_cast<Reference*>(ref)->remove(provider, parent, index);
  } else {
    index = -1;
    return false;
  }
}


Bool IndirectReference::getShared(Provider *provider, IdentifiableObject *parent,
                                  SharedPtr<IdentifiableObject> &result, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndirectReference::get"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::IndirectReference::get"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  IdentifiableObject *ref;
  if (!provider->tryGetPlainValue(this->key.get(), ref)) {
    index = -1;
    return false;
  }
  if (ref == 0) {
    index = -1;
    return false;
  }
  if (ref->isA<String>()) {
    index = -1;
    MapSharedContainer const *container = parent->getInterface<MapSharedContainer>();
    if (container == 0) return false;
    Int i = container->findIndex(static_cast<String*>(ref)->get());
    if (i == -1) return false;
    result = container->get(i);
    return true;
  } else if (ref->isA<Integer>()) {
    index = -1;
    SharedContainer const *container = parent->getInterface<SharedContainer>();
    if (container == 0) return false;
    Int i = static_cast<Integer*>(ref)->get();
    if (i < 0 || i >= container->getCount()) return false;
    result = container->get(i);
    return true;
  } else if (ref->isA<Reference>()) {
    return static_cast<Reference*>(ref)->getShared(provider, parent, result, index);
  } else {
    index = -1;
    return false;
  }
}


Bool IndirectReference::getPlain(Provider *provider, IdentifiableObject *parent,
                                 IdentifiableObject *&result, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndirectReference::getPlain"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::IndirectReference::getPlain"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  IdentifiableObject *ref;
  if (!provider->tryGetPlainValue(this->key.get(), ref)) {
    index = -1;
    return false;
  }
  if (ref == 0) {
    index = -1;
    return false;
  }
  if (ref->isA<String>()) {
    index = -1;
    MapSharedContainer const *container;
    MapPlainContainer const *plainContainer;
    if ((container = parent->getInterface<MapSharedContainer>()) != 0) {
      Int i = container->findIndex(static_cast<String*>(ref)->get());
      if (i == -1) return false;
      result = container->get(i).get();
      return true;
    } else if ((plainContainer = parent->getInterface<MapPlainContainer>()) != 0) {
      Int i = plainContainer->findIndex(static_cast<String*>(ref)->get());
      if (i == -1) return false;
      result = plainContainer->get(i);
      return true;
    } else {
      return false;
    }
  } else if (ref->isA<Integer>()) {
    index = -1;
    Int i = static_cast<Integer*>(ref)->get();
    SharedContainer const *container;
    PlainContainer const *plainContainer;
    if ((container = parent->getInterface<SharedContainer>()) != 0) {
      if (i < 0 || i >= container->getCount()) return false;
      result = container->get(i).get();
      return true;
    } else if ((plainContainer = parent->getInterface<PlainContainer>()) != 0) {
      if (i < 0 || i >= plainContainer->getCount()) return false;
      result = plainContainer->get(i);
      return true;
    } else {
      return false;
    }
  } else if (ref->isA<Reference>()) {
    return static_cast<Reference*>(ref)->getPlain(provider, parent, result, index);
  } else {
    index = -1;
    return false;
  }
}


void IndirectReference::unsetIndexes(Int from, Int to)
{
  if (this->key != 0) this->key->unsetIndexes(from, to);
  Reference::unsetIndexes(from, to);
}

} } // namespace
