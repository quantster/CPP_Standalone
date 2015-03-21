// ==============================================================================
// Author: Nitish Kanabar nkanabar@gmail.com
// ------------------------------------------------------------------------------
//
// This class maintains the best price and volume for orders in our exchange.
//
// ------------------------------------------------------------------------------
// DESIGN NOTES:
// 
// TRIVIAL approach:
// Maintaining the best price and volume when an order is added is trivial - we
// keep track of the highest bid and lowest offer and their corresponding volumes. 
// When an order arrives we compare its attributes against our stored values and
// update the stored values if required.
//
// NON-TRIVIAL approach:
// HOWEVER, we want to ensure the integrity of our feed when an order is REMOVED.
// The naive approach described above fails to handle order removal correctly.
// Say we remove the order that corresponds to the best price - in this case the
// prior second-best price and volume are promoted to the new best-rocie and volume.
// Thus we need to keep track of the second-best price and volume too.
// Extending this argument further, we see that we need to keep track of the
// volumes at ALL price levels.
//
// To keep track of the volume at each price level, I have used the STL map with
// key = price and value = totalVolume at that price. 
// Each side (bid and ask) gets its own map  - called bid-map and ask-map.
// Updating the volume when an order is added or removed involves updating the
// appropriate element of the appropriate map. When the volume at any price level
// falls to 0, we remove that price-level from our map.
// We use the fact that the STL map is SORTED by key to find the best-price and
// volume - for bids the highest price, and therefore the first element of the
// map, is the best price. For asks the lowest price, and therefore the last
// element of the asks-map, is the best price.
//
// The bid and ask maps are stored together in a pair for convenience. Each
// supported stock-code gets its own pair of maps. The entire collection is
// itself stored in another map with key = stockCode and value = pair of maps.
//
// So, to lookup / update the volume at any price-level involves first looking
// up the pair of maps, then looking up the volume at that price-level.
//
// This approach is also extensible in the sense that we can extend this class
// to implement querying/messaging at different price levels rather than just
// at the best price level.
//
// ==============================================================================
#ifndef _BestPriceFeed_H_
#define _BestPriceFeed_H_

#include <string>
#include <utility>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "Order.h"

using std::pair;
using std::string;
using std::map;
using boost::scoped_ptr;
using boost::shared_ptr;

namespace Tibra
{
namespace Exchange
{

  class StockExchange;

  class BestPriceFeed
  {
    public:
      BestPriceFeed(const StockExchange& _exchange)
        : exchange(_exchange), totalVolumes(new tTotalVolumes) {}

      virtual ~BestPriceFeed() {}

      virtual void AddOrder(const tOrderPtr&);
      virtual void RemoveOrder(const tOrderPtr&);

    private:
      const StockExchange& exchange;

      typedef map<int, int> tPrice2Volumes;
      typedef shared_ptr<tPrice2Volumes> tPrice2VolumesPtr;
      typedef pair<tPrice2VolumesPtr, tPrice2VolumesPtr> tVolumesPair;
      typedef map<string, tVolumesPair> tTotalVolumes;
      
      scoped_ptr<tTotalVolumes> totalVolumes;

      inline void callOnBestPriceFeed(const tTotalVolumes::iterator&) const;
  };

}
}

#endif
