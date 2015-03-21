/* =============================================================================
  (C): Nitish Kanabar - 2012. nitish@indigoriverllc.com
--------------------------------------------------------------------------------
  class EquityOptionLookup::Reqest : definitions
============================================================================== */

#include <sstream>
using std::ostringstream;

#include "EquityOptionLookup.h"
using namespace lumacu;


string
EquityOptionLookup::Request::commandLine()
{
  ostringstream cmd_stream;
  cmd_stream << "CEO," << symbols.front() << ",";

  cmd_stream << optionType << ",";

  cmd_stream << EquityOptionLookup::month(expiryMonth, optionType) << ",";
  //cmd_stream << "0,"; // near months
  showLeaps ? cmd_stream << "T," : cmd_stream << "F,";
  cmd_stream << "0,0,0," << lumacu::CR_LF; // other filters

  return cmd_stream.str();
}

