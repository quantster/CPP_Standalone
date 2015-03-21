// ==============================================================================
// Author: Nitish Kanabar nkanabar@gmail.com
// ==============================================================================

#define BOOST_TEST_MODULE TibraExchange
#include <boost/test/included/unit_test.hpp>

#include "StockExchange.h"

using namespace Tibra::Exchange;

struct ClientFixture
{
  static string StockCode;
  static int ExchangeOrderId;
  static int ClientOrderId;
  static int ErrorCode;
  static int BidPrice;
  static int BidVolume;
  static int AskPrice;
  static int AskVolume;

  static void OnOrderAdded(int exchangeOrderId, int clientOrderId, int errorCode)
  {
    ClientFixture::ExchangeOrderId = exchangeOrderId;
    ClientFixture::ClientOrderId = clientOrderId;
    ClientFixture::ErrorCode = errorCode;
  }

  static void OnOrderRemoved(int exchangeOrderId, int errorCode)
  {
    ClientFixture::ExchangeOrderId = exchangeOrderId;
    ClientFixture::ErrorCode = errorCode;
  }

  static void OnBestPriceFeed(const string& stockCode, int bidPrice, int bidVolume,
      int askPrice, int askVolume)
  {
    ClientFixture::StockCode = stockCode;
    ClientFixture::BidPrice = bidPrice;
    ClientFixture::BidVolume = bidVolume;
    ClientFixture::AskPrice = askPrice;
    ClientFixture::AskVolume = askVolume;
  }

  ClientFixture() : clientOrderId(1)
  {
    exchange.OnOrderAdded = ClientFixture::OnOrderAdded;
    exchange.OnOrderRemoved = ClientFixture::OnOrderRemoved;
    exchange.OnBestPriceFeed = ClientFixture::OnBestPriceFeed;
  }

  ~ClientFixture() {}

  StockExchange exchange;

  int clientOrderId;
  enum tOrderSide { ASK = 0, BID = 1 };
};

string ClientFixture::StockCode("");
int ClientFixture::ExchangeOrderId = 0;
int ClientFixture::ClientOrderId = 0;
int ClientFixture::ErrorCode = IStockExchange::eNO_ERROR;
int ClientFixture::BidPrice = 0;
int ClientFixture::BidVolume = 0;
int ClientFixture::AskPrice = 0;
int ClientFixture::AskVolume = 0;

//------------------------------------------------------------------------------
// test AddOrder functionality
BOOST_FIXTURE_TEST_SUITE(AddOrder, ClientFixture)

BOOST_AUTO_TEST_CASE(UnknownStockCode)
{
  exchange.AddOrder("UNK", BID, 0, 0, 0);
  BOOST_CHECK(ErrorCode == IStockExchange::eUNKNOWN_STOCK_CODE);
}

BOOST_AUTO_TEST_CASE(InvalidOrderPrice)
{
  exchange.AddOrder("RIO", BID, -100, 1000, clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eINVALID_ORDER_PRICE);
}

BOOST_AUTO_TEST_CASE(InvalidOrderVolume)
{
  exchange.AddOrder("RIO", BID, 100, -1000, clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eINVALID_ORDER_VOLUME);
}

BOOST_AUTO_TEST_CASE(NoErrorSingleBidOrder)
{
  exchange.AddOrder("RIO", BID, 100, 1000, clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);
}

BOOST_AUTO_TEST_CASE(NoErrorSingleAskOrder)
{
  exchange.AddOrder("ANZ", ASK, 100, 1000, clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);
}

BOOST_AUTO_TEST_CASE(NoErrorManyBidOrders)
{
  exchange.AddOrder("ANZ", BID, 100, 1000, clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);

  exchange.AddOrder("RIO", BID, 102, 2000, ++clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);

  exchange.AddOrder("ANZ", BID, 100, 1000, ++clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);
}

BOOST_AUTO_TEST_CASE(NoErrorManyAskOrders)
{
  exchange.AddOrder("ANZ", ASK, 100, 1000, ++clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);

  exchange.AddOrder("RIO", ASK, 102, 2000, ++clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);

  exchange.AddOrder("ANZ", ASK, 100, 1000, ++clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);
}

BOOST_AUTO_TEST_CASE(NoErrorMixedOrders)
{
  exchange.AddOrder("ANZ", ASK, 100, 1000, ++clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);

  exchange.AddOrder("RIO", BID, 102, 2000, ++clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);

  exchange.AddOrder("ANZ", ASK, 100, 1000, ++clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);

  exchange.AddOrder("ANZ", BID, 100, 1000, ++clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);

  exchange.AddOrder("RIO", ASK, 102, 2000, ++clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);

  exchange.AddOrder("ANZ", ASK, 100, 1000, ++clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);
}

BOOST_AUTO_TEST_CASE(ExchangeOrderIdIncrement)
{
  exchange.AddOrder("ANZ", ASK, 100, 1000, ++clientOrderId);
  int exchangeOrderId = ExchangeOrderId;

  exchange.AddOrder("RIO", BID, 102, 2000, ++clientOrderId);
  BOOST_CHECK(exchangeOrderId < ExchangeOrderId);
}

BOOST_AUTO_TEST_SUITE_END()

//------------------------------------------------------------------------------
// test RemoveOrder functionality
BOOST_FIXTURE_TEST_SUITE(RemoveOrder, ClientFixture)

BOOST_AUTO_TEST_CASE(InvalidExchangeOrderId)
{
  exchange.AddOrder("ANZ", BID, 100, 1000, ++clientOrderId);
  exchange.AddOrder("RIO", ASK, 100, 1000, ++clientOrderId);
  exchange.AddOrder("BHP", BID, 100, 1000, ++clientOrderId);
  exchange.AddOrder("ANZ", BID, 100, 1000, ++clientOrderId);

  exchange.RemoveOrder(ExchangeOrderId + 100);
  BOOST_CHECK(ErrorCode == IStockExchange::eNONEXISTENT_ORDER);
}

BOOST_AUTO_TEST_CASE(RemoveMultipleTimes)
{
  exchange.AddOrder("ANZ", BID, 100, 1000, ++clientOrderId);
  exchange.AddOrder("RIO", ASK, 100, 1000, ++clientOrderId);
  exchange.AddOrder("BHP", BID, 100, 1000, ++clientOrderId);
  exchange.AddOrder("ANZ", BID, 100, 1000, ++clientOrderId);

  exchange.RemoveOrder(ExchangeOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);

  exchange.RemoveOrder(ExchangeOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNONEXISTENT_ORDER);
}

BOOST_AUTO_TEST_CASE(IntegrityOfExchangeOrderId)
{
  exchange.AddOrder("ANZ", ASK, 100, 1000, 101);
  int exchangeOrderId = ExchangeOrderId;

  exchange.RemoveOrder(exchangeOrderId);

  exchange.AddOrder("RIO", BID, 102, 2000, 102);
  BOOST_CHECK(exchangeOrderId < ExchangeOrderId);
}

BOOST_AUTO_TEST_CASE(NoError)
{
  exchange.AddOrder("ANZ", BID, 100, 1000, ++clientOrderId);
  exchange.AddOrder("RIO", ASK, 100, 1000, ++clientOrderId);
  exchange.AddOrder("BHP", BID, 100, 1000, ++clientOrderId);
  exchange.AddOrder("ANZ", BID, 100, 1000, ++clientOrderId);

  exchange.RemoveOrder(ExchangeOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);
}

BOOST_AUTO_TEST_SUITE_END()

//------------------------------------------------------------------------------
// test BestFeed functionality
BOOST_FIXTURE_TEST_SUITE(BestPriceFeed, ClientFixture)

BOOST_AUTO_TEST_CASE(AddSingleBid)
{
  exchange.AddOrder("ANZ", BID, 100, 1000, ++clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);

  BOOST_CHECK(StockCode == "ANZ");
  BOOST_CHECK(BidPrice == 100);
  BOOST_CHECK(BidVolume == 1000);
  BOOST_CHECK(AskPrice == 0);
  BOOST_CHECK(AskVolume == 0);
}

BOOST_AUTO_TEST_CASE(AddSingleAsk)
{
  exchange.AddOrder("ANZ", ASK, 100, 1000, ++clientOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);

  BOOST_CHECK(StockCode == "ANZ");
  BOOST_CHECK(BidPrice == 0);
  BOOST_CHECK(BidVolume == 0);
  BOOST_CHECK(AskPrice == 100);
  BOOST_CHECK(AskVolume == 1000);
}

BOOST_AUTO_TEST_CASE(AddOrdersCheckStockCode)
{
  exchange.AddOrder("ANZ", ASK, 100, 1000, ++clientOrderId);
  BOOST_CHECK(StockCode == "ANZ");

  exchange.AddOrder("RIO", ASK, 100, 1000, ++clientOrderId);
  BOOST_CHECK(StockCode == "RIO");

  exchange.AddOrder("RIO", BID, 100, 1000, ++clientOrderId);
  BOOST_CHECK(StockCode == "RIO");

  exchange.AddOrder("BHP", BID, 100, 1000, ++clientOrderId);
  BOOST_CHECK(StockCode == "BHP");
}

BOOST_AUTO_TEST_CASE(AddBidsWithChangingBestPrice)
{
  exchange.AddOrder("ANZ", BID, 100, 1000, ++clientOrderId);
  BOOST_CHECK(BidPrice == 100);
  BOOST_CHECK(BidVolume == 1000);

  exchange.AddOrder("ANZ", BID, 200, 2000, ++clientOrderId);
  BOOST_CHECK(BidPrice == 200);
  BOOST_CHECK(BidVolume == 2000);

  exchange.AddOrder("ANZ", BID, 300, 3000, ++clientOrderId);
  BOOST_CHECK(BidPrice == 300);
  BOOST_CHECK(BidVolume == 3000);
}

BOOST_AUTO_TEST_CASE(AddAsksWithChangingBestPrice)
{
  exchange.AddOrder("RIO", ASK, 300, 3000, ++clientOrderId);
  BOOST_CHECK(AskPrice == 300);
  BOOST_CHECK(AskVolume == 3000);

  exchange.AddOrder("RIO", ASK, 200, 2000, ++clientOrderId);
  BOOST_CHECK(AskPrice == 200);
  BOOST_CHECK(AskVolume == 2000);

  exchange.AddOrder("RIO", ASK, 100, 1000, ++clientOrderId);
  BOOST_CHECK(AskPrice == 100);
  BOOST_CHECK(AskVolume == 1000);
}

BOOST_AUTO_TEST_CASE(AddBidsCheckVolume)
{
  exchange.AddOrder("ANZ", BID, 100, 1000, ++clientOrderId);
  BOOST_CHECK(BidPrice == 100);
  BOOST_CHECK(BidVolume == 1000);

  exchange.AddOrder("ANZ", BID, 100, 2000, ++clientOrderId);

  BOOST_CHECK(BidPrice == 100);
  BOOST_CHECK(BidVolume == 3000);
  BOOST_CHECK(AskPrice == 0);
  BOOST_CHECK(AskVolume == 0);
}

BOOST_AUTO_TEST_CASE(AddAsksCheckVolume)
{
  exchange.AddOrder("ANZ", ASK, 100, 1000, ++clientOrderId);
  BOOST_CHECK(AskPrice == 100);
  BOOST_CHECK(AskVolume == 1000);

  exchange.AddOrder("ANZ", ASK, 100, 2000, ++clientOrderId);

  BOOST_CHECK(BidPrice == 0);
  BOOST_CHECK(BidVolume == 0);
  BOOST_CHECK(AskPrice == 100);
  BOOST_CHECK(AskVolume == 3000);
}

BOOST_AUTO_TEST_CASE(RemoveBidsCheckVolume)
{
  exchange.AddOrder("ANZ", BID, 100, 1000, ++clientOrderId);
  exchange.AddOrder("ANZ", BID, 100, 2000, ++clientOrderId);
  BOOST_CHECK(BidPrice == 100);
  BOOST_CHECK(BidVolume == 3000);
  exchange.RemoveOrder(ExchangeOrderId);

  BOOST_CHECK(BidPrice == 100);
  BOOST_CHECK(BidVolume == 1000);
  BOOST_CHECK(AskPrice == 0);
  BOOST_CHECK(AskVolume == 0);
}

BOOST_AUTO_TEST_CASE(RemoveAsksCheckVolume)
{
  exchange.AddOrder("RIO", ASK, 100, 1000, ++clientOrderId);
  exchange.AddOrder("RIO", ASK, 100, 2000, ++clientOrderId);
  BOOST_CHECK(AskPrice == 100);
  BOOST_CHECK(AskVolume == 3000);
  exchange.RemoveOrder(ExchangeOrderId);

  BOOST_CHECK(BidPrice == 0);
  BOOST_CHECK(BidVolume == 0);
  BOOST_CHECK(AskPrice == 100);
  BOOST_CHECK(AskVolume == 1000);
}

BOOST_AUTO_TEST_CASE(AddMixedOrders)
{
  exchange.AddOrder("ANZ", BID, 100, 1000, ++clientOrderId);
  BOOST_CHECK(BidPrice == 100);
  BOOST_CHECK(BidVolume == 1000);
  BOOST_CHECK(AskPrice == 0);
  BOOST_CHECK(AskVolume == 0);

  exchange.AddOrder("ANZ", ASK, 200, 2000, ++clientOrderId);

  BOOST_CHECK(BidPrice == 100);
  BOOST_CHECK(BidVolume == 1000);
  BOOST_CHECK(AskPrice == 200);
  BOOST_CHECK(AskVolume == 2000);
}

BOOST_AUTO_TEST_CASE(RemoveLastOrder)
{
  exchange.AddOrder("ANZ", BID, 100, 1000, ++clientOrderId);
  exchange.RemoveOrder(ExchangeOrderId);
  BOOST_CHECK(ErrorCode == IStockExchange::eNO_ERROR);

  BOOST_CHECK(StockCode == "ANZ");
  BOOST_CHECK(BidPrice == 0);
  BOOST_CHECK(BidVolume == 0);
  BOOST_CHECK(AskPrice == 0);
  BOOST_CHECK(AskVolume == 0);
}

BOOST_AUTO_TEST_CASE(AddNonBestPriceOrder)
{
  exchange.AddOrder("ANZ", ASK, 100, 1000, ++clientOrderId);
  BOOST_CHECK(AskPrice == 100);
  BOOST_CHECK(AskVolume == 1000);

  exchange.AddOrder("ANZ", ASK, 120, 2000, ++clientOrderId);
  BOOST_CHECK(AskPrice == 100);
  BOOST_CHECK(AskVolume == 1000);
}

BOOST_AUTO_TEST_CASE(RemoveNonBestPriceOrder)
{
  exchange.AddOrder("ANZ", BID, 120, 2000, ++clientOrderId);
  BOOST_CHECK(BidPrice == 120);
  BOOST_CHECK(BidVolume == 2000);

  exchange.AddOrder("ANZ", BID, 100, 1000, ++clientOrderId);
  int exchangeOrderId = ExchangeOrderId;
  BOOST_CHECK(BidPrice == 120);
  BOOST_CHECK(BidVolume == 2000);

  exchange.AddOrder("ANZ", BID, 90, 1000, ++clientOrderId);
  BOOST_CHECK(BidPrice == 120);
  BOOST_CHECK(BidVolume == 2000);

  exchange.RemoveOrder(exchangeOrderId);
  BOOST_CHECK(BidPrice == 120);
  BOOST_CHECK(BidVolume == 2000);
}


BOOST_AUTO_TEST_CASE(AddAndRemoveBestBid)
{
  exchange.AddOrder("BHP", BID, 100, 1000, ++clientOrderId);
  BOOST_CHECK(BidPrice == 100);
  BOOST_CHECK(BidVolume == 1000);

  exchange.AddOrder("BHP", BID, 120, 2000, ++clientOrderId);
  int exchangeOrderId = ExchangeOrderId;
  BOOST_CHECK(BidPrice == 120);
  BOOST_CHECK(BidVolume == 2000);

  exchange.AddOrder("BHP", BID, 100, 1000, ++clientOrderId);
  BOOST_CHECK(BidPrice == 120);
  BOOST_CHECK(BidVolume == 2000);

  exchange.RemoveOrder(exchangeOrderId);

  BOOST_CHECK(BidPrice == 100);
  BOOST_CHECK(BidVolume == 2000);
  BOOST_CHECK(AskPrice == 0);
  BOOST_CHECK(AskVolume == 0);
}

BOOST_AUTO_TEST_CASE(AddAndRemoveBestAsk)
{
  exchange.AddOrder("BHP", ASK, 120, 1000, ++clientOrderId);
  BOOST_CHECK(AskPrice == 120);
  BOOST_CHECK(AskVolume == 1000);

  exchange.AddOrder("BHP", ASK, 100, 2000, ++clientOrderId);
  int exchangeOrderId = ExchangeOrderId;
  BOOST_CHECK(AskPrice == 100);
  BOOST_CHECK(AskVolume == 2000);

  exchange.AddOrder("BHP", ASK, 120, 1000, ++clientOrderId);
  BOOST_CHECK(AskPrice == 100);
  BOOST_CHECK(AskVolume == 2000);

  exchange.RemoveOrder(exchangeOrderId);

  BOOST_CHECK(BidPrice == 0);
  BOOST_CHECK(BidVolume == 0);
  BOOST_CHECK(AskPrice == 120);
  BOOST_CHECK(AskVolume == 2000);
}


BOOST_AUTO_TEST_SUITE_END()
