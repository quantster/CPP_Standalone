/* =============================================================================
  (C): Nitish Kanabar - 2012. nitish@indigoriverllc.com
--------------------------------------------------------------------------------
  namespace EquityOptionLookup:
  Members to process requests for equity option chains

  class EquityOptionLookup::Request
  Derived class specific to making / processing equity option lookup data.

============================================================================== */

#ifndef _EQUITY_OPTION_LOOKUP_H_
#define _EQUITY_OPTION_LOOKUP_H_

#include "OptionLookup.h"
using namespace lumacu;

namespace lumacu
{
  namespace EquityOptionLookup
  {
    const char JAN_FOR_CALLS('A');
    const char JAN_FOR_PUTS('M');

    static string month(int m, OptionLookup::OptionType t)
    {
      ostringstream out;
      switch (t)
      {
        case OptionLookup::BOTH:
          out << char(JAN_FOR_CALLS + m - 1);
          out << char(JAN_FOR_PUTS  + m - 1);
          break;

        case OptionLookup::CALL:
          out << char(JAN_FOR_CALLS + m - 1);
          break;

        case OptionLookup::PUT:
          out << char(JAN_FOR_PUTS + m - 1);
          break;
      }
      return out.str();
    }

    // class to construct a request for an equity/index option chain
    class Request : public OptionLookup::Request
    {
      public:
        Request(const string& s, OptionLookup::OptionType t, int m, bool leaps)
          : OptionLookup::Request(s, t, m), showLeaps(leaps) {}
        virtual ~Request() {}

        virtual string commandLine();

      protected:
        bool showLeaps;
    };
  }
}

#endif
