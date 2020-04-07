/**
 * @file Spp/RootManagerExtension.h
 * Contains the header of class Spp::RootManagerExtension.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_ROOTMANAGEREXTENSION_H
#define SPP_ROOTMANAGEREXTENSION_H

namespace Spp
{

class RootManagerExtension : public ObjTiInterface
{
  //============================================================================
  // Type Info

  OBJ_INTERFACE_INFO(RootManagerExtension, ObjTiInterface, "Spp", "Spp", "alusus.org");


  //============================================================================
  // Types

  public: struct Overrides
  {
    TiFunctionBase *dumpLlvmIrForElementRef;
    TiFunctionBase *buildObjectFileForElementRef;
    TiFunctionBase *importFileRef;
    TiFunctionBase *getModifierStringsRef;
  };


  //============================================================================
  // Member Variables

  private: TiObject *owner;


  //============================================================================
  // Constructor

  public: RootManagerExtension(TiObject *o) : owner(o)
  {
    Basic::initBindingCaches(this->owner, {
      &this->dumpLlvmIrForElement,
      &this->buildObjectFileForElement,
      &this->importFile,
      &this->getModifierStrings,
      &this->rootExecBuildManager,
      &this->outputBuildManager,
      &this->astProcessingBuildManager
    });
  }


  //============================================================================
  // Member Properties

  public: BINDING_CACHE(rootExecBuildManager, BuildManager);
  public: BINDING_CACHE(outputBuildManager, BuildManager);
  public: BINDING_CACHE(astProcessingBuildManager, BuildManager);


  //============================================================================
  // Member Functions

  /// @name ObjTiInterface Implementation
  /// @{

  public: virtual TiObject* getTiObject()
  {
    return this->owner;
  }

  public: virtual TiObject const* getTiObject() const
  {
    return this->owner;
  }

  /// @}

  /// @name Setup Functions
  /// @{

  public: static Overrides* extend(
    Core::Main::RootManager *rootManager,
    SharedPtr<BuildManager> const &rootExecBuildManager,
    SharedPtr<BuildManager> const &outputBuildManager,
    SharedPtr<BuildManager> const &astProcessingBuildManager
  );
  public: static void unextend(Core::Main::RootManager *rootManager, Overrides *overrides);

  /// @}

  /// @name Main Functions
  /// @{

  public: METHOD_BINDING_CACHE(dumpLlvmIrForElement,
    void, (TiObject*, Core::Notices::Store*, Core::Processing::Parser*)
  );
  public: static void _dumpLlvmIrForElement(
    TiObject *self, TiObject *element, Core::Notices::Store *noticeStore, Core::Processing::Parser *parser
  );

  public: METHOD_BINDING_CACHE(buildObjectFileForElement,
    Bool, (TiObject*, Char const*, Core::Notices::Store*, Core::Processing::Parser*)
  );
  public: static Bool _buildObjectFileForElement(
    TiObject *self, TiObject *element, Char const *objectFilename, Core::Notices::Store *noticeStore,
    Core::Processing::Parser *parser
  );

  public: METHOD_BINDING_CACHE(importFile, void, (Char const*));
  public: static void _importFile(TiObject *self, Char const *filename);

  public: METHOD_BINDING_CACHE(getModifierStrings,
    Bool, (TiObject*, Char const*, Char const***, Word*, Core::Notices::Store*, Core::Processing::Parser*)
  );
  public: static Bool _getModifierStrings(
    TiObject *self, TiObject *element, Char const *modifierKwd, Char const **resultStrs[], Word *resultCount,
    Core::Notices::Store *noticeStore, Core::Processing::Parser *parser
  );

  /// @}

}; // class

} // namespace

#endif
