/* =============================================================================
  (C): Nitish Kanabar - 2012. nitish@indigoriverllc.com
--------------------------------------------------------------------------------
  class lumacu::Application : definitions
============================================================================== */


#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <stdexcept>
using std::exception;
using std::runtime_error;

#include <string>
using std::string;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include "EquityOptionLookup.h"
#include "FuturesOptionLookup.h"
#include "Level1.h"

#include "Application.h"
using namespace lumacu;

Application&
Application::initialize(int argc, char* argv[])
{
  static Application app;
  try
  {
    app.processCommandLine(argc, argv);
    app.initializeDataFeeds();
  }
  catch (opts::options_description& e)
  {
    cerr << e;
    throw EXIT_FAILURE;
  }
  catch (exception& e)
  {
    cerr << "Error: Application failed to initialize\n";
    cerr << e.what() << endl;
    throw EXIT_FAILURE;
  }
  catch (int status)
  {
    throw status;
  }
  catch (...)
  {
    cerr << "Error: Unknown error during application initialization\n";
    throw EXIT_FAILURE;
  }

  return app;
}

void
Application::initializeDataFeeds()
{
  try
  {
    lookup_feed = auto_ptr<Feed>(new Feed
    (
      options_map["feed_host"].as<string>(),
      options_map["feed_lookup_port"].as<string>())
    );
  }
  catch (exception&)
  {
    throw runtime_error("Failed connecting to Lookup feed. Verify that IQLink is active.");
  }

  try
  {
    quotes_feed = auto_ptr<Feed>(new Feed
    (
      options_map["feed_host"].as<string>(),
      options_map["feed_quotes_port"].as<string>())
    );
  }
  catch (exception&)
  {
    throw runtime_error("Failed connecting to Quotes feed. Verify that IQLink is active.");
  }
}

void
Application::run()
{
  boost::shared_ptr<Level1::Request> level1_request;

  string query_type(options_map["query_type"].as<string>());
  if (query_type == "specific_symbol")
  {
    level1_request = boost::shared_ptr<Level1::Request>
        (new Level1::Request(options_map["symbol"].as<string>()));
  }
  else
  {
    boost::shared_ptr<IRequest> lookup_request;
    if (query_type == "equity_options")
    {
      lookup_request = boost::shared_ptr<IRequest>(new EquityOptionLookup::Request
      (
        options_map["symbol"].as<string>(),
        lumacu::OptionLookup::optionType
        (
          static_cast<bool>(options_map.count("calls")),
          static_cast<bool>(options_map.count("puts")),
          static_cast<bool>(options_map.count("both"))
        ),
        options_map["month"].as<int>(),
        static_cast<bool>(options_map.count("leaps"))
      ));
    }
    else if (query_type == "futures_options")
    {
      lookup_request = boost::shared_ptr<IRequest>(new FuturesOptionLookup::Request
      (
        options_map["symbol"].as<string>(),
        lumacu::OptionLookup::optionType
        (
          static_cast<bool>(options_map.count("calls")),
          static_cast<bool>(options_map.count("puts")),
          static_cast<bool>(options_map.count("both"))
        ),
        options_map["month"].as<int>(),
        options_map["years"].as<string>()
      ));
    }

    lookup_feed->processRequest(lookup_request.get());

    level1_request = boost::shared_ptr<Level1::Request>
      (new Level1::Request(lookup_request->getResults()));
  }

  quotes_feed->processRequest(level1_request.get());
}

void
Application::processCommandLine(int argc, char* argv[])
{
  // register the cmd-line
  opts::options_description feed_options("Feed Configuration");
  feed_options.add_options()
    ("feed_host", opts::value<string>()->default_value("127.0.0.1"),
        "IP address of IQLink host")
    ("feed_lookup_port", opts::value<string>()->default_value("9100"),
        "Options chain lookup port")
    ("feed_quotes_port", opts::value<string>()->default_value("5009"),
        "Level 1 quotes port")
    ;

  /*
  opts::options_description db_options("Database Configuration");
  db_options.add_options()
    ("nodb",    opts::value<bool>()->default_value(true), 
        "Do not store quotes. Ignores the db_* options if specified")
    ("db_host", opts::value<string>()->default_value("-"),
        "IP address of database host")
    ("db_port", opts::value<string>()->default_value("-"))
    ("db_user", opts::value<string>()->default_value("-"))
    ("db_pass", opts::value<string>()->default_value("-"))
    ;
  */

  opts::options_description query_options("Query Options");
  query_options.add_options()
    ("query_type",  opts::value<string>()->default_value("equity_options"),
        "equity_options | futures_options | specific_symbol")
    ("symbol",  opts::value<string>(),  "Equity or Index symbol")
    ("puts",    "Show puts only")
    ("calls",   "Show calls only")
    ("both",    "Show both puts and calls")
    ("month",   opts::value<int>(), "1-12")
    ("years",   opts::value<string>(), 
            "The last digit of the years without delimiters Ex: 2005 - 2014 would be 5678901234")
    ("leaps",   "Show LEAPS")
    ;

  opts::options_description all("Syntax");
  all.add_options()
    ("help,h", "Show this message")
    ;
 
  all.add(feed_options).add(query_options);

  // process the cmd-line
  opts::store(opts::parse_command_line(argc, argv, all), options_map);

  if (options_map.count("help"))
  {
    cout << all;
    throw EXIT_SUCCESS;
  }

  if (!options_map.count("feed_host")
      || !options_map.count("feed_lookup_port")
      || !options_map.count("feed_quotes_port"))
  {
    cerr << "Error: missing feed configuration\n";
    throw feed_options;
  }


  // validate params specific to the query type
  bool missingOptions = false;
  if (options_map["query_type"].as<string>() == "equity_options")
  {
    missingOptions = !
      ( haveValidSymbol()
        &&
        haveValidMonth()
      );
  }
  else if (options_map["query_type"].as<string>() == "futures_options")
  {
    missingOptions = !
      ( haveValidSymbol()
        &&
        haveValidMonth()
        &&
        haveValidYears()
      );
  }
  else if (options_map["query_type"].as<string>() == "specific_symbol")
  {
    missingOptions = !haveValidSymbol();
  }
  else
  {
    cerr << "Error: unknown query type\n";
    throw query_options;
  }
  if (missingOptions) throw query_options;
}
