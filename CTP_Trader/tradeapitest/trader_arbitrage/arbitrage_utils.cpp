#include "arbitrage_utils.h"

CalendarSpreadArbitrage::CalendarSpreadArbitrage(CMdHandler *md, CTraderHandler *tr, CThostFtdcTraderApi *pTraderApi, char **instrumentIDs) {
    this->md = md;
    this->tr = tr;
    this->pTraderApi = pTraderApi;
    this->instrumentIDs = instrumentIDs;
    this->instrumenIDs_str[0] = instrumentIDs[0];
    this->instrumenIDs_str[1] = instrumentIDs[1];
    this->stop_flag = false;     // should be false;
}

void CalendarSpreadArbitrage::priceDiffUpdate() {
    // Assume that both marketData exists.
    this->spread_long = this->md->marketData[this->instrumenIDs_str[0]]->AskPrice1 - this->md->marketData[this->instrumenIDs_str[1]]->BidPrice1;
    this->spread_short = this->md->marketData[this->instrumenIDs_str[0]]->BidPrice1 - this->md->marketData[this->instrumenIDs_str[1]]->AskPrice1;
}

void CalendarSpreadArbitrage::positionUpdate() {
    printf("Updating the current positions.\n");
    // First check whether the user is holding long/short positions for the corresponding instrument.
    if (this->tr->positionLongData.count(this->instrumenIDs_str[0]) > 0 || this->tr->positionShortData.count(this->instrumenIDs_str[0]) > 0) {
        pTraderInvestorPosition(this->pTraderApi, this->instrumentIDs[0]);
    }
    
    if (this->tr->positionLongData.count(this->instrumenIDs_str[1]) > 0 || this->tr->positionShortData.count(this->instrumenIDs_str[1]) > 0) {
        pTraderInvestorPosition(this->pTraderApi, this->instrumentIDs[1]);
    }
}

void CalendarSpreadArbitrage::openPosition() {
    // Need to make sure no instrument positions are being hold long/short before opening.
    if (this->tr->positionLongData.count(this->instrumenIDs_str[0]) == 0 && this->tr->positionShortData.count(this->instrumenIDs_str[0]) == 0 
        && this->tr->positionLongData.count(this->instrumenIDs_str[1]) == 0 && this->tr->positionShortData.count(this->instrumenIDs_str[1]) == 0) {

        // Buy open the close instrument and Sell open the far instrument.
        if (this->spread_long <= this->threshold[0]) {
            pTraderOrderInsert(this->pTraderApi, this->instrumentIDs[0], 0);
            pTraderOrderInsert(this->pTraderApi, this->instrumentIDs[1], 1);
            positionUpdate();
        }
        // Buy open the far instrument and Sell open the close instrument.
        else if (this->spread_short >= this->threshold[1]) {
            pTraderOrderInsert(this->pTraderApi, this->instrumentIDs[1], 0);
            pTraderOrderInsert(this->pTraderApi, this->instrumentIDs[0], 1);
            positionUpdate();
        }
    }
}

void CalendarSpreadArbitrage::closePosition() { 
    // Check if have the long position for the close instrument.
    if (this->tr->positionLongData[this->instrumenIDs_str[0]]->Position > 0) {
        // Close the position if can make money / reduce loss.
        if (this->spread_short >= this->threshold[1] || this->spread_short <= this->threshold_stoppoint[0]) {
            pTraderOrderInsert(this->pTraderApi, this->instrumentIDs[0], 3);
            pTraderOrderInsert(this->pTraderApi, this->instrumentIDs[1], 2);
            positionUpdate();
        }
    }

    // Check if have the short position for the close instrument.
    if (this->tr->positionShortData[this->instrumenIDs_str[0]]->Position > 0) {
        // Close the position if can make money / reduce loss.
        if (this->spread_long <= this->threshold[0] || this->spread_long >= this->threshold_stoppoint[1]) {
            pTraderOrderInsert(this->pTraderApi, this->instrumentIDs[0], 2);
            pTraderOrderInsert(this->pTraderApi, this->instrumentIDs[1], 3);
            positionUpdate();
        }
    }
}

void CalendarSpreadArbitrage::run() {
    printf("=========== Starting the Calendar Spread Strategy ===========\n");

    while (true) {
        if (this->md->tick_change) {

            this->priceDiffUpdate();
            // this->closePosition();
            // this->openPosition();
            // this->md->tick_change = false;
        }

        sleep(5);

        if (stop_flag) {
            printf("=========== Finishing the Calendar Spread Strategy ===========\n");
            break;
        }
    }
}

ButterflySpreadArbitrage::ButterflySpreadArbitrage(CMdHandler *md, CTraderHandler *tr, CThostFtdcTraderApi *pTraderApi, char **instrumentIDs) {
    this->md = md;
    this->tr = tr;
    this->pTraderApi = pTraderApi;
    this->instrumentIDs = instrumentIDs;
    this->instrumenIDs_str[0] = instrumentIDs[0];
    this->instrumenIDs_str[1] = instrumentIDs[1];
    this->instrumenIDs_str[2] = instrumentIDs[2];
    this->stop_flag = false;     // should be false;
}

void ButterflySpreadArbitrage::stockPriceUpdate() {
    // TODO: currentStockPrice of the product.
    // this->currentStockPrice = 
}

void ButterflySpreadArbitrage::positionUpdate() {
    printf("Updating the current positions.\n");
    // First check whether the user is holding long/short positions for the corresponding instrument.
    if (this->tr->positionLongData.count(this->instrumenIDs_str[0]) > 0 || this->tr->positionShortData.count(this->instrumenIDs_str[0]) > 0) {
        pTraderInvestorPosition(this->pTraderApi, this->instrumentIDs[0]);
    }
    
    if (this->tr->positionLongData.count(this->instrumenIDs_str[1]) > 0 || this->tr->positionShortData.count(this->instrumenIDs_str[1]) > 0) {
        pTraderInvestorPosition(this->pTraderApi, this->instrumentIDs[1]);
    }

    if (this->tr->positionLongData.count(this->instrumenIDs_str[2]) > 0 || this->tr->positionShortData.count(this->instrumenIDs_str[2]) > 0) {
        pTraderInvestorPosition(this->pTraderApi, this->instrumentIDs[2]);
    }
}


void ButterflySpreadArbitrage::openPosition() {
    // Need to make sure no instrument positions are being hold long/short before opening.
    if (this->tr->positionLongData.count(this->instrumenIDs_str[0]) == 0 && this->tr->positionShortData.count(this->instrumenIDs_str[0]) == 0 
        && this->tr->positionLongData.count(this->instrumenIDs_str[1]) == 0 && this->tr->positionShortData.count(this->instrumenIDs_str[1]) == 0
        && this->tr->positionLongData.count(this->instrumenIDs_str[2]) == 0 && this->tr->positionShortData.count(this->instrumenIDs_str[2]) == 0) {

        // Long butterfly positioning.
        // Assuming that the stock price will not fluctuate much around the middle strike price.
        // If the stockPrice is close to the middle strike price (in the profit region), then open the position.
        this->netCost = 2 * this->md->marketData[instrumentIDs[1]]->BidPrice1 - this->md->marketData[instrumentIDs[0]]->AskPrice1 - this->md->marketData[instrumentIDs[2]]->AskPrice1;
        this->profitLower = this->md->strikePrice[instrumenIDs_str[0]] + this->netCost;
        this->profitUpper = this->md->strikePrice[instrumenIDs_str[2]] - this->netCost;


        if (this->md->stockPrice >= this->profitLower && this->md->stockPrice <= this->profitUpper) {
            pTraderOrderInsert(this->pTraderApi, this->instrumentIDs[0], 0);
            pTraderOrderInsert(this->pTraderApi, this->instrumentIDs[1], 1);
            pTraderOrderInsert(this->pTraderApi, this->instrumentIDs[1], 1);
            pTraderOrderInsert(this->pTraderApi, this->instrumentIDs[2], 0);
            positionUpdate();
            return;
        }

        // Short butterfly positioning.
        // Assuming that the stock price will not fluctuate much around the middle strike price.
        // If the stockPrice is far from the middle strike price (in the profit region), then open the position.
        this->netCost = this->md->marketData[instrumentIDs[0]]->BidPrice1 +  this->md->marketData[instrumentIDs[2]]->BidPrice1 - 2 * this->md->marketData[instrumentIDs[1]]->AskPrice1;
        this->profitLower = this->md->strikePrice[instrumenIDs_str[0]] + this->netCost;
        this->profitUpper = this->md->strikePrice[instrumenIDs_str[2]] - this->netCost;

        if (this->md->stockPrice < this->profitLower || this->md->stockPrice > this->profitUpper) {
            pTraderOrderInsert(this->pTraderApi, this->instrumentIDs[0], 1);
            pTraderOrderInsert(this->pTraderApi, this->instrumentIDs[1], 0);
            pTraderOrderInsert(this->pTraderApi, this->instrumentIDs[1], 0);
            pTraderOrderInsert(this->pTraderApi, this->instrumentIDs[2], 1);
            positionUpdate();
            return;
        }
    }
}

void ButterflySpreadArbitrage::closePosition() {
    // Check if have the long position for the close instrument.
    if (this->tr->positionLongData[this->instrumenIDs_str[0]]->Position > 0) {
        // TODO: If reaches expiration date, then decide whether to expire or execute.
        // if (this->expireDate[this->instrumenIDs_str[0]].compare()
    }
}


void ButterflySpreadArbitrage::run() {
    printf("=========== Starting the Butterfly Spread Arbitrage ===========\n");

    while (true) {
        if (this->md->tick_change) {

            this->stockPriceUpdate();
            // this->closePosition();
            // this->openPosition();
            // this->md->tick_change = false;
        }

        sleep(5);

        if (stop_flag) {
            printf("=========== Finishing the Butterfly Spread Arbitrage ===========\n");
            break;
        }
    }
}

