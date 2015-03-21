/* =============================================================================
  (C): Nitish Kanabar - 2012. nitish@indigoriverllc.com
--------------------------------------------------------------------------------
  class Feed :
  This class abstracts the various "Feeds" (services) offered by the IQFeed
  server. The IQFeed server provides different types of data on different ports.
  A Feed object abstracts the connection to a specific IQFeed port. It establishes
  a socket connection to the specified IQFeed server port and maintains this
  connection. It uses a IRequest object to obtain the commandline to pass to 
  the IQFeed port and it passes the response of the server back to the IRequest
  object. As such it is a conduit between the IRequest object and the IQFeed 
  port that is capable for processing that request.
============================================================================== */


#ifndef _FEED_H_
#define _FEED_H_

#include <string>
using std::string;

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

#include "IRequest.h"

namespace lumacu
{
	class Feed
  {
    public:
      Feed();
      Feed(const string&, const string&);
      virtual ~Feed() {}

      virtual void processRequest(IRequest*);

    private:
      boost::asio::io_service io_service;
      boost::shared_ptr<tcp::socket> socket;

      Feed(const Feed&);
      Feed& operator=(const Feed&);
  };
}

#endif
