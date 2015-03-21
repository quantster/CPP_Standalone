/* =============================================================================
  (C): Nitish Kanabar - 2012. nitish@indigoriverllc.com
--------------------------------------------------------------------------------
  IRequest: definitions
============================================================================== */


#include <istream>
using std::istream;

#include <stdexcept>
using std::runtime_error;

#include <sstream>
using std::ostringstream;

#include <algorithm>
using std::remove_copy_if;

#include <boost/tokenizer.hpp>
using boost::tokenizer;
using boost::char_separator;

#include "IRequest.h"

using namespace lumacu;

void
IRequest::processResponse(const string& response)
{
  char_separator<char> separators(FIELD_SEPARATOR.c_str());
  tokenizer<char_separator<char> > tokens(response, separators);

  results.clear();
  remove_copy_if(tokens.begin(), tokens.end(),
        std::insert_iterator<list<string> >(results, results.begin()),
        IsServerMessage);

  if (results.empty())
  {
    throw runtime_error("Empty response from feed server");
  }

  if (response[0] == ERROR_MSG)
  {
    results.pop_front();
    if (results.front() == NODATA)
    {
      throw runtime_error("No data for current request");
    }
    else if (results.front() == SYNTAX_ERROR)
    {
      throw runtime_error("Syntax error in server request");
    }
  }
}

void
IRequest::processResponse(boost::shared_ptr<tcp::socket> socket)
{
  boost::asio::streambuf response_buffer;
  istream response_stream(&response_buffer);

  boost::asio::read_until(*socket, response_buffer, CR_LF);

  string response_line;
  std::getline(response_stream, response_line);

  processResponse(response_line);
  results.remove(DATA_SEPARATOR);
}
