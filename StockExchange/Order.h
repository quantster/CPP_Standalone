#ifndef _Order_H_
#define _Order_H_

#include <string>
#include <boost/shared_ptr.hpp>

using std::string;
using boost::shared_ptr;

namespace Tibra
{
namespace Exchange
{

  struct Order
  {
    string stockCode;
    bool bidSide;
    int price;
    int volume;
    int clientOrderId;
    int exchangeOrderId;

    Order(
          const string& _stockCode,
          bool _bidSide,
          int _price,
          int _volume,
          int _clientOrderId,
          int _exchangeOrderId)
      : stockCode(_stockCode),
        bidSide(_bidSide),
        price(_price),
        volume(_volume),
        clientOrderId(_clientOrderId),
        exchangeOrderId(_exchangeOrderId) {}
  };

  typedef shared_ptr<Order> tOrderPtr;

  struct pCompareOrder
  {
    inline bool operator()(const tOrderPtr& lhs, const tOrderPtr& rhs)
    {
      return (lhs->exchangeOrderId == rhs->exchangeOrderId);
    }
  };

}
}

#endif
