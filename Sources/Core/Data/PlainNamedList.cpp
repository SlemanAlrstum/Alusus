/**
 * @file Core/Data/PlainNamedList.cpp
 * Contains the implementation of class Core::Data::PlainNamedList.
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
// Static Variables

const Int PlainNamedList::recordCountIncrement = 100;


//==============================================================================
// Initialization Functions

void PlainNamedList::initialize(Word maxStrSize, Word reservedCount)
{
  this->release();
  this->maxStrSize = maxStrSize;
  this->reservedCount = reservedCount;
  this->buffer = new Byte[this->reservedCount * this->getRecordSize()];
  this->count = 0;
}


void PlainNamedList::reinitialize(Word maxStrSize, Word reservedCount)
{
  if (this->buffer == 0) {
    throw GeneralException(STR("Buffer is not initialized."), STR("Core::Data::PlainNamedList::reinitialize"));
  }
  if (this->maxStrSize == maxStrSize && this->reservedCount == reservedCount) return;
  Byte *oldBuf = this->buffer;
  Word oldStrSize = this->maxStrSize;
  this->maxStrSize = maxStrSize;
  this->reservedCount = reservedCount;
  this->buffer = new Byte[this->reservedCount * this->getRecordSize()];

  if (!this->isEmpty()) {
    if (oldStrSize == this->maxStrSize) {
      memcpy(this->buffer, oldBuf, this->getRecordSize()*this->count);
    } else {
      for (Int i = 0; static_cast<Word>(i) < this->count; ++i) {
        Byte *destBuf = this->buffer + i*this->getRecordSize();
        Byte *srcBuf = oldBuf + i*PlainNamedList::getRecordSize(oldStrSize);
        sbstr_cast(destBuf).assign(sbstr_cast(srcBuf), this->maxStrSize);
        *reinterpret_cast<IdentifiableObject**>(destBuf+sizeof(Char)*this->maxStrSize) =
            *reinterpret_cast<IdentifiableObject**>(srcBuf+sizeof(Char)*oldStrSize);
      }
    }
  }
  delete[] oldBuf;
}


void PlainNamedList::copy(const PlainNamedList *src)
{
  if (src->buffer == 0) {
    throw InvalidArgumentException(STR("src"), STR("Core::Data::PlainNamedList::copy"),
                                   STR("Stack is not initialized."));
  }

  // If we have a small buffer, we can drop it.
  if (this->buffer!=0 && (this->maxStrSize < src->maxStrSize ||
                          this->reservedCount < src->reservedCount)) {
    delete[] this->buffer;
    this->buffer = 0;
  }

  // If we don't have a buffer, we need to create one with the same size.
  if (this->buffer == 0) {
    this->maxStrSize = src->maxStrSize;
    this->reservedCount = src->reservedCount;
    this->count = 0;
    this->buffer = new Byte[this->reservedCount * this->getRecordSize()];
  }

  // Copy members one by one.
  this->clear();
  for (Int i = 0; i < src->getCount(); ++i) {
    this->add(src->getName(i).c_str(), src->get(i));
  }
}


void PlainNamedList::release()
{
  if (this->buffer != 0) delete[] this->buffer;
  this->buffer = 0;
}


void PlainNamedList::clear()
{
  if (this->buffer == 0) {
    throw GeneralException(STR("List not initialized."), STR("Core::Data::PlainNamedList::clear"));
  }
  ASSERT(this->count >= 0);
  this->count = 0;
}


//==============================================================================
// DataOwner Implementation

void PlainNamedList::unsetIndexes(Int from, Int to)
{
  if (this->buffer == 0) return;
  for (Word i = 0; i < this->count; ++i) {
    IdentifiableObject *obj = this->get(i);
    if (obj != 0) Data::unsetIndexes(obj, from, to);
  }
}


//==============================================================================
// NamedListSharedContainer Implementation

void PlainNamedList::set(Int index, IdentifiableObject *val)
{
  if (this->buffer == 0) {
    throw GeneralException(STR("List not initialized."), STR("Core::Data::PlainNamedList::set"));
  }
  if (index < 0 || index >= this->count) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::PlainNamedList::set"),
                                   STR("Out of range."), index);
  }
  Byte *buf = this->buffer + index*this->getRecordSize();
  *reinterpret_cast<IdentifiableObject**>(buf+sizeof(Char)*this->maxStrSize) = val;
}


void PlainNamedList::remove(Int index)
{
  if (this->buffer == 0) {
    throw GeneralException(STR("List not initialized."), STR("Core::Data::PlainNamedList::remove"));
  }
  ASSERT(this->count >= 0);
  if (index < 0 || index >= this->count) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::PlainNamedList::remove"),
                                   STR("Out of range."), index);
  }
  if (this->count > index+1) {
    memcpy(this->buffer+this->getRecordSize()*index,
           this->buffer+this->getRecordSize()*(index+1),
           this->getRecordSize()*(this->count-(index+1)));
  }
  this->count--;
}


IdentifiableObject* PlainNamedList::get(Int index) const
{
  if (this->buffer == 0) {
    throw GeneralException(STR("List not initialized."), STR("Core::Data::PlainNamedList::get"));
  }
  if (index < 0 || index >= this->count) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::PlainNamedList::get"),
                                   STR("Out of range."), index);
  }
  Byte *buf = this->buffer + index*this->getRecordSize();
  return *reinterpret_cast<IdentifiableObject**>(buf+sizeof(Char)*this->maxStrSize);
}


Int PlainNamedList::add(Char const *name, IdentifiableObject *val)
{
  if (this->buffer == 0) {
    throw GeneralException(STR("List not initialized."), STR("Core::Data::PlainNamedList::add"));
  }
  if (this->count >= this->reservedCount) {
    this->reinitialize(this->maxStrSize, this->reservedCount+PlainNamedList::recordCountIncrement);
  }
  Byte *buf = this->buffer + this->count*this->getRecordSize();
  sbstr_cast(buf).assign(name==0?STR(""):name, this->maxStrSize);
  *reinterpret_cast<IdentifiableObject**>(buf+sizeof(Char)*this->maxStrSize) = val;
  this->count++;
  return this->count - 1;
}


void PlainNamedList::set(Int index, Char const *name, IdentifiableObject *val)
{
  if (this->buffer == 0) {
    throw GeneralException(STR("List not initialized."), STR("Core::Data::PlainNamedList::set"));
  }
  if (index < 0 || index >= this->count) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::PlainNamedList::set"),
                                   STR("Out of range."), index);
  }
  Byte *buf = this->buffer + index*this->getRecordSize();
  sbstr_cast(buf).assign(name==0?STR(""):name, this->maxStrSize);
  *reinterpret_cast<IdentifiableObject**>(buf+sizeof(Char)*this->maxStrSize) = val;
}


const SbStr& PlainNamedList::getName(Int index) const
{
  if (this->buffer == 0) {
    throw GeneralException(STR("List not initialized."), STR("Core::Data::PlainNamedList::getName"));
  }
  if (index < 0 || index >= this->count) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::PlainNamedList::getName"),
                                   STR("Out of range."), index);
  }
  Byte *buf = this->buffer + index*this->getRecordSize();
  return sbstr_cast(buf);
}

} } // namespace
