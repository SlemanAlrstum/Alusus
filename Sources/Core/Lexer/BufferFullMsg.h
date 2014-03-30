/**
 * @file Core/Lexer/BufferFullMsg.h
 * Contains the header of class Lexer::BufferFullMsg.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_BUFFER_FULL_MSG_H
#define LEXER_BUFFER_FULL_MSG_H

namespace Core { namespace Lexer
{

/**
 * @brief A build message for the "Buffer Full" error.
 * @ingroup lexer
 *
 * This message class is for error code L1002, which is raised when the
 * lexer's internal buffer gets full. This may cause a token to be broken into
 * two.
 */
class BufferFullMsg : public Common::BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(BufferFullMsg, Common::BuildMsg, "Core.Lexer", "Core", "alusus.net");


  //============================================================================
  // Constructor / Destructor

  public: BufferFullMsg(Int l, Int c) : Common::BuildMsg(l, c)
  {
  }

  public: virtual ~BufferFullMsg()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Common::BuildMsg::getCode()
  public: virtual const Str& getCode() const
  {
    static Str code("L1002");
    return code;
  }

  /// @sa Common::BuildMsg::getSeverity()
  public: virtual Int getSeverity() const
  {
    return 1;
  }

  /// @sa Common::BuildMsg::getCode()
  public: virtual void buildDescription(Str &str) const
  {
    str = STR("Lexer Error: Input buffer is full. A single token is too long to fit in the input buffer. "
              "The token may have been broken into more than one token.");
  }

}; // class

} } // namespace

#endif
