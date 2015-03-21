/* =============================================================================
  (C): Nitish Kanabar - 2012. nitish@indigoriverllc.com
--------------------------------------------------------------------------------
  namespace FuturesOptionLookup:
  Members specific to processing futures options chains

  class FuturesOptionLookup::Request
  Derived class to process / make requests for futures options chains
============================================================================== */

#ifndef _FUTURES_OPTION_LOOKUP_H_
#define _FUTURES_OPTION_LOOKUP_H_

#include "OptionLookup.h"
using namespace lumacu;

namespace lumacu
{
  namespace FuturesOptionLookup
  {
    static string month(int m)
    {
      switch (m)
      {
        case 1:
          return "F";
          break;
        case 2:
          return "G";
          break;
        case 3:
          return "H";
          break;
         case 4:
          return "J";
          break;
        case 5:
          return "K";
          break;
        case 6:
          return "M";
          break;
        case 7:
          return "N";
          break;
        case 8:
          return "Q";
          break;
        case 9:
          return "U";
          break;
        case 10:
          return "V";
          break;
        case 11:
          return "X";
          break;
        case 12:
          return "Z";
          break;
      }
    }

    // class to construct a request for an equity/index option chain
    class Request : public lumacu::OptionLookup::Request
    {
      public:
        Request(const string& s, OptionLookup::OptionType t, int m, const string& y)
          : OptionLookup::Request(s, t, m), years(y) {}
        virtual ~Request() {}

        virtual string commandLine();

      protected:
        string years;
    };
  }
}

#endif
