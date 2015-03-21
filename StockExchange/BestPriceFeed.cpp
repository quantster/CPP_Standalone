// ==============================================================================
// Author: Nitish Kanabar nkanabar@gmail.com
// ==============================================================================

#include <utility>
#include "StockExchange.h"
#include "BestPriceFeed.h"

using namespace Tibra::Exchange;

using std::make_pair;

// ------------------------------------------------------------------------------
//  Add a order to the feed.
//  Create the bid-map and the ask-map if this stock-code is seen for the first time
//  else update the volumes at the appropriate price-levels.
//  Call the best-price feed event if that information has changed.
void
BestPriceFeed::AddOrder(const tOrderPtr& order)
{
  bool triggerBestFeedEvent = false;

  tTotalVolumes::iterator itr = totalVolumes->find(order->stockCode);
  if (itr == totalVolumes->end()) {
    // create the bid-map and the ask-map
    tPrice2VolumesPtr bidVolumes(new tPrice2Volumes);
    tPrice2VolumesPtr askVolumes(new tPrice2Volumes);

    order->bidSide ? 
          bidVolumes->insert(make_pair<int,int>(order->price, order->volume))
        : askVolumes->insert(make_pair<int,int>(order->price, order->volume));

    // add the bid-map and ask-map pair to the totalVolumes map
    pair<tTotalVolumes::iterator, bool> returnPair = 
    totalVolumes->insert(
        make_pair<string, tVolumesPair>(
          order->stockCode, 
          make_pair<tPrice2VolumesPtr, tPrice2VolumesPtr>(bidVolumes, askVolumes)));

    itr = returnPair.first;

    triggerBestFeedEvent = true;
  }
  else {
    // the maps already exist, so update the appropriate volume
    tPrice2VolumesPtr mapVolumes;
    int bestPrice = 0;

    if (order->bidSide) {
      // use the bids-map 
      // best-price is the highest bid price. i.e the last element
      mapVolumes = (*itr).second.first; 
      bestPrice = mapVolumes->empty() ? order->price : (*mapVolumes->rbegin()).first;
      triggerBestFeedEvent = (bestPrice <= order->price);
    }
    else {
      // use the bids-map 
      // best-price is the lowest ask price. i.e the first element
      mapVolumes = (*itr).second.second;
      bestPrice = mapVolumes->empty() ? order->price : (*mapVolumes->begin()).first;
      triggerBestFeedEvent = (bestPrice >= order->price);
    }

    // update the volume for the price level of the order
    tPrice2Volumes::iterator itr2 = mapVolumes->find(order->price);
    if (itr2 == mapVolumes->end())
      mapVolumes->insert(make_pair<int,int>(order->price, order->volume));
    else 
      (*itr2).second += order->volume;
  }

  if (triggerBestFeedEvent) callOnBestPriceFeed(itr);
}

// ------------------------------------------------------------------------------
//  Remove an order from the feed
//  If the volume at the price-level falls to 0, then remove that element from
//  the map.
//  Call the best-price event if that information has changed.
//  We should always be able to find the maps and the price-level - not being
//  able to find these is an internal error that we communicate to our client -
//  the exchange using exceptions.
void
BestPriceFeed::RemoveOrder(const tOrderPtr& order)
{
  tTotalVolumes::iterator itr = totalVolumes->find(order->stockCode);

  if (itr == totalVolumes->end())
    throw (IStockExchange::eINTERNAL_SYSTEM_FAILURE);

  bool triggerBestFeedEvent = false;

  tPrice2VolumesPtr mapVolumes;
  int bestPrice = 0;

  if (order->bidSide) {
    mapVolumes = (*itr).second.first;
    bestPrice = mapVolumes->empty() ? order->price : (*mapVolumes->rbegin()).first;
    triggerBestFeedEvent = (bestPrice <= order->price);
  }
  else {
    mapVolumes = (*itr).second.second;
    bestPrice = mapVolumes->empty() ? order->price : (*mapVolumes->begin()).first;
    triggerBestFeedEvent = (bestPrice >= order->price);
  }

  // find the price level and update the corresponding volume
  tPrice2Volumes::iterator itr2 = mapVolumes->find(order->price);

  if (itr2 == mapVolumes->end())
    throw (IStockExchange::eINTERNAL_SYSTEM_FAILURE);

  if (((*itr2).second -= order->volume) == 0)
    mapVolumes->erase(itr2);

  if (triggerBestFeedEvent) callOnBestPriceFeed(itr);
}


// ------------------------------------------------------------------------------
//  helper method to call the specified callback
void
BestPriceFeed::callOnBestPriceFeed(const tTotalVolumes::iterator& itr) const
{
  if (exchange.OnBestPriceFeed.empty()) return;

  int bestBidPrice = 0;
  int bestBidVolume = 0;
  int bestAskPrice = 0;
  int bestAskVolume = 0;

  tPrice2VolumesPtr bidVolumes = (*itr).second.first;
  if (!bidVolumes->empty()) {
    tPrice2Volumes::reverse_iterator bidVolumesItr = bidVolumes->rbegin();

    bestBidPrice = (*bidVolumesItr).first;
    bestBidVolume = (*bidVolumesItr).second;
  }

  tPrice2VolumesPtr askVolumes = (*itr).second.second;
  if (!askVolumes->empty()) {
    tPrice2Volumes::iterator askVolumesItr = askVolumes->begin();;
    bestAskPrice = (*askVolumesItr).first;
    bestAskVolume = (*askVolumesItr).second;
  }

   exchange.OnBestPriceFeed((*itr).first,
      bestBidPrice,
      bestBidVolume,
      bestAskPrice,
      bestAskVolume);
}

