#include <string>
#include <boost/function.hpp>

using boost::function;

namespace Tibra
{
namespace Exchange
{

  class IStockExchange
  {
    public:

      // public methods

      virtual ~IStockExchange() {}

      virtual void AddOrder(
          const std::string& stockCode,
          bool bidSide,
          int price,
          int volume,
          int clientOrderId) = 0;

      virtual void RemoveOrder(
          int exchangeOrderId) = 0;

      // public events
      function<void
        (int exchangeOrderId, int clientOrderId, int errorCode)> OnOrderAdded;

      function<void
        (int exchangeOrderId, int errorCode)> OnOrderRemoved;

      function<void
        (const std::string& stockCode, 
         int bidPrice, int bidVolume,
         int askPrice, int askVolume)> OnBestPriceFeed;

      // error codes
      enum tCallBackErrorCodes
      {
        eNO_ERROR = 0,
        eINTERNAL_SYSTEM_FAILURE = 1,
        eNONEXISTENT_ORDER = 2,
        eUNKNOWN_STOCK_CODE = 3,
        eINVALID_ORDER_PRICE = 4,
        eINVALID_ORDER_VOLUME = 5,
      };

  };

}
}
