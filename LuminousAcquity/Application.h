/* =============================================================================
  (C): Nitish Kanabar - 2012. nitish@indigoriverllc.com
--------------------------------------------------------------------------------
  class lumacu::Application
  This is where it all comes together.
  The application processes the command-line to obtain configuration info to
  connect to the IQFeed server and creates two Feed objects - one for the 
  options chain lookup and the other for the Level-1 quotes.
  Each request type has its own required set of parameters. Once the command-line
  is parsed and validated, the Application connects to the IQFeed server via
  the Feed objects and processes the specific request.
  The application is designed to be used in a snap-shot query mode - i.e. it
  connects to IQFeed, submits the requests, processes the response, and then
  exits. Another possible mode (not currently implemented) is a feed-handler mode,
  where the application maintains a connection to the Feeds and processes 
  asynchronous events (such as quote updates, trades, etc) as they arrive from
  the server.
============================================================================== */

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <iostream>
using std::cerr;

#include <memory>
using std::auto_ptr;

#include <boost/program_options.hpp>
namespace opts = boost::program_options;

#include "Feed.h"

namespace lumacu
{
  class Application
  {
    public:
      static Application& initialize(int, char**);
      virtual ~Application() {}

      void run();

    private:
      Application() {}

      void processCommandLine(int, char**);
      void initializeDataFeeds();

      // helpers
      inline bool haveValidSymbol()
      {
        if (options_map.count("symbol") == 0)
        {
          cerr << "Error: missing symbol\n";
          return false;
        }
        return true;
      }

      inline bool haveValidMonth()
      {
        if (options_map.count("month") == 0)
        {
          cerr << "Error: missing month\n";
          return false;
        }
        if (options_map["month"].as<int>() < 1 
            ||
            options_map["month"].as<int>() > 12
           )
        {
          cerr << "Error: invalid month\n";
          return false;
        }
        return true;
      }

      inline bool haveValidYears()
      {
        if (options_map.count("years") == 0)
        {
          cerr << "Error: missing years\n";
          return false;
        }
        return true;
      }

      // data members
      auto_ptr<Feed> lookup_feed;
      auto_ptr<Feed> quotes_feed;

      opts::variables_map options_map;
  };
}

#endif
