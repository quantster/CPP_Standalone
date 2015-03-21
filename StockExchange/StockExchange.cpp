// ==============================================================================
// Author: Nitish Kanabar nkanabar@gmail.com
// ==============================================================================

#include <utility>
#include "StockExchange.h"
#include "BestPriceFeed.h"

using std::make_pair;
using namespace Tibra::Exchange;

set<string> StockExchange::supportedStockCodes;

// -----------------------------------------------------------------------------
// initialize the exchange with the supported stockCodes
void StockExchange::initialize()
{
  supportedStockCodes.insert("BHP");
  supportedStockCodes.insert("RIO");
  supportedStockCodes.insert("ANZ");
}

// -----------------------------------------------------------------------------
// c'tor
StockExchange::StockExchange()
  : nextExchangeOrderId(0), orders(new tOrders)
{
  if (supportedStockCodes.empty()) initialize();
  feed.reset(new BestPriceFeed(*this));
}


// -----------------------------------------------------------------------------
// add a new order to the exchange after validating inputs
void StockExchange::AddOrder(
    const string& stockCode,
    bool bidSide,
    int price,
    int volume,
    int clientOrderId)
{
  // validate the inputs - stockCode, price, volume
  if (supportedStockCodes.find(stockCode) == supportedStockCodes.end()) {
    callOnOrderAdded(0, clientOrderId, eUNKNOWN_STOCK_CODE);
    return;
  }

  if (price < 0) {
    callOnOrderAdded(0, clientOrderId, eINVALID_ORDER_PRICE);
    return;
  }

  if (volume < 0) {
    if (!OnOrderAdded.empty()) OnOrderAdded(0, clientOrderId, eINVALID_ORDER_VOLUME);
    return;
  }

  // internal system error
  if (++nextExchangeOrderId < 0) {
    callOnOrderAdded(0, clientOrderId, eINTERNAL_SYSTEM_FAILURE);
    return;
  }

  // create the order and add it to the order book
  tOrderPtr o(new Order(
          stockCode, bidSide, price, volume, clientOrderId, nextExchangeOrderId));

  orders->insert(make_pair<int, tOrderPtr>(o->exchangeOrderId, o));

  callOnOrderAdded(o->exchangeOrderId, clientOrderId, eNO_ERROR);

  feed->AddOrder(o);
}

// -----------------------------------------------------------------------------
// remove the specified order from the exchange
void StockExchange::RemoveOrder(int exchangeOrderId)
{
  tOrders::iterator itr;
  if (exchangeOrderId <= 0 || (itr = orders->find(exchangeOrderId)) == orders->end()) {
    callOnOrderRemoved(exchangeOrderId, eNONEXISTENT_ORDER);
    return;
  }

  tOrderPtr order = (*itr).second;
  orders->erase(itr);

  try {
    feed->RemoveOrder(order);
  }
  catch (tCallBackErrorCodes errorCode) {
    callOnOrderRemoved(exchangeOrderId, errorCode);
    return;
  }

  callOnOrderRemoved(exchangeOrderId, eNO_ERROR);
}
