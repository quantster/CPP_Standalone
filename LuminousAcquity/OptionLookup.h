/* =============================================================================
  (C): Nitish Kanabar - 2012. nitish@indigoriverllc.com
--------------------------------------------------------------------------------
  namespace OptionLookup
  Declares members specific to processing requests for option chains

  class OptionLookup::Request
  Abstract Base class for Option lookup requests - had additional data members
  that are specific for requesting option chains.
============================================================================== */

#ifndef _OPTION_LOOKUP_H_
#define _OPTION_LOOKUP_H_

#include <sstream>
using std::ostringstream;

#include <ostream>
using std::ostream;

#include "IRequest.h"

namespace lumacu
{
  namespace OptionLookup
  {
    enum OptionType {CALL, PUT, BOTH};

    inline OptionType optionType(bool call, bool put, bool both)
    {
      if (both) return BOTH; // ignore the rest
      if (call && put) return BOTH;
      return call ? CALL : PUT;
    }

    inline ostream& operator<<(ostream& os, OptionType t)
    {
      switch (t)
      {
        case OptionLookup::BOTH:
          os << "pc";
          break;
        case OptionLookup::CALL:
          os << "c";
          break;
        case OptionLookup::PUT:
          os << "p";
          break;
      }

      return os;
    }


    class Request : public lumacu::IRequest
    {
      public:
        Request(const string& s, OptionType t, int m)
          : IRequest(s), optionType(t), expiryMonth(m) {}
        virtual ~Request() {}

        virtual string commandLine() = 0;

      protected:
        OptionType optionType;
        int expiryMonth;
    };
  }
}

#endif
