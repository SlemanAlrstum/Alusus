/**
 * @file Scg/LibraryGateway.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <map>
#include <core.h>
#include <LibraryGateway.h>

namespace Scg
{

using namespace Core;
using namespace Core::Data;

//==============================================================================
// Overloaded Abstract Functions

void LibraryGateway::initialize(Standard::RootManager *manager)
{
    this->moduleHandler = std::make_shared<ModuleParsingHandler>(manager);
    this->buildHandler = std::make_shared<BuildParsingHandler>(manager);
    this->runHandler = std::make_shared<RunParsingHandler>(manager);
    this->dumpHandler = std::make_shared<DumpParsingHandler>(manager);
    this->defHandler = std::make_shared<DefParsingHandler>(manager);
    this->handler = std::make_shared<Core::Standard::GenericParsingHandler>();

    GrammarRepository *grammarRepository = manager->getGrammarRepository();

    // Create leading commands.

    //// run = "run" + Subject
    grammarRepository->setSharedValue(STR("root:Main.Run"), SymbolDefinition::create({
        {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("kwd"), std::make_shared<String>(STR("run"))},
            {STR("prms"), SharedList::create({
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("root:Subject"))},
                    {STR("min"), std::make_shared<Integer>(1)},
                    {STR("max"), std::make_shared<Integer>(1)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    })
                })}
            })},
        {SymbolDefElement::HANDLER, this->runHandler}
        }));

    //// build = "build" + Subject
    grammarRepository->setSharedValue(STR("root:Main.Build"), SymbolDefinition::create({
        {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("kwd"), std::make_shared<String>(STR("build"))},
            {STR("prms"), SharedList::create({
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("root:Subject"))},
                    {STR("min"), std::make_shared<Integer>(1)},
                    {STR("max"), std::make_shared<Integer>(1)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    })
                })}
            })},
        {SymbolDefElement::HANDLER, this->buildHandler}
        }));

    //// dump = "dump" + Subject
    grammarRepository->setSharedValue(STR("root:Main.Dump"), SymbolDefinition::create({
        {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("kwd"), std::make_shared<String>(STR("dump"))},
            {STR("prms"), SharedList::create({
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("root:Subject"))},
                    {STR("min"), std::make_shared<Integer>(1)},
                    {STR("max"), std::make_shared<Integer>(1)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    })
                })}
            })},
        {SymbolDefElement::HANDLER, this->dumpHandler}
        }));

    //// def = "def" + Subject
    grammarRepository->setSharedValue(STR("root:Main.Def"), SymbolDefinition::create({
        {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("kwd"), std::make_shared<String>(STR("def"))},
            {STR("prms"), SharedList::create({
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("root:Expression"))},
                    {STR("min"), std::make_shared<Integer>(1)},
                    {STR("max"), std::make_shared<Integer>(1)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    })
                })}
            })},
        {SymbolDefElement::HANDLER, this->defHandler}
        }));

    //// return = "return" + Subject
    grammarRepository->setSharedValue(STR("root:Main.Return"), SymbolDefinition::create({
        {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("kwd"), std::make_shared<String>(STR("return"))},
            {STR("prms"), SharedList::create({
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("root:Expression"))},
                    {STR("min"), std::make_shared<Integer>(0)},
                    {STR("max"), std::make_shared<Integer>(1)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    })
                })}
            })},
        {SymbolDefElement::HANDLER, this->handler}
        }));

    //// if = "if" + Exp + Statement
    grammarRepository->setSharedValue(STR("root:Main.If"), SymbolDefinition::create({
        {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("kwd"), std::make_shared<String>(STR("if"))},
            {STR("prms"), SharedList::create({
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("root:Expression"))},
                    {STR("min"), std::make_shared<Integer>(1)},
                    {STR("max"), std::make_shared<Integer>(1)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    }),
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("module:Statement"))},
                    {STR("min"), std::make_shared<Integer>(1)},
                    {STR("max"), std::make_shared<Integer>(1)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    })
                })}
            })},
        {SymbolDefElement::HANDLER, this->handler}
        }));

    //// for = "for" + Exp + Statement
    grammarRepository->setSharedValue(STR("root:Main.For"), SymbolDefinition::create({
        {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("kwd"), std::make_shared<String>(STR("for"))},
            {STR("prms"), SharedList::create({
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("root:Expression"))},
                    {STR("min"), std::make_shared<Integer>(1)},
                    {STR("max"), std::make_shared<Integer>(1)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    }),
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("module:Statement"))},
                    {STR("min"), std::make_shared<Integer>(1)},
                    {STR("max"), std::make_shared<Integer>(1)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    })
                })}
            })},
        {SymbolDefElement::HANDLER, this->handler}
        }));

    //// while = "while" + Exp + Statement
    grammarRepository->setSharedValue(STR("root:Main.While"), SymbolDefinition::create({
        {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("kwd"), std::make_shared<String>(STR("while"))},
            {STR("prms"), SharedList::create({
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("root:Expression"))},
                    {STR("min"), std::make_shared<Integer>(1)},
                    {STR("max"), std::make_shared<Integer>(1)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    }),
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("module:Statement"))},
                    {STR("min"), std::make_shared<Integer>(1)},
                    {STR("max"), std::make_shared<Integer>(1)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    })
                })}
            })},
        {SymbolDefElement::HANDLER, this->handler}
        }));

    //// link = "link" + Set
    grammarRepository->setSharedValue(STR("root:Main.Link"), SymbolDefinition::create({
        {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("kwd"), std::make_shared<String>(STR("link"))},
            {STR("prms"), SharedList::create({
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("module:Statement"))},
                    {STR("min"), std::make_shared<Integer>(1)},
                    {STR("max"), std::make_shared<Integer>(1)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    })
                })}
            })},
        {SymbolDefElement::HANDLER, this->handler}
        }));

    //// Add command to leading commands list.
    SharedList *cmd_list = this->GetLeadingCommandsList(grammarRepository);
    this->leadingCmdListPos = static_cast<Int>(cmd_list->getCount());
    cmd_list->add(ReferenceParser::parseQualifier(STR("module:Run")));
    cmd_list->add(ReferenceParser::parseQualifier(STR("module:Build")));
    cmd_list->add(ReferenceParser::parseQualifier(STR("module:Dump")));
    cmd_list->add(ReferenceParser::parseQualifier(STR("module:Def")));
    cmd_list->add(ReferenceParser::parseQualifier(STR("module:Return")));
    cmd_list->add(ReferenceParser::parseQualifier(STR("module:If")));
    cmd_list->add(ReferenceParser::parseQualifier(STR("module:For")));
    cmd_list->add(ReferenceParser::parseQualifier(STR("module:While")));
    cmd_list->add(ReferenceParser::parseQualifier(STR("module:Link")));


    // Create inner command.

    //// module = "module" + Subject + Subject
    grammarRepository->setSharedValue(STR("root:Subject.Module"), SymbolDefinition::create({
        {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("kwd"), std::make_shared<String>(STR("module"))},
            {STR("prms"), SharedList::create({
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("root:SubSet"))},
                    {STR("min"), std::make_shared<Integer>(1)},
                    {STR("max"), std::make_shared<Integer>(1)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    })
                })}
            })},
        {SymbolDefElement::HANDLER, this->moduleHandler}
        }));

    //// function = "function" + Exp + Statement
    // TODO: Exp and Statement need to be optional, but at least one of them is
    // given.
    /*grammarRepository->setSharedValue(STR("root:Subject.Function"), SymbolDefinition::create({
        {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("kwd"), std::make_shared<String>(STR("function"))},
            {STR("prms"), SharedList::create({
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("root:SubSubject"))},
                    {STR("min"), std::make_shared<Integer>(1)},
                    {STR("max"), std::make_shared<Integer>(2)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    })
                })}
            })},
        {SymbolDefElement::HANDLER, this->handler}
        }));*/

    grammarRepository->setSharedValue(STR("root:Subject.Function"), SymbolDefinition::create({
        {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("kwd"), std::make_shared<String>(STR("function"))},
            {STR("prms"), SharedList::create({
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("root:Expression"))},
                    {STR("min"), std::make_shared<Integer>(1)},
                    {STR("max"), std::make_shared<Integer>(1)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    }),
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("root:SubSet"))},
                    {STR("min"), std::make_shared<Integer>(1)},
                    {STR("max"), std::make_shared<Integer>(1)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    })
                })}
            })},
        {SymbolDefElement::HANDLER, this->handler}
        }));

    //// structure = "struct" + Statement
    grammarRepository->setSharedValue(STR("root:Subject.Structure"), SymbolDefinition::create({
        {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("kwd"), std::make_shared<String>(STR("struct"))},
            {STR("prms"), SharedList::create({
                SharedMap::create(false, {
                    {STR("prd"), ReferenceParser::parseQualifier(STR("root:SubSet"))},
                    {STR("min"), std::make_shared<Integer>(1)},
                    {STR("max"), std::make_shared<Integer>(1)},
                    {STR("pty"), std::make_shared<Integer>(1)}
                    })
                })}
            })},
        {SymbolDefElement::HANDLER, this->handler}
        }));

    //// Add command to inner commands list.
    cmd_list = this->GetInnerCommandsList(grammarRepository);
    this->innerCmdListPos = static_cast<Int>(cmd_list->getCount());
    cmd_list->add(ReferenceParser::parseQualifier(STR("module:Module")));
    cmd_list->add(ReferenceParser::parseQualifier(STR("module:Function")));
    cmd_list->add(ReferenceParser::parseQualifier(STR("module:Structure")));

    // Create tilde commands.

    // ~ptr
    grammarRepository->setSharedValue(STR("root:Expression.Pointer_Tilde"), SymbolDefinition::create({
        {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("kwd"), std::make_shared<String>(STR("ptr"))},
            {STR("prms"), SharedList::create({})}
            })},
        {SymbolDefElement::HANDLER, this->handler}
        }));
    // ~cnt
    grammarRepository->setSharedValue(STR("root:Expression.Content_Tilde"), SymbolDefinition::create({
        {SymbolDefElement::TERM, ReferenceParser::parseQualifier(STR("root:Cmd"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("kwd"), std::make_shared<String>(STR("cnt"))},
            {STR("prms"), SharedList::create({})}
            })},
        {SymbolDefElement::HANDLER, this->handler}
        }));
    //// Add command to tilde commands list.
    cmd_list = this->GetTildeCommandsList(grammarRepository);
    this->tildeCmdListPos = static_cast<Int>(cmd_list->getCount());
    cmd_list->add(ReferenceParser::parseQualifier(STR("module:Pointer_Tilde")));
    cmd_list->add(ReferenceParser::parseQualifier(STR("module:Content_Tilde")));


    // Define sub statements.

    // Define a SubMain module.
    grammarRepository->setSharedValue(STR("root:SubMain"), GrammarModule::create({
        {STR("@parent"), ReferenceParser::parseQualifier(STR("root:Main"))}}));

    //// def = "def" + Subject
    grammarRepository->setSharedValue(STR("root:SubMain.Def"), SymbolDefinition::create({
        {SymbolDefElement::PARENT_REF, ReferenceParser::parseQualifier(STR("pmodule:Def"))},
        {SymbolDefElement::HANDLER, this->handler}
        }));

    //// Overloaded subject.
    grammarRepository->setSharedValue(STR("root:SubSubject"), GrammarModule::create({
        {STR("@parent"), ReferenceParser::parseQualifier(STR("root:Subject"))}
        }));
    grammarRepository->setSharedValue(STR("root:SubSubject.Subject1"), SymbolDefinition::create({
        {SymbolDefElement::PARENT_REF, ReferenceParser::parseQualifier(STR("pmodule:Subject1"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("sbj1"), SharedList::create({
                ReferenceParser::parseQualifier(STR("module:SubjectCmdGrp")),
                ReferenceParser::parseQualifier(STR("module:Parameter")),
                ReferenceParser::parseQualifier(STR("root:SubSet"))
                })},
            {STR("sbj2"), SharedList::create({ReferenceParser::parseQualifier(STR("root:Expression"))})},
            {STR("sbj3"), SharedList::create({ReferenceParser::parseQualifier(STR("root:Expression"))})},
            {STR("frc2"), 0},
            {STR("frc3"), 0}
            })}
        }));

    //// Overloaded set.
    grammarRepository->setSharedValue(STR("root:SubSet"), SymbolDefinition::create({
        {SymbolDefElement::PARENT_REF, ReferenceParser::parseQualifier(STR("root:Set"))},
        {SymbolDefElement::VARS, SharedMap::create(false, {
            {STR("stmt"), ReferenceParser::parseQualifier(STR("root:SubMain.StatementList"))}
            })}
        }));
}


void LibraryGateway::uninitialize(Standard::RootManager *manager)
{
    GrammarRepository *grammarRepository = manager->getGrammarRepository();

    // Remove commands from leading commands list.
    SharedList *cmdList = this->GetLeadingCommandsList(grammarRepository);
    for (int i = 0; i < 8; ++i) {
      cmdList->remove(this->leadingCmdListPos);
    }

    // Remove command from inner commands list.
    cmdList = this->GetInnerCommandsList(grammarRepository);
    for (int i = 0; i < 3; ++i) {
        cmdList->remove(this->innerCmdListPos);
    }

    // Remove command from tilde commands list.
    cmdList = this->GetTildeCommandsList(grammarRepository);
    for (int i = 0; i < 2; ++i) {
        cmdList->remove(this->tildeCmdListPos);
    }

    // Delete definitions.
    grammarRepository->removeValue(STR("root:Main.Run"));
    grammarRepository->removeValue(STR("root:Main.Build"));
    grammarRepository->removeValue(STR("root:Main.Dump"));
    grammarRepository->removeValue(STR("root:Main.Def"));
    grammarRepository->removeValue(STR("root:Main.Return"));
    grammarRepository->removeValue(STR("root:Main.If"));
    grammarRepository->removeValue(STR("root:Main.For"));
    grammarRepository->removeValue(STR("root:Main.While"));
    grammarRepository->removeValue(STR("root:Main.Link"));
    grammarRepository->removeValue(STR("root:Subject.Module"));
    grammarRepository->removeValue(STR("root:Subject.Function"));
    grammarRepository->removeValue(STR("root:Subject.Structure"));
    grammarRepository->removeValue(STR("root:Expression.Pointer_Tilde"));
    grammarRepository->removeValue(STR("root:Expression.Content_Tilde"));
    grammarRepository->removeValue(STR("root:SubMain"));
    grammarRepository->removeValue(STR("root:SubSubject"));
    grammarRepository->removeValue(STR("root:SubSet"));
}


SharedList* LibraryGateway::GetLeadingCommandsList(GrammarRepository *grammarRepository)
{
  PlainModulePairedPtr retVal;
  grammarRepository->getPlainValue(STR("root:Main.LeadingCmdGrp"), retVal);
  SymbolDefinition *def = io_cast<SymbolDefinition>(retVal.object);
  if (def == 0) {
      throw GeneralException(STR("Could not find leading command group."),
                              STR("Scg::LibraryGateway::GetLeadingCommandsList"));
  }
  GrammarContext context;
  context.setRoot(grammarRepository->getRoot().get());
  SharedMap *vars = context.getSymbolVars(def, retVal.module);
  SharedList *cmd_list = vars->get(STR("cmds")).io_cast_get<SharedList>();
  if (cmd_list == 0) {
      throw GeneralException(STR("Could not find leading command group's command list."),
                              STR("Scg::LibraryGateway::GetLeadingCommandsList"));
  }
  return cmd_list;
}


SharedList* LibraryGateway::GetInnerCommandsList(GrammarRepository *grammarRepository)
{
  PlainModulePairedPtr retVal;
  grammarRepository->getPlainValue(STR("root:Subject.SubjectCmdGrp"), retVal);
  SymbolDefinition *def = io_cast<SymbolDefinition>(retVal.object);
  if (def == 0) {
      throw GeneralException(STR("Could not find inner command group."),
                              STR("Scg::LibraryGateway::GetInnerCommandsList"));
  }
  GrammarContext context;
  context.setRoot(grammarRepository->getRoot().get());
  SharedMap *vars = context.getSymbolVars(def, retVal.module);
  SharedList *cmd_list = vars->get(STR("cmds")).io_cast_get<SharedList>();
  if (cmd_list == 0) {
      throw GeneralException(STR("Could not find inner command group's command list."),
                              STR("Scg::LibraryGateway::GetInnerCommandsList"));
  }
  return cmd_list;
}


SharedList* LibraryGateway::GetTildeCommandsList(GrammarRepository *grammarRepository)
{
  PlainModulePairedPtr retVal;
  grammarRepository->getPlainValue(STR("root:Expression.DefaultPostfixTildeCmd"), retVal);
  SymbolDefinition *def = io_cast<SymbolDefinition>(retVal.object);
  if (def == 0) {
      throw GeneralException(STR("Could not find tilde command group."),
                              STR("Scg::LibraryGateway::GetTildeCommandsList"));
  }
  GrammarContext context;
  context.setRoot(grammarRepository->getRoot().get());
  SharedMap *vars = context.getSymbolVars(def, retVal.module);
  SharedList *cmd_list = vars->get(STR("cmds")).io_cast_get<SharedList>();
  if (cmd_list == 0) {
      throw GeneralException(STR("Could not find inner command group's command list."),
                              STR("Scg::LibraryGateway::GetTildeCommandsList"));
  }
  return cmd_list;
}

} // namespace

