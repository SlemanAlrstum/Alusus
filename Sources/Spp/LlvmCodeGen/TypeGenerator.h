/**
 * @file Spp/LlvmCodeGen/TypeGenerator.h
 * Contains the header of class Spp::LlvmCodeGen::TypeGenerator.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_TYPEGENERATOR_H
#define SPP_LLVMCODEGEN_TYPEGENERATOR_H

namespace Spp { namespace LlvmCodeGen
{

class TypeGenerator : public TiObject, public virtual DynamicBindings, public virtual DynamicInterfaces
{
  //============================================================================
  // Type Info

  TYPE_INFO(TypeGenerator, TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.net", (
    INHERITANCE_INTERFACES(DynamicBindings, DynamicInterfaces),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACES(interfaceList);


  //============================================================================
  // Member Variables

  private: Generator *generator;


  //============================================================================
  // Constructors & Destructor

  public: TypeGenerator(Generator *g = 0) : generator(g)
  {
    this->initBindingCaches();
    this->initBindings();
  }

  public: TypeGenerator(TypeGenerator *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->generator = parent->getGenerator();
  }

  public: virtual ~TypeGenerator()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  private: void initBindingCaches();
  private: void initBindings();

  public: void setGenerator(Generator *g)
  {
    this->generator = g;
  }

  public: Generator* getGenerator() const
  {
    return this->generator;
  }

  /// @}

  /// @name Main Operation Functions
  /// @{

  public: Bool getGeneratedType(TiObject *ref, Spp::Ast::Type *&type);
  public: Bool getGeneratedLlvmType(TiObject *ref, llvm::Type *&llvmTypeResult, Ast::Type **astTypeResult = 0);

  /// @}

  /// @name Code Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateType, Bool, (Spp::Ast::Type*));
  private: static Bool _generateType(TiObject *self, Spp::Ast::Type *astType);

  public: METHOD_BINDING_CACHE(generateVoidType, Bool, (Spp::Ast::VoidType*));
  private: static Bool _generateVoidType(TiObject *self, Spp::Ast::VoidType *astType);

  public: METHOD_BINDING_CACHE(generateIntegerType, Bool, (Spp::Ast::IntegerType*));
  private: static Bool _generateIntegerType(TiObject *self, Spp::Ast::IntegerType *astType);

  public: METHOD_BINDING_CACHE(generateFloatType, Bool, (Spp::Ast::FloatType*));
  private: static Bool _generateFloatType(TiObject *self, Spp::Ast::FloatType *astType);

  public: METHOD_BINDING_CACHE(generatePointerType, Bool, (Spp::Ast::PointerType*));
  private: static Bool _generatePointerType(TiObject *self, Spp::Ast::PointerType *astType);

  public: METHOD_BINDING_CACHE(generateArrayType, Bool, (Spp::Ast::ArrayType*));
  private: static Bool _generateArrayType(TiObject *self, Spp::Ast::ArrayType *astType);

  public: METHOD_BINDING_CACHE(generateUserType, Bool, (Spp::Ast::UserType*));
  private: static Bool _generateUserType(TiObject *self, Spp::Ast::UserType *astType);

  public: METHOD_BINDING_CACHE(generateCast,
    Bool, (llvm::IRBuilder<>*, Spp::Ast::Type*, Spp::Ast::Type*, llvm::Value*, llvm::Value*&)
  );
  public: static Bool _generateCast(
    TiObject *self, llvm::IRBuilder<> *llvmIrBuilder, Spp::Ast::Type *srcType, Spp::Ast::Type *targetType,
    llvm::Value *llvmValue, llvm::Value *&llvmCastedValue
  );

  public: METHOD_BINDING_CACHE(dereferenceIfNeeded,
    void, (llvm::IRBuilder<>*, Spp::Ast::Type*, llvm::Value*, Spp::Ast::Type*&, llvm::Value*&)
  );
  private: static void _dereferenceIfNeeded(
    TiObject *self, llvm::IRBuilder<> *llvmIrBuilder, Spp::Ast::Type *astType, llvm::Value *llvmValue,
    Spp::Ast::Type *&resultAstType, llvm::Value *&llvmResult
  );

  public: METHOD_BINDING_CACHE(generateDefaultValue, Bool, (Spp::Ast::Type*, llvm::Constant*&));
  private: static Bool _generateDefaultValue(TiObject *self, Spp::Ast::Type *astType, llvm::Constant *&result);

  /// @}

}; // class

} } // namespace

#endif
