/**
 * @file Spp/Ast/Template.cpp
 * Contains the implementation of class Spp::Ast::Template.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace Ast
{

//==============================================================================
// Member Functions

TioSharedPtr const& Template::getDefaultInstance(Helper *helper)
{
  if (this->templateBody == 0) {
    throw EXCEPTION(GenericException, STR("Template body is not set."));
  }

  // Do we already have a default instance?
  for (Int i = 0; i < this->instances.size(); ++i) {
    if (this->instances[i]->getCount() == 1) {
      return this->instances[i]->get(0);
    }
  }
  // No default instance was found, create a new one.
  auto block = std::make_shared<Block>();
  block->add(this->templateBody->clone());
  this->instances.push_back(block);
  block->setOwner(this);
  return this->instances.back()->get(0);
}


Bool Template::matchInstance(TiObject *templateInput, Helper *helper, TioSharedPtr &result)
{
  if (this->templateBody == 0) {
    throw EXCEPTION(GenericException, STR("Template body is not set."));
  }

  SharedPtr<Core::Notices::Notice> notice;

  // Do we already have an instance?
  for (Int i = 0; i < this->instances.size(); ++i) {
    if (this->matchTemplateVars(templateInput, this->instances[i].get(), helper, notice)) {
      result = this->instances[i]->get(0);
      return true;
    } else {
      if (notice != 0) {
        result = notice;
        return false;
      }
    }
  }
  // No instance was found, create a new one.
  auto block = std::make_shared<Block>();
  block->add(this->templateBody->clone());
  if (!this->assignTemplateVars(templateInput, block.get(), helper, notice)) {
    result = notice;
    return false;
  }
  this->instances.push_back(block);
  block->setOwner(this);
  result = this->instances.back()->get(0);
  return true;
}


Bool Template::matchTemplateVars(
  TiObject *templateInput, Block *instance, Helper *helper, SharedPtr<Core::Notices::Notice> &notice
) {
  if (templateInput->isDerivedFrom<Core::Data::Ast::List>()) {
    auto list = static_cast<Core::Data::Ast::List*>(templateInput);
    if (this->varDefs.size() != list->getCount()) {
      notice = std::make_shared<Spp::Notices::TemplateArgMismatchNotice>(
        Core::Data::Ast::findSourceLocation(templateInput)
      );
      return false;
    }
    for (Int i = 0; i < list->getCount(); ++i) {
      if (!this->matchTemplateVar(list->getElement(i), instance, i, helper, notice)) return false;
    }
  } else {
    if (this->varDefs.size() != 1) {
      notice = std::make_shared<Spp::Notices::TemplateArgMismatchNotice>(
        Core::Data::Ast::findSourceLocation(templateInput)
      );
      return false;
    }
    if (!this->matchTemplateVar(templateInput, instance, 0, helper, notice)) return false;
  }
  return true;
}


Bool Template::matchTemplateVar(
  TiObject *templateInput, Block *instance, Int varIndex, Helper *helper,
  SharedPtr<Core::Notices::Notice> &notice
) {
  switch (this->varDefs[varIndex].second.val) {
    case VarType::INTEGER: {
      auto var = ti_cast<Core::Basic::TiInt>(
        Template::getTemplateVar(instance, this->varDefs[varIndex].first.c_str())
      );
      if (var == 0) {
        throw EXCEPTION(GenericException, STR("Missing variable in template instance."));
      }
      auto newVar = ti_cast<Core::Data::Ast::IntegerLiteral>(
        Template::traceObject(templateInput, VarType::INTEGER, helper)
      );
      if (newVar == 0) {
        notice = std::make_shared<Spp::Notices::InvalidTemplateArgNotice>(
          Core::Data::Ast::findSourceLocation(templateInput)
        );
        return false;
      }
      return std::stol(newVar->getValue().get()) == var->get();
    }

    case VarType::STRING: {
      auto var = ti_cast<TiStr>(
        Template::getTemplateVar(instance, this->varDefs[varIndex].first.c_str())
      );
      if (var == 0) {
        throw EXCEPTION(GenericException, STR("Missing variable in template instance."));
      }
      auto newVar = ti_cast<Core::Data::Ast::StringLiteral>(
        Template::traceObject(templateInput, VarType::STRING, helper)
      );
      if (newVar == 0) {
        notice = std::make_shared<Spp::Notices::InvalidTemplateArgNotice>(
          Core::Data::Ast::findSourceLocation(templateInput)
        );
        return false;
      }
      return newVar->getValue() == var->get();
    }

    default: {
      auto var = Template::getTemplateVar(instance, this->varDefs[varIndex].first.c_str());
      if (var == 0) {
        throw EXCEPTION(GenericException, STR("Missing variable in template instance."));
      }
      auto newVar = Template::traceObject(templateInput, this->varDefs[varIndex].second, helper);
      if (newVar == 0) {
        notice = std::make_shared<Spp::Notices::InvalidTemplateArgNotice>(
          Core::Data::Ast::findSourceLocation(templateInput)
        );
        return false;
      }
      return newVar == var;
    }
  }
}


Bool Template::assignTemplateVars(
  TiObject *templateInput, Block *instance, Helper *helper, SharedPtr<Core::Notices::Notice> &notice
) {
  if (templateInput->isDerivedFrom<Core::Data::Ast::List>()) {
    auto list = static_cast<Core::Data::Ast::List*>(templateInput);
    if (this->varDefs.size() != list->getCount()) {
      notice = std::make_shared<Spp::Notices::TemplateArgMismatchNotice>(
        Core::Data::Ast::findSourceLocation(templateInput)
      );
      return false;
    }
    for (Int i = 0; i < list->getCount(); ++i) {
      auto var = Template::traceObject(list->getElement(i), this->varDefs[i].second, helper);
      auto def = Core::Data::Ast::Definition::create();
      def->setName(this->varDefs[i].first.c_str());
      if (this->varDefs[i].second == VarType::INTEGER) {
        auto intLiteral = static_cast<Core::Data::Ast::IntegerLiteral*>(var);
        def->setTarget(Core::Basic::TiInt::create(std::stol(intLiteral->getValue().get())));
      } else if (this->varDefs[i].second == VarType::STRING) {
        auto strLiteral = static_cast<Core::Data::Ast::StringLiteral*>(var);
        def->setTarget(std::make_shared<TiStr>(strLiteral->getValue().get()));
      } else {
        def->setTarget(std::make_shared<Core::Basic::TioWeakBox>(getWeakPtr(var)));
      }
      instance->add(def);
    }
  } else {
    if (this->varDefs.size() != 1) {
      notice = std::make_shared<Spp::Notices::TemplateArgMismatchNotice>(
        Core::Data::Ast::findSourceLocation(templateInput)
      );
      return false;
    }
    auto var = Template::traceObject(templateInput, this->varDefs[0].second, helper);
    auto def = Core::Data::Ast::Definition::create();
    def->setName(this->varDefs[0].first.c_str());
    if (this->varDefs[0].second == VarType::INTEGER) {
      auto intLiteral = static_cast<Core::Data::Ast::IntegerLiteral*>(var);
      def->setTarget(Core::Basic::TiInt::create(std::stol(intLiteral->getValue().get())));
    } else if (this->varDefs[0].second == VarType::STRING) {
      auto strLiteral = static_cast<Core::Data::Ast::StringLiteral*>(var);
      def->setTarget(std::make_shared<TiStr>(strLiteral->getValue().get()));
    } else {
      def->setTarget(std::make_shared<Core::Basic::TioWeakBox>(getWeakPtr(var)));
    }
  instance->add(def);
  }
  return true;
}


TiObject* Template::getTemplateVar(Block const *instance, Char const *name)
{
  for (Int i = 0; i < instance->getCount(); ++i) {
    auto def = ti_cast<Core::Data::Ast::Definition>(instance->getElement(i));
    if (def != 0 && def->getName() == name) {
      auto box = def->getTarget().ti_cast_get<Core::Basic::TioWeakBox>();
      if (box != 0) return box->get().lock().get();
      else return def->getTarget().get();
    }
  }
  throw EXCEPTION(GenericException, STR("Template var not found."));
}


TiObject* Template::traceObject(TiObject *ref, VarType varType, Helper *helper)
{
  TiObject *result = 0;
  Node *refNode = ti_cast<Node>(ref);
  if (refNode == 0) {
    throw EXCEPTION(GenericException, STR("Invalid template variable."));
  }
  if (varType == VarType::INTEGER && ref->isDerivedFrom<Core::Data::Ast::IntegerLiteral>()) {
    return ref;
  } else if (varType == VarType::STRING && ref->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
    return ref;
  } else if (varType == VarType::TYPE && ref->isDerivedFrom<Spp::Ast::Type>()) {
    return ref;
  } else if (varType == VarType::FUNCTION && ref->isDerivedFrom<Spp::Ast::Function>()) {
    return ref;
  } else {
    helper->getSeeker()->foreach(ref, refNode->getOwner(),
      [=, &result](TiObject *obj, Core::Notices::Notice*)->Core::Data::Seeker::Verb
      {
        if (varType == VarType::INTEGER && obj->isDerivedFrom<Core::Data::Ast::IntegerLiteral>()) {
          result = obj;
          return Core::Data::Seeker::Verb::STOP;
        } else if (varType == VarType::STRING && obj->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
          result = obj;
          return Core::Data::Seeker::Verb::STOP;
        } else if (varType == VarType::TYPE && obj->isDerivedFrom<Spp::Ast::Type>()) {
          result = obj;
          return Core::Data::Seeker::Verb::STOP;
        } else if (varType == VarType::FUNCTION && obj->isDerivedFrom<Spp::Ast::Function>()) {
          result = obj;
          return Core::Data::Seeker::Verb::STOP;
        }
        // TODO: Recurse if the object is an Alias.
        return Core::Data::Seeker::Verb::MOVE;
      }, 0
    );
  }
  return result;
}


//==============================================================================
// Clonable Implementation

SharedPtr<TiObject> Template::clone() const
{
  SharedPtr<Template> newTemplate = std::make_shared<Template>();
  newTemplate->setProdId(this->getProdId());
  newTemplate->setSourceLocation(this->getSourceLocation());
  for (Word i = 0; i < this->varDefs.size(); ++i) {
    newTemplate->setVarDefs(this->getVarDefs());
  }
  newTemplate->setTemplateBody(this->getTemplateBody());
  return newTemplate;
}


//==============================================================================
// Printable Implementation

void Template::print(OutStream &stream, Int indents) const
{
  stream << STR("Template") ;
  Word id = this->getProdId();
  if (id != UNKNOWN_ID) {
    stream << STR(" [") << ID_GENERATOR->getDesc(id) << STR("]");
  }
  // dump var defs
  stream << STR("\n");
  printIndents(stream, indents+1);
  stream << STR("-varDefs:");
  for (Word i = 0; i < this->varDefs.size(); ++i) {
    stream << STR("\n");
    printIndents(stream, indents+2);
    stream << this->varDefs[i].first << STR(": ");
    switch (this->varDefs[i].second.val) {
      case VarType::INTEGER: stream << STR("TiInt"); break;
      case VarType::STRING: stream << STR("TiStr"); break;
      case VarType::TYPE: stream << STR("Type"); break;
      case VarType::FUNCTION: stream << STR("Function"); break;
    }
  }
  // dump body
  stream << STR("\n");
  printIndents(stream, indents+1);
  stream << STR("-templateBody:\n");
  printIndents(stream, indents+2);
  Core::Data::dumpData(stream, this->templateBody->getTiObject(), indents+2);
  // dump instances
  stream << STR("\n");
  printIndents(stream, indents+1);
  stream << STR("-instances:");
  for (Word i = 0; i < this->instances.size(); ++i) {
    stream << STR("\n");
    printIndents(stream, indents+2);
    Core::Data::dumpData(stream, this->instances[i].get(), indents+2);
  }
}

} } // namespace
