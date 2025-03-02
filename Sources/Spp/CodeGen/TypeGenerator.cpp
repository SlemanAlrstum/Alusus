/**
 * @file Spp/CodeGen/TypeGenerator.cpp
 * Contains the implementation of class Spp::CodeGen::TypeGenerator.
 *
 * @copyright Copyright (C) 2023 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"
#include <regex>

namespace Spp { namespace CodeGen
{

//==============================================================================
// Initialization Functions

void TypeGenerator::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->generateType,
    &this->generateVoidType,
    &this->generateIntegerType,
    &this->generateFloatType,
    &this->generatePointerType,
    &this->generateReferenceType,
    &this->generateArrayType,
    &this->generateUserType,
    &this->generateUserTypeMemberVars,
    &this->generateUserTypeAutoConstructor,
    &this->generateUserTypeAutoDestructor,
    &this->generateFunctionType,
    &this->generateCast,
    &this->generateDefaultValue,
    &this->generateDefaultArrayValue,
    &this->generateDefaultUserTypeValue,
    &this->getTypeAllocationSize
  });
}


void TypeGenerator::initBindings()
{
  this->generateType = &TypeGenerator::_generateType;
  this->generateVoidType = &TypeGenerator::_generateVoidType;
  this->generateIntegerType = &TypeGenerator::_generateIntegerType;
  this->generateFloatType = &TypeGenerator::_generateFloatType;
  this->generatePointerType = &TypeGenerator::_generatePointerType;
  this->generateReferenceType = &TypeGenerator::_generateReferenceType;
  this->generateArrayType = &TypeGenerator::_generateArrayType;
  this->generateUserType = &TypeGenerator::_generateUserType;
  this->generateUserTypeMemberVars = &TypeGenerator::_generateUserTypeMemberVars;
  this->generateUserTypeAutoConstructor = &TypeGenerator::_generateUserTypeAutoConstructor;
  this->generateUserTypeAutoDestructor = &TypeGenerator::_generateUserTypeAutoDestructor;
  this->generateFunctionType = &TypeGenerator::_generateFunctionType;
  this->generateCast = &TypeGenerator::_generateCast;
  this->generateDefaultValue = &TypeGenerator::_generateDefaultValue;
  this->generateDefaultArrayValue = &TypeGenerator::_generateDefaultArrayValue;
  this->generateDefaultUserTypeValue = &TypeGenerator::_generateDefaultUserTypeValue;
  this->getTypeAllocationSize = &TypeGenerator::_getTypeAllocationSize;
}


//==============================================================================
// Main Operation Functions

Bool TypeGenerator::getGeneratedType(
  TiObject *ref, Generation *g, Session *session, TiObject *&targetTypeResult, Ast::Type **astTypeResult
) {
  Spp::Ast::Type *astType;
  if (!this->_getGeneratedType(ref, g, session, astType)) return false;
  targetTypeResult = session->getEda()->getCodeGenData<TiObject>(astType);
  if (astTypeResult != 0) {
    *astTypeResult = astType;
  }
  return true;
}


Bool TypeGenerator::getGeneratedVoidType(
  Generation *g, Session *session, TiObject *&tgTypeResult, Ast::VoidType **astTypeResult
) {
  auto voidType = this->astHelper->getVoidType();
  Ast::Type *dummy;
  if (!this->_getGeneratedType(voidType, g, session, dummy)) return false;
  tgTypeResult = session->getEda()->getCodeGenData<TiObject>(voidType);
  if (astTypeResult != 0) {
    *astTypeResult = voidType;
  }
  return true;
}


Bool TypeGenerator::_getGeneratedType(TiObject *ref, Generation *g, Session *session, Spp::Ast::Type *&type)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(ref);
  if (metadata == 0) {
    throw EXCEPTION(GenericException, S("Reference does not contain metadata."));
  }

  Bool shouldPushSl = ref->isDerivedFrom<Spp::Ast::Type>() ? false : true;

  type = this->astHelper->traceType(ref);
  if (type == 0) return false;

  Core::Data::SourceLocation *sourceLocation = 0;
  if (shouldPushSl && metadata->findSourceLocation() != 0) {
    sourceLocation = metadata->findSourceLocation().get();
    this->astHelper->getNoticeStore()->pushPrefixSourceLocation(sourceLocation);
  }
  Bool result = this->generateType(type, g, session);
  if (shouldPushSl && sourceLocation != 0) {
    this->astHelper->getNoticeStore()->popPrefixSourceLocation();
  }

  return result;
}


//==============================================================================
// Code Generation Functions

Bool TypeGenerator::_generateType(TiObject *self, Spp::Ast::Type *astType, Generation *g, Session *session)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);

  auto cgType = session->getEda()->tryGetCodeGenData<TiObject>(astType);
  if (cgType != 0) return true;

  if (astType->isDerivedFrom<Spp::Ast::VoidType>()) {
    return typeGenerator->generateVoidType(static_cast<Spp::Ast::VoidType*>(astType), session);
  } else if (astType->isDerivedFrom<Spp::Ast::IntegerType>()) {
    return typeGenerator->generateIntegerType(static_cast<Spp::Ast::IntegerType*>(astType), session);
  } else if (astType->isDerivedFrom<Spp::Ast::FloatType>()) {
    return typeGenerator->generateFloatType(static_cast<Spp::Ast::FloatType*>(astType), session);
  } else if (astType->isDerivedFrom<Spp::Ast::PointerType>()) {
    return typeGenerator->generatePointerType(static_cast<Spp::Ast::PointerType*>(astType), g, session);
  } else if (astType->isDerivedFrom<Spp::Ast::ReferenceType>()) {
    return typeGenerator->generateReferenceType(static_cast<Spp::Ast::ReferenceType*>(astType), g, session);
  } else if (astType->isDerivedFrom<Spp::Ast::ArrayType>()) {
    return typeGenerator->generateArrayType(static_cast<Spp::Ast::ArrayType*>(astType), g, session);
  } else if (astType->isDerivedFrom<Spp::Ast::UserType>()) {
    return typeGenerator->generateUserType(static_cast<Spp::Ast::UserType*>(astType), g, session);
  } else if (astType->isDerivedFrom<Spp::Ast::FunctionType>()) {
    return typeGenerator->generateFunctionType(static_cast<Spp::Ast::FunctionType*>(astType), g, session);
  } else {
    typeGenerator->astHelper->getNoticeStore()->add(newSrdObj<Spp::Notices::InvalidTypeNotice>());
    return false;
  }
}


Bool TypeGenerator::_generateVoidType(TiObject *self, Spp::Ast::VoidType *astType, Session *session)
{
  TioSharedPtr tgType;
  if (!session->getTg()->generateVoidType(tgType)) return false;
  session->getEda()->setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateIntegerType(TiObject *self, Spp::Ast::IntegerType *astType, Session *session)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto bitCount = astType->getBitCount(typeGenerator->astHelper, session->getExecutionContext());
  // TODO: Support 128 bits?
  if (bitCount != 1 && bitCount != 8 && bitCount != 16 && bitCount != 32 && bitCount != 64) {
    typeGenerator->astHelper->getNoticeStore()->add(newSrdObj<Spp::Notices::InvalidIntegerBitCountNotice>());
    return false;
  }
  TioSharedPtr tgType;
  if (!session->getTg()->generateIntType(bitCount, astType->isSigned(), tgType)) return false;
  session->getEda()->setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateFloatType(TiObject *self, Spp::Ast::FloatType *astType, Session *session)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto bitCount = astType->getBitCount(typeGenerator->astHelper);
  // TODO: Support 128 bits?
  if (bitCount != 32 && bitCount != 64) {
    typeGenerator->astHelper->getNoticeStore()->add(newSrdObj<Spp::Notices::InvalidFloatBitCountNotice>());
    return false;
  }
  TioSharedPtr tgType;
  if (!session->getTg()->generateFloatType(bitCount, tgType)) return false;
  session->getEda()->setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generatePointerType(
  TiObject *self, Spp::Ast::PointerType *astType, Generation *g, Session *session
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto contentAstType = astType->getContentType(typeGenerator->astHelper);
  if (typeGenerator->astHelper->isVoid(contentAstType)) {
    contentAstType = typeGenerator->astHelper->getCharType();
  }
  if (!typeGenerator->generateType(contentAstType, g, session)) return false;
  TiObject *contentTgType = session->getEda()->getCodeGenData<TiObject>(contentAstType);
  TioSharedPtr tgType;
  if (!session->getTg()->generatePointerType(contentTgType, tgType)) return false;
  session->getEda()->setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateReferenceType(
  TiObject *self, Spp::Ast::ReferenceType *astType, Generation *g, Session *session
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto contentAstType = astType->getContentType(typeGenerator->astHelper);
  if (typeGenerator->astHelper->isVoid(contentAstType)) {
    contentAstType = typeGenerator->astHelper->getCharType();
  }
  if (!typeGenerator->generateType(contentAstType, g, session)) return false;
  TiObject *contentTgType = session->getEda()->getCodeGenData<TiObject>(contentAstType);
  TioSharedPtr tgType;
  if (!session->getTg()->generatePointerType(contentTgType, tgType)) return false;
  session->getEda()->setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateArrayType(TiObject *self, Spp::Ast::ArrayType *astType, Generation *g, Session *session)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto contentAstType = astType->getContentType(typeGenerator->astHelper);
  if (!typeGenerator->generateType(contentAstType, g, session)) return false;
  TiObject *contentTgType = session->getEda()->getCodeGenData<TiObject>(contentAstType);
  auto size = astType->getSize(typeGenerator->astHelper);
  TioSharedPtr tgType;
  if (!session->getTg()->generateArrayType(contentTgType, size, tgType)) return false;
  session->getEda()->setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateUserType(TiObject *self, Spp::Ast::UserType *astType, Generation *g, Session *session)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto name = std::regex_replace(
    typeGenerator->astHelper->resolveNodePath(astType).getBuf(), std::regex("[^a-zA-Z0-9_]"), S("_")
  );

  if (!typeGenerator->astProcessor->processTypeBody(astType)) return false;

  // Did this type get generated during preprocessing? If so it means we ran into circular code generation.
  if (session->getEda()->tryGetCodeGenData<TiObject>(astType) != 0) {
    typeGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::CircularUserTypeCodeGenNotice>(astType->findSourceLocation())
    );
    return false;
  }

  TioSharedPtr tgType;
  if (!session->getTg()->generateStructTypeDecl(name.c_str(), tgType)) return false;
  session->getEda()->setCodeGenData(astType, tgType);
  if (!typeGenerator->generateUserTypeMemberVars(astType, g, session)) return false;
  if (!typeGenerator->generateUserTypeAutoConstructor(astType, g, session)) return false;
  if (!typeGenerator->generateUserTypeAutoDestructor(astType, g, session)) return false;
  return true;
}


Bool TypeGenerator::_generateUserTypeMemberVars(
  TiObject *self, Spp::Ast::UserType *astType, Generation *g, Session *session
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  TiObject *tgType = session->getEda()->tryGetCodeGenData<TiObject>(astType);
  ASSERT(tgType != 0);

  // Prepare struct members.
  auto body = astType->getBody().get();
  if (body == 0) {
    throw EXCEPTION(GenericException, S("User type missing body block."));
  }
  auto prevInProgress = session->getEda()->tryGetCodeGenData<TiBool>(body);
  if (prevInProgress != 0) {
    if (prevInProgress->get()) {
      typeGenerator->astHelper->getNoticeStore()->add(
        newSrdObj<Spp::Notices::CircularUserTypeDefinitionsNotice>(astType->findSourceLocation())
      );
      return false;
    } else {
      return true;
    }
  }
  auto inProgress = TiBool::create(true);
  session->getEda()->setCodeGenData(body, inProgress);

  // Generate the structure.
  Bool result = true;
  PlainList<TiObject> members;
  PlainMap<TiObject> tgMemberTypes;
  SharedList<TiObject> tgMembers;
  for (Int i = 0; i < body->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(body->getElement(i));
    if (def != 0) {
      auto obj = def->getTarget().get();
      if (typeGenerator->astHelper->isInMemVariable(obj)) {
        if (typeGenerator->astHelper->getVariableDomain(def) == Ast::DefinitionDomain::OBJECT) {
          TiObject *tgType;
          Ast::Type *astMemberType;
          if (!typeGenerator->getGeneratedType(obj, g, session, tgType, &astMemberType)) {
            result = false;
            continue;
          }
          Ast::setAstType(obj, astMemberType);
          // Make sure the name is unique.
          Str defName = def->getName().getStr();
          for (LongInt i = 0; tgMemberTypes.findIndex(defName.getBuf()) != -1; ++i) {
            defName = def->getName().getStr() + i;
          }
          tgMemberTypes.add(defName.getBuf(), tgType);
          members.add(obj);
        } else {
          if (Spp::Ast::isInjection(def)) {
            typeGenerator->astHelper->getNoticeStore()->add(
              newSrdObj<Spp::Notices::SharedInjectionNotice>(def->findSourceLocation())
            );
            result = false;
          }
        }
      } else if (typeGenerator->astHelper->isValueOnlyVariable(obj)) {
        continue;
      } else if (obj->isDerivedFrom<Core::Data::Ast::Bridge>()) {
        if (!typeGenerator->astHelper->validateUseStatement(static_cast<Core::Data::Ast::Bridge*>(obj))) {
          result = false;
        }
        continue;
      } else {
        if (Spp::Ast::isInjection(def)) {
          typeGenerator->astHelper->getNoticeStore()->add(
            newSrdObj<Spp::Notices::InvalidInjectionTypeNotice>(def->findSourceLocation())
          );
          result = false;
        }
      }
    }
  }

  // Even some members fail, we'll still try to generate a target struct with whatever members we have so that we don't
  // end up with a struct that has no body, which can cause exceptions later on during other operations.
  if (!session->getTg()->generateStructTypeBody(tgType, &tgMemberTypes, &tgMembers)) return false;

  if (!result) return false;

  if (tgMemberTypes.getElementCount() != tgMembers.getCount()) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating struct body."));
  }
  for (Int i = 0; i < tgMemberTypes.getElementCount(); ++i) {
    session->getEda()->setCodeGenData(members.get(i), tgMembers.get(i));
  }
  inProgress->set(false);

  return true;
}


Bool TypeGenerator::_generateUserTypeAutoConstructor(
  TiObject *self, Spp::Ast::UserType *astType, Generation *g, Session *session
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  // Validate that the type has custom initialization.
  if ((astType->getInitializationMethod(
    typeGenerator->astHelper, session->getExecutionContext()
  ) & Ast::TypeInitMethod::AUTO) == 0) return true;

  // Generate pointer type for the user type.
  auto astPtrType = typeGenerator->astHelper->getPointerTypeFor(astType);
  TiObject *tgPtrType;
  if (!typeGenerator->getGeneratedType(astPtrType, g, session, tgPtrType, 0)) return false;

  // Generate function type.
  TioSharedPtr tgVoidType;
  if (!session->getTg()->generateVoidType(tgVoidType)) {
    throw EXCEPTION(GenericException, S("Failed to generate LLVM void type."));
  }
  PlainMap<TiObject> argTypes;
  argTypes.add(S("this"), tgPtrType);
  TioSharedPtr tgFuncType;
  if (!session->getTg()->generateFunctionType(&argTypes, tgVoidType.get(), false, tgFuncType)) {
    throw EXCEPTION(GenericException, S("Failed to generate function type for type constructor."));
  }

  // Prepare the constructor function.
  auto funcName = typeGenerator->astHelper->resolveNodePath(astType) + S(".__autoConstruct__");
  TioSharedPtr tgFunc;
  if (!session->getTg()->generateFunctionDecl(funcName, tgFuncType.get(), tgFunc)) {
    throw EXCEPTION(GenericException, S("Failed to generate function declaration for type constructor."));
  }
  TioSharedPtr tgContext;
  SharedList<TiObject> args;
  if (!session->getTg()->prepareFunctionBody(tgFunc.get(), tgFuncType.get(), &args, tgContext)) {
    throw EXCEPTION(GenericException, S("Failed to prepare function body for type constructor."));
  }

  // Prepare dependencies.
  DestructionStack destructionStack;
  Session childSession(session, tgContext.get(), tgContext.get(), &destructionStack);
  childSession.setTgSelf(args.get(0));
  childSession.setAstSelfType(astType);

  auto body = astType->getBody().get();
  session->getEda()->setCodeGenData(body, tgContext);

  auto astThisType = typeGenerator->getAstHelper()->getReferenceTypeFor(astType, Ast::ReferenceMode::EXPLICIT);
  g->addThisDefinition(body, S("this"), astThisType, false, args.get(0), &childSession);

  // Main loop.
  for (Int i = 0; i < body->getElementCount(); ++i) {
    auto obj = body->getElement(i);
    auto def = ti_cast<Core::Data::Ast::Definition>(obj);
    if (def != 0) {
      auto target = def->getTarget().get();
      if (
        typeGenerator->getAstHelper()->isInMemVariable(target) &&
        typeGenerator->getAstHelper()->getVariableDomain(def) == Ast::DefinitionDomain::OBJECT
      ) {
        // Initialize member variable.
        if (!g->generateMemberVarInitialization(target, &childSession)) return false;
      }
    } else {
      // Generate regular statement.
      TerminalStatement terminal;
      if (!g->generateStatement(obj, &childSession, terminal)) return false;
    }
  }

  // Finish the function.
  if (!childSession.getTg()->finishFunctionBody(tgFunc.get(), tgFuncType.get(), &args, tgContext.get())) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for type constructor."));
  }

  session->getEda()->removeCodeGenData(body);

  // Assign the function to the type.
  session->getEda()->setAutoCtor(astType, tgFunc);
  session->getEda()->setAutoCtorType(astType, tgFuncType);

  return true;
}


Bool TypeGenerator::_generateUserTypeAutoDestructor(
  TiObject *self, Spp::Ast::UserType *astType, Generation *g, Session *session
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  // Validate that the type has custom initialization.
  if ((astType->getDestructionMethod(
    typeGenerator->astHelper, session->getExecutionContext()
  ) & Ast::TypeInitMethod::AUTO) == 0) return true;

  // Generate pointer type for the user type.
  auto astPtrType = typeGenerator->astHelper->getPointerTypeFor(astType);
  TiObject *tgPtrType;
  if (!typeGenerator->getGeneratedType(astPtrType, g, session, tgPtrType, 0)) return false;

  // Generate function type.
  TioSharedPtr tgVoidType;
  if (!session->getTg()->generateVoidType(tgVoidType)) {
    throw EXCEPTION(GenericException, S("Failed to generate LLVM void type."));
  }
  PlainMap<TiObject> argTypes;
  argTypes.add(S("this"), tgPtrType);
  TioSharedPtr tgFuncType;
  if (!session->getTg()->generateFunctionType(&argTypes, tgVoidType.get(), false, tgFuncType)) {
    throw EXCEPTION(GenericException, S("Failed to generate function type for type destructor."));
  }

  // Prepare the destructor function.
  auto funcName = typeGenerator->astHelper->resolveNodePath(astType) + S(".__autoDestruct__");
  TioSharedPtr tgFunc;
  if (!session->getTg()->generateFunctionDecl(funcName, tgFuncType.get(), tgFunc)) {
    throw EXCEPTION(GenericException, S("Failed to generate function declaration for type destructor."));
  }
  TioSharedPtr tgContext;
  SharedList<TiObject> args;
  if (!session->getTg()->prepareFunctionBody(tgFunc.get(), tgFuncType.get(), &args, tgContext)) {
    throw EXCEPTION(GenericException, S("Failed to prepare function body for type destructor."));
  }

  // Prepare dependencies.
  DestructionStack destructionStack;
  Session childSession(session, tgContext.get(), tgContext.get(), &destructionStack);
  childSession.setTgSelf(args.get(0));
  childSession.setAstSelfType(astType);

  // Main loop.
  auto body = astType->getBody().get();
  for (Int i = 0; i < body->getCount(); ++i) {
    auto obj = body->getElement(i);
    auto def = ti_cast<Core::Data::Ast::Definition>(obj);
    if (def != 0) {
      auto target = def->getTarget().get();
      if (
        typeGenerator->getAstHelper()->isInMemVariable(target) &&
        typeGenerator->getAstHelper()->getVariableDomain(def) == Ast::DefinitionDomain::OBJECT
      ) {
        // Destruct member variable.
        if (!g->generateMemberVarDestruction(target, &childSession)) return false;
      }
    }
  }

  // Finish the function.
  if (!childSession.getTg()->finishFunctionBody(tgFunc.get(), tgFuncType.get(), &args, tgContext.get())) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for type destructor."));
  }

  // Assign the function to the type.
  session->getEda()->setAutoDtor(astType, tgFunc);
  session->getEda()->setAutoDtorType(astType, tgFuncType);

  return true;
}


Bool TypeGenerator::_generateFunctionType(
  TiObject *self, Spp::Ast::FunctionType *astType, Generation *g, Session *session
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  // Construct the list of argument TG types.
  auto astArgs = astType->getArgTypes().get();
  auto argCount = astArgs == 0 ? 0 : astArgs->getCount();
  PlainMap<TiObject> tgArgs;
  for (Int i = 0; i < argCount; ++i) {
    auto argType = astArgs->getElement(i);
    if (argType->isDerivedFrom<Ast::ArgPack>()) break;
    TiObject *tgType;
    Ast::Type *astType;
    if (!typeGenerator->getGeneratedType(argType, g, session, tgType, &astType)) {
      return false;
    }
    if (astType->getInitializationMethod(
      typeGenerator->astHelper, session->getExecutionContext()
    ) != Ast::TypeInitMethod::NONE) {
      // This type has custom initialization, so we'll pass it by reference instead of by value.
      auto astPtrType = typeGenerator->astHelper->getPointerTypeFor(astType);
      if (!typeGenerator->getGeneratedType(astPtrType, g, session, tgType, 0)) {
        return false;
      }
    }
    tgArgs.add(astArgs->getKey(i), tgType);
    Ast::setAstType(argType, astType);
  }

  // Get the return TG type.
  TiObject *tgRetType = 0;
  if (astType->getRetType() != 0) {
    Ast::Type *astRetType;
    if (!typeGenerator->getGeneratedType(astType->getRetType().get(), g, session, tgRetType, &astRetType)) {
      return false;
    }
    Ast::setAstType(astType->getRetType().get(), astRetType);
    if (astRetType->getInitializationMethod(
      typeGenerator->astHelper, session->getExecutionContext()
    ) != Ast::TypeInitMethod::NONE) {
      // This type has custom initialization, so we'll pass it by reference instead of by value.
      auto astRetPtrType = typeGenerator->astHelper->getPointerTypeFor(astRetType);
      if (!typeGenerator->getGeneratedType(astRetPtrType, g, session, tgRetType, 0)) {
        return false;
      }
      tgArgs.insert(0, S("#ret"), tgRetType);
      if (!typeGenerator->getGeneratedVoidType(g, session, tgRetType, 0)) return false;
    }
  } else {
    if (!typeGenerator->getGeneratedVoidType(g, session, tgRetType, 0)) return false;
  }

  // Generate the type.
  TioSharedPtr tgFuncType;
  if (!session->getTg()->generateFunctionType(&tgArgs, tgRetType, astType->isVariadic(), tgFuncType)) return false;
  session->getEda()->setCodeGenData(astType, tgFuncType);
  return true;
}


Bool TypeGenerator::_generateCast(
  TiObject *self, Generation *g, Session *session, Spp::Ast::Type *srcType, Spp::Ast::Type *targetType,
  Core::Data::Node *astNode, TiObject *tgValue, Bool implicit, GenResult &result
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);
  Ast::Function *caster;
  auto matchType = typeGenerator->astHelper->matchTargetType(
    srcType, targetType, session->getExecutionContext(), caster
  );

  // If this type is a pass-by-ref type then we should not dereference all the way to a value since we will be needing
  // a reference to the value and not the value itself.
  if (matchType.derefs == 1 && matchType == Ast::TypeMatchStatus::EXACT) {
    if (targetType->getInitializationMethod(
      typeGenerator->astHelper, session->getExecutionContext()
    ) != Ast::TypeInitMethod::NONE) {
      result.targetData = getSharedPtr(tgValue);
      result.astType = srcType;
      return true;
    }
  }

  if (matchType.derefs > 0) {
    // Dereference tgValue.
    auto srcReferenceType = ti_cast<Spp::Ast::ReferenceType>(srcType);
    if (srcReferenceType == 0) {
      throw EXCEPTION(GenericException, S("Invalid TypeMatchStatus value."));
    }
    auto srcContentType = srcReferenceType->getContentType(typeGenerator->astHelper);
    auto tgContentType = session->getEda()->getCodeGenData<TiObject>(srcContentType);
    TioSharedPtr tgDerefVal;
    if (!session->getTg()->generateDereference(session->getTgContext(), tgContentType, tgValue, tgDerefVal)) return false;
    return typeGenerator->generateCast(
      g, session, srcContentType, targetType, astNode, tgDerefVal.get(), implicit, result
    );
  } else if (matchType.derefs == -1) {
    // The match type returned a negative deref, which means we have a value when we actually need a reference, so we
    // will store the value into a temp var and get a reference to that.
    TioSharedPtr tgTempVar;
    if (!g->generateTempVar(astNode, srcType, session, false, tgTempVar)) return false;
    PlainList<TiObject> paramAstNodes({ astNode });
    PlainList<TiObject> paramAstTypes({ srcType });
    SharedList<TiObject> paramTgValues({ getSharedPtr(tgValue) });
    TioSharedPtr tgRef;
    if (!session->getTg()->generateVarReference(
      session->getTgContext(), session->getEda()->getCodeGenData<TiObject>(srcType),
      tgTempVar.get(), tgRef
    )) return false;
    if (!g->generateVarInitialization(
      srcType, tgRef.get(), astNode, &paramAstNodes, &paramAstTypes, &paramTgValues, session
    )) return false;
    // Register temp var for destruction.
    g->registerDestructor(astNode, srcType, tgTempVar, session->getExecutionContext(), session->getDestructionStack());
    Ast::Type *refAstType = typeGenerator->astHelper->getReferenceTypeFor(srcType, Ast::ReferenceMode::IMPLICIT);
    return typeGenerator->generateCast(
      g, session, refAstType, targetType, astNode, tgRef.get(), implicit, result
    );
  }

  if (matchType <= Ast::TypeMatchStatus::EXPLICIT_CAST && implicit) {
    return false;
  } else if (matchType == Ast::TypeMatchStatus::EXACT) {
    // Same type, return value as is.
    result.targetData = getSharedPtr(tgValue);
    result.astType = srcType;
    return true;
  } else if (matchType == Ast::TypeMatchStatus::CUSTOM_CASTER) {
    // Call the caster.
    GenResult callResult;
    PlainList<TiObject> paramTgValues({ tgValue });
    PlainList<TiObject> paramAstTypes({ srcType });
    if (!g->generateFunctionCall(astNode, caster, &paramAstTypes, &paramTgValues, session, callResult)) return false;
    return typeGenerator->generateCast(
      g, session, callResult.astType, targetType, astNode, callResult.targetData.get(), implicit, result
    );
  } else if (srcType->isDerivedFrom<Spp::Ast::IntegerType>()) {
    // Casting from integer.
    auto srcIntegerType = static_cast<Spp::Ast::IntegerType*>(srcType);
    TiObject *srcTgType;
    if (!typeGenerator->getGeneratedType(srcIntegerType, g, session, srcTgType, 0)) return false;
    if (targetType->isDerivedFrom<Spp::Ast::IntegerType>()) {
      // Cast from integer to another integer.
      auto targetIntegerType = static_cast<Spp::Ast::IntegerType*>(targetType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetIntegerType, g, session, targetTgType, 0)) return false;
      if (!session->getTg()->generateCastIntToInt(
        session->getTgContext(), srcTgType, targetTgType, tgValue, result.targetData
      )) return false;
      result.astType = targetType;
      return true;
    } else if (targetType->isDerivedFrom<Spp::Ast::FloatType>()) {
      // Cast from integer to float.
      auto targetFloatType = static_cast<Spp::Ast::FloatType*>(targetType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetFloatType, g, session, targetTgType, 0)) return false;
      if (!session->getTg()->generateCastIntToFloat(
        session->getTgContext(), srcTgType, targetTgType, tgValue, result.targetData
      )) return false;
      result.astType = targetType;
      return true;
    } else if (targetType->isDerivedFrom<Spp::Ast::PointerType>()) {
      // Cast from integer to pointer.
      auto targetPointerType = static_cast<Spp::Ast::PointerType*>(targetType);
      if (
        srcIntegerType->isNullLiteral() ||
        srcIntegerType->getBitCount(typeGenerator->astHelper, session->getExecutionContext())
          == session->getExecutionContext()->getPointerBitCount()
      ) {
        TiObject *targetTgType;
        if (!typeGenerator->getGeneratedType(targetPointerType, g, session, targetTgType, 0)) return false;
        if (!session->getTg()->generateCastIntToPointer(
          session->getTgContext(), srcTgType, targetTgType, tgValue, result.targetData
        )) return false;
        result.astType = targetType;
        return true;
      }
    }
  } else if (srcType->isDerivedFrom<Spp::Ast::FloatType>()) {
    // Casting from float.
    auto srcFloatType = static_cast<Spp::Ast::FloatType*>(srcType);
    TiObject *srcTgType;
    if (!typeGenerator->getGeneratedType(srcFloatType, g, session, srcTgType, 0)) return false;
    if (targetType->isDerivedFrom<Spp::Ast::IntegerType>()) {
      // Cast from float to integer.
      auto targetIntegerType = static_cast<Spp::Ast::IntegerType*>(targetType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetIntegerType, g, session, targetTgType, 0)) return false;
      if (!session->getTg()->generateCastFloatToInt(
        session->getTgContext(), srcTgType, targetTgType, tgValue, result.targetData
      )) return false;
      result.astType = targetType;
      return true;
    } else if (targetType->isDerivedFrom<Spp::Ast::FloatType>()) {
      // Cast from float to another float.
      auto targetFloatType = static_cast<Spp::Ast::FloatType*>(targetType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetFloatType, g, session, targetTgType, 0)) return false;
      if (!session->getTg()->generateCastFloatToFloat(
        session->getTgContext(), srcTgType, targetTgType, tgValue, result.targetData
      )) return false;
      result.astType = targetType;
      return true;
    }
  } else if (srcType->isDerivedFrom<Spp::Ast::PointerType>()) {
    // Casting from pointer.
    if (targetType->isDerivedFrom<Spp::Ast::IntegerType>()) {
      // Cast pointer to integer.
      auto targetIntegerType = static_cast<Spp::Ast::IntegerType*>(targetType);
      Word targetBitCount = targetIntegerType->getBitCount(typeGenerator->astHelper, session->getExecutionContext());
      if (session->getExecutionContext()->getPointerBitCount() == targetBitCount) {
        TiObject *srcTgType;
        if (!typeGenerator->getGeneratedType(srcType, g, session, srcTgType, 0)) return false;
        TiObject *targetTgType;
        if (!typeGenerator->getGeneratedType(targetIntegerType, g, session, targetTgType, 0)) return false;
        if (!session->getTg()->generateCastPointerToInt(
          session->getTgContext(), srcTgType, targetTgType, tgValue, result.targetData
        )) return false;
        result.astType = targetType;
        return true;
      }
    } else if (targetType->isDerivedFrom<Spp::Ast::PointerType>()) {
      // Cast pointer to another pointer.
      auto targetPointerType = static_cast<Spp::Ast::PointerType*>(targetType);
      TiObject *srcTgType;
      if (!typeGenerator->getGeneratedType(srcType, g, session, srcTgType, 0)) return false;
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetPointerType, g, session, targetTgType, 0)) return false;
      if (!session->getTg()->generateCastPointerToPointer(
        session->getTgContext(), srcTgType, targetTgType, tgValue, result.targetData
      )) return false;
      result.astType = targetType;
      return true;
    }
  } else if (srcType->isDerivedFrom<Spp::Ast::ReferenceType>()) {
    // Casting from reference.
    if (targetType->isDerivedFrom<Spp::Ast::ReferenceType>()) {
      // Casting from reference to another reference
      auto targetReferenceType = static_cast<Spp::Ast::ReferenceType*>(targetType);
      TiObject *srcTgType;
      if (!typeGenerator->getGeneratedType(srcType, g, session, srcTgType, 0)) return false;
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetReferenceType, g, session, targetTgType, 0)) return false;
      if (!session->getTg()->generateCastPointerToPointer(
        session->getTgContext(), srcTgType, targetTgType, tgValue, result.targetData
      )) return false;
      result.astType = targetType;
      return true;
    }
  }

  return false;
}


Bool TypeGenerator::_generateDefaultValue(
  TiObject *self, Spp::Ast::Type *astType, Generation *g, Session *session, TioSharedPtr &result
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  if (astType->isDerivedFrom<Spp::Ast::IntegerType>()) {
    // Generate integer 0
    auto tgType = session->getEda()->tryGetCodeGenData<TiObject>(astType);
    if (tgType == 0) {
      if (!typeGenerator->generateType(astType, g, session)) return false;
      tgType = session->getEda()->getCodeGenData<TiObject>(astType);
    }

    auto integerType = static_cast<Spp::Ast::IntegerType*>(astType);
    auto bitCount = integerType->getBitCount(typeGenerator->astHelper, session->getExecutionContext());
    return session->getTg()->generateIntLiteral(session->getTgContext(), bitCount, integerType->isSigned(), 0, result);
  } else if (astType->isDerivedFrom<Spp::Ast::FloatType>()) {
    // Generate float 0
    auto tgType = session->getEda()->tryGetCodeGenData<TiObject>(astType);
    if (tgType == 0) {
      if (!typeGenerator->generateType(astType, g, session)) return false;
      tgType = session->getEda()->getCodeGenData<TiObject>(astType);
    }

    auto floatType = static_cast<Spp::Ast::FloatType*>(astType);
    auto bitCount = floatType->getBitCount(typeGenerator->astHelper);
    return session->getTg()->generateFloatLiteral(session->getTgContext(), bitCount, (Double)0, result);
  } else if (astType->isDerivedFrom<Spp::Ast::PointerType>()) {
    // Generate pointer null
    auto tgType = session->getEda()->tryGetCodeGenData<TiObject>(astType);
    if (tgType == 0) {
      if (!typeGenerator->generateType(astType, g, session)) return false;
      tgType = session->getEda()->getCodeGenData<TiObject>(astType);
    }

    return session->getTg()->generateNullPtrLiteral(session->getTgContext(), tgType, result);
  } else if (astType->isDerivedFrom<Spp::Ast::ReferenceType>()) {
    // Generate pointer null
    auto tgType = session->getEda()->tryGetCodeGenData<TiObject>(astType);
    if (tgType == 0) {
      if (!typeGenerator->generateType(astType, g, session)) return false;
      tgType = session->getEda()->getCodeGenData<TiObject>(astType);
    }

    return session->getTg()->generateNullPtrLiteral(session->getTgContext(), tgType, result);
  } else if (astType->isDerivedFrom<Spp::Ast::ArrayType>()) {
    // Generate zeroed out array.
    return typeGenerator->generateDefaultArrayValue(static_cast<Ast::ArrayType*>(astType), g, session, result);
  } else if (astType->isDerivedFrom<Spp::Ast::UserType>()) {
    // Generate zeroed out structure.
    return typeGenerator->generateDefaultUserTypeValue(static_cast<Ast::UserType*>(astType), g, session, result);
  } else {
    throw EXCEPTION(GenericException, S("Invlid type for generation of default value."));
  }
}


Bool TypeGenerator::_generateDefaultArrayValue(
  TiObject *self, Spp::Ast::ArrayType *astType, Generation *g, Session *session, TioSharedPtr &result
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  auto tgType = session->getEda()->tryGetCodeGenData<TiObject>(astType);
  if (tgType == 0) {
    if (!typeGenerator->generateType(astType, g, session)) return false;
    tgType = session->getEda()->getCodeGenData<TiObject>(astType);
  }

  auto elementAstType = astType->getContentType(typeGenerator->astHelper);
  auto elementTgType = session->getEda()->tryGetCodeGenData<TiObject>(elementAstType);
  if (elementTgType == 0) {
    if (!typeGenerator->generateType(elementAstType, g, session)) return false;
    elementTgType = session->getEda()->getCodeGenData<TiObject>(elementAstType);
  }

  TioSharedPtr elementVal;
  if (!typeGenerator->generateDefaultValue(elementAstType, g, session, elementVal)) return false;

  auto size = astType->getSize(typeGenerator->astHelper);
  SharedList<TiObject> memberVals;
  for (Word i = 0; i < size; ++i) {
    memberVals.add(elementVal);
  }

  return session->getTg()->generateArrayLiteral(session->getTgContext(), tgType, &memberVals, result);
}


Bool TypeGenerator::_generateDefaultUserTypeValue(
  TiObject *self, Spp::Ast::UserType *astType, Generation *g, Session *session, TioSharedPtr &result
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  auto tgType = session->getEda()->tryGetCodeGenData<TiObject>(astType);
  if (tgType == 0) {
    if (!typeGenerator->generateType(astType, g, session)) return false;
    tgType = session->getEda()->getCodeGenData<TiObject>(astType);
  }

  auto body = astType->getBody().get();
  SharedList<TiObject> memberVals;
  PlainMap<TiObject> memberTypes;
  for (Int i = 0; i < body->getElementCount(); ++i) {
    auto def = ti_cast<Core::Data::Ast::Definition>(body->getElement(i));
    if (def != 0) {
      auto obj = def->getTarget().get();
      if (typeGenerator->getAstHelper()->isInMemVariable(obj)) {
        if (typeGenerator->getAstHelper()->getVariableDomain(def) == Ast::DefinitionDomain::OBJECT) {
          TiObject *tgMemberType;
          Ast::Type *astMemberType;
          if (!typeGenerator->getGeneratedType(obj, g, session, tgMemberType, &astMemberType)) return false;
          memberTypes.addElement(def->getName().get(), tgMemberType);
          TioSharedPtr memberVal;
          if (!typeGenerator->generateDefaultValue(astMemberType, g, session, memberVal)) return false;
          memberVals.add(memberVal);
        }
      }
    }
  }
  return session->getTg()->generateStructLiteral(session->getTgContext(), tgType, &memberTypes, &memberVals, result);
}


Bool TypeGenerator::_getTypeAllocationSize(
  TiObject *self, Spp::Ast::Type *astType, Generation *g, Session *session, Word &result
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);
  TiObject *tgType;
  if (!typeGenerator->getGeneratedType(astType, g, session, tgType, 0)) return false;
  result = session->getTg()->getTypeAllocationSize(tgType);
  return true;
}

} } // namespace
