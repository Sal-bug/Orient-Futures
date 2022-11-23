#include "../common.h"
#include "../utils.h"
#include "../mdhandler.h"
#include "../traderhandler.h"

class CalendarSpreadArbitrage {
	public:
    CalendarSpreadArbitrage(CMdHandler *md, CTraderHandler *tr, CThostFtdcTraderApi *pTraderApi, char **instrumentIDs);

    ~CalendarSpreadArbitrage();

    // Calculate price difference for both arbitrage situations.
    void priceDiffUpdate();
    // Calculate thresholds for determining open/close.
    void thresholdUpdate();
    // Update the investor positions.
    void positionUpdate();
    // Open positions for the options.
    void openPosition();
    // Close positions for the options.
    void closePosition();
    // Run the strategy in loops.
    void run();

    private:
    double spread_long;         // Long close and short far.
    double spread_short;        // Short close and long far.
    CMdHandler *md;
    CTraderHandler *tr;
    CThostFtdcTraderApi *pTraderApi;
    char **instrumentIDs;
    string instrumenIDs_str[2];
    double threshold[2];                    // 0 for lower, 1 for upper.
    double threshold_stoppoint[2];          // 0 for lower, 1 for upper.
    bool stop_flag;
};

class ButterflySpreadArbitrage {
    public:
    ButterflySpreadArbitrage(CMdHandler *md, CTraderHandler *tr, CThostFtdcTraderApi *pTraderApi, char **instrumentIDs);

    ~ButterflySpreadArbitrage();

    void stockPriceUpdate();
    // Calculate thresholds for determining open/close.
    void thresholdUpdate();
    // Update the investor positions.
    void positionUpdate();
    // Open positions for the options.
    void openPosition();
    // Close positions for the options.
    void closePosition();
    // Run the strategy in loops.
    void run();

    private:
    double netCost;
    double profitUpper, profitLower;

    CMdHandler *md;
    CTraderHandler *tr;
    CThostFtdcTraderApi *pTraderApi;
    char **instrumentIDs;
    string instrumenIDs_str[3];
    double threshold[2];                    // 0 for lower, 1 for upper.
    double threshold_stoppoint[2];          // 0 for lower, 1 for upper.
    bool stop_flag;
};
