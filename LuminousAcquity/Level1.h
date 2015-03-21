/* =============================================================================
  (C): Nitish Kanabar - 2012. nitish@indigoriverllc.com
--------------------------------------------------------------------------------
  namespace Level1:
  Members are specialized for requesting and handling level1 quotes data

  class Level1::Request
  Derived class to make & process requests for Level1 quotes

============================================================================== */

#ifndef _LEVEL1_H_
#define _LEVEL1_H_

#include <boost/shared_ptr.hpp>

#include "IRequest.h"

namespace lumacu
{
  namespace Level1
  {
    // class to construct a request for an equity/index option chain
    class Request : public lumacu::IRequest
    {
      public:
        Request(const list<string>& s) : IRequest(s) {}
        Request(const string& s) : IRequest(s) {}

        virtual ~Request() {}

        virtual string commandLine();
        virtual void processResponse(boost::shared_ptr<tcp::socket>);
        virtual void processResponse(const string&, const string&);
    };
  }
}

#endif
