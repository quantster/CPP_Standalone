/* =============================================================================
  (C): Nitish Kanabar - 2012. nitish@indigoriverllc.com
--------------------------------------------------------------------------------
  class lumacu::Level1 : definitions
============================================================================== */


#include <istream>
using std::istream;

#include <sstream>
using std::ostringstream;

#include <vector>
using std::vector;

#include <boost/tokenizer.hpp>
using boost::tokenizer;
using boost::char_separator;

#include <iostream>

#include "Level1.h"

using namespace lumacu::Level1;

string
Request::commandLine()
{
  ostringstream cmd_stream;
  cmd_stream << "T" << lumacu::CR_LF; // get a time-stamp from the server

  for (list<string>::iterator itr = symbols.begin();
      itr != symbols.end(); ++itr)
  {
    cmd_stream << "w" << *itr << lumacu::CR_LF;
  }

  return cmd_stream.str();
}

void
Request::processResponse(shared_ptr<tcp::socket> socket)
{
  std::cout << "#yyyymmdd,hh:mm:ss,symbol,lasttrade,perchange,totvol,"
    << "open,high,low,close,bid,ask,bid-size,ask-size,lasttrade,"
    << "open-interest\n";

  boost::asio::streambuf response_buffer;
  istream response_stream(&response_buffer);

  string response_line, timestamp;
  int num_consequtive_timestamps(0);
  while (true)
  {
    boost::asio::read_until(*socket, response_buffer, lumacu::LF);
    std::getline(response_stream, response_line);

    // ignore everything other than Time and Update messages
    if (
      response_line[0] != lumacu::TIME_MSG
      &&
      response_line[0] != lumacu::SUMMARY_MSG
      )
    {
      continue;
    }
      
    //break on some number of consecutive TIME messages
    if (response_line[0] == lumacu::TIME_MSG)
    {
      if (++num_consequtive_timestamps == 5) break;
      if (timestamp.empty()) timestamp = response_line;
    }
    else
    {
      num_consequtive_timestamps = 0;
      processResponse(response_line, timestamp);
    }

  }
}

void
Request::processResponse(const string& response, const string& timestamp)
{
  char_separator<char> separators(FIELD_SEPARATOR.c_str(), "",boost::keep_empty_tokens);
  tokenizer<char_separator<char> > tokens(response, separators);

  vector<string> fields;
  fields.clear();

  remove_copy_if(tokens.begin(), tokens.end(),
      std::insert_iterator<vector<string> >(fields, fields.begin()),
      IsServerMessage);

  // construct the desired output from the relevant fields

  // output format:
  // date,time,symbol,ex_yy,ex_mm,last trade,% change,total volume,
  // high,low,bid,ask,last trade,

  
  ostringstream out;
  out << timestamp.substr(2,8) << ",";    // date
  out << timestamp.substr(11,17) << ",";  // time
  // symbol
  out << fields[1] << ",";

  // last trade
  fields[3].empty() ? out << "-," : out << fields[3] << ",";

  // percent change
  fields[5].empty() ? out << "-," : out << fields[5] << ",";

  // total volume
  fields[6].empty() ? out << "-," : out << fields[6] << ",";

  // open
  fields[19].empty() ? out << "-," : out << fields[19] << ",";

  // high
  fields[8].empty() ? out << "-," : out << fields[8] << ",";

  // low
  fields[9].empty() ? out << "-," : out << fields[9] << ",";

  // close
  fields[20].empty() ? out << "-," : out << fields[20] << ",";

  // bid
  fields[10].empty() ? out << "-," : out << fields[10] << ",";

  // ask
  fields[11].empty() ? out << "-," : out << fields[11] << ",";
  
  // bid-size
  fields[12].empty() ? out << "-," : out << fields[12] << ",";

  // ask-size
  fields[13].empty() ? out << "-," : out << fields[13] << ",";

  // last trade time
  fields[17].empty() ? out << "-," : out << fields[17] << ",";

  // open interest
  fields[18].empty() ? out << "-," : out << fields[18] << ",";

  std::cout << out.str() << std::endl;
}
