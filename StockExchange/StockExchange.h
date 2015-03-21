// ==============================================================================
// Author: Nitish Kanabar nkanabar@gmail.com
// ------------------------------------------------------------------------------
// This class implements the IStockExhange interface
// ==============================================================================

#ifndef _StockExchange_H_
#define _StockExchange_H_

#include <string>
#include <set>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "IStockExchange.h"
#include "Order.h"
#include "BestPriceFeed.h"

using std::string;
using std::set;
using boost::scoped_ptr;
using boost::shared_ptr;

namespace Tibra
{
namespace Exchange
{
  class StockExchange : public IStockExchange
  {
    public:
      StockExchange();

      virtual ~StockExchange() {}

      virtual void AddOrder(
          const string& stockCode,
          bool bidSide,
          int price,
          int volume,
          int clientOrderId);

      virtual void RemoveOrder(
          int exchangeOrderId);

    private:
      int nextExchangeOrderId;

      typedef map<int, tOrderPtr> tOrders;
      scoped_ptr<tOrders> orders;

      scoped_ptr<BestPriceFeed> feed;

      static set<string> supportedStockCodes;
      static void initialize();

      // helpers
      inline void callOnOrderAdded(int exchangeOrderId, int clientOrderId, int errorCode)
      {
        if (!OnOrderAdded.empty())
          OnOrderAdded(exchangeOrderId, clientOrderId, errorCode);
      }

      inline void callOnOrderRemoved(int exchangeOrderId, int errorCode)
      {
        if (!OnOrderRemoved.empty())
          OnOrderRemoved(exchangeOrderId, errorCode);
      }
  };

}
}

#endif
