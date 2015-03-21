/* =============================================================================
  (C): Nitish Kanabar - 2012. nitish@indigoriverllc.com
--------------------------------------------------------------------------------
  namespace lumacu:
  Constants to help process responses from the IQFeed server

  class IRequest:
  Abstract Base class for all requests to the IQFeed server.
  This class defines the interface that the specific request/feed handlers
  should implement. 
  The derived class should define the commandLine() member function to create
  the commandline that is sent to IQFeed for that specific request.
  The derived class MAY be required to override the processResponse() member
  function if the format of the reponse is specific for that request type.
============================================================================== */

#ifndef _I_REQUEST_H_
#define _I_REQUEST_H_

#include <string>
using std::string;

#include <list>
using std::list;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <boost/asio.hpp>
using boost::asio::ip::tcp;


namespace lumacu
{
  // standard reponses returned by the server
  const string CR("\r");
  const string LF("\n");
  const string CR_LF("\r\n");
  const string FIELD_SEPARATOR(",\r\n");
  const string ENDMSG("!ENDMSG!");
  const string NODATA("!NO_DATA!");
  const string SYNTAX_ERROR("!SYNTAX_ERROR!");
  const string DATA_SEPARATOR(":");

  // message types
  const unsigned char TIME_MSG('T');
  const unsigned char ERROR_MSG('E');
  const unsigned char SYSTEM_MSG('S');
  const unsigned char FUNDAMENTAL_MSG('F');
  const unsigned char UPDATE_MSG('Q');
  const unsigned char SUMMARY_MSG('P');
  const unsigned char REGIONAL_MSG('R');
  const unsigned char NEWS_MSG('N');

  // filter to remove server messages from the response
  inline bool IsServerMessage(const string& me)
  {
    return (me == ENDMSG || me == CR || me == LF || me == CR_LF || me == " ");
  }

  // abstract base class for all request/feed handlers
	class IRequest
  {
    public:
      IRequest(const list<string>& s) : symbols(s) {}
      IRequest(const string&s) { symbols.push_front(s); }
      virtual ~IRequest() {}

      virtual string commandLine() = 0;

      virtual void processResponse(shared_ptr<tcp::socket>);

      inline const list<string>& getResults() const
      {
        return results;
      }

    protected:
      virtual void processResponse(const string&);

      list<string> symbols;
      list<string> results;
  };
}

#endif
