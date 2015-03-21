/* =============================================================================
  (C): Nitish Kanabar - 2012. nitish@indigoriverllc.com
--------------------------------------------------------------------------------
  class FuturesOptionLookup::Request : definitions
============================================================================== */


#include <sstream>
using std::ostringstream;

#include "FuturesOptionLookup.h"
using namespace lumacu;


string
FuturesOptionLookup::Request::commandLine()
{
  ostringstream cmd_stream;
  cmd_stream << "CFO," << symbols.front() << ",";

  cmd_stream << optionType << ",";

  cmd_stream << FuturesOptionLookup::month(expiryMonth) << ",";
	cmd_stream << years << ",";
  cmd_stream << "0,"; // near months
  cmd_stream << lumacu::CR_LF;

  return cmd_stream.str();
}
