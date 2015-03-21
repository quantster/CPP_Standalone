/* =============================================================================
  (C): Nitish Kanabar - 2012. nitish@indigoriverllc.com
--------------------------------------------------------------------------------
  class Feed : definitions
============================================================================== */


#include <boost/asio.hpp>

#include <iostream>
using std::ostream;

#include "Feed.h"

using namespace lumacu;

Feed::Feed(const string& feed_host, const string& feed_port)
{
	tcp::resolver resolver(io_service);
	tcp::resolver::query query(feed_host, feed_port);
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

	socket = shared_ptr<tcp::socket>(new tcp::socket(io_service));
	boost::asio::connect(*socket, endpoint_iterator);
}

void
Feed::processRequest(IRequest* request)
{
  boost::asio::streambuf request_buffer;
  ostream request_stream(&request_buffer);
  request_stream << request->commandLine();
  
  // send the request
  boost::asio::write(*socket, request_buffer);

  // have the request object process the response
  request->processResponse(socket);
}

