#include "arbitrage_utils.h"

using namespace std;

sem_t sem;
int requestID = 0;

int main()
{    
    // Initialization of variables
	sem_init(&sem,0,0);
    CThostFtdcReqUserLoginField userLoginField;
    CThostFtdcUserLogoutField userLogoutField;
    CThostFtdcReqAuthenticateField userAuthField;
    
    char *instrumentIDs[2];                         // Always close first, far second.
    instrumentIDs[0] = (char *)"IO2211-P-3650";
    instrumentIDs[1] = (char *)"IO2211-C-3650";

    // Create MD API instance and register MD SPI.
    CThostFtdcMdApi *pMdApi = CThostFtdcMdApi::CreateFtdcMdApi();
    CMdHandler md(pMdApi);
    pMdApi->RegisterSpi(&md);
    
    printf("Current API version: %s \n", pMdApi->GetApiVersion());
    
    pMdInit(pMdApi, &userLoginField, &userAuthField, &sem, &requestID);
    pMdSubscribe(pMdApi, instrumentIDs, &sem);
    

    sleep(2);
    printf("=========== Finish creating MD instance ===========\n");

    // cout << md.marketData["ru2310"]->InstrumentID << endl;

    // Create Trader API instance and register Trader SPI.
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
    CTraderHandler tr(pTraderApi);
    pTraderApi->RegisterSpi(&tr);

    pTraderInit(pTraderApi, &userLoginField, &userAuthField);
    printf("=========== Finish creating Trader instance ===========\n");

    // pTraderSettmentInfoConfirm(pTraderApi);
    pTraderQryInstrument(pTraderApi);

    // SpreadArbitrage spread_arbitrage = SpreadArbitrage(&md, &tr, pTraderApi, instrumentIDs);
    // spread_arbitrage.run();

    while (true) {
        sleep(2);
    }
    // Finishing the program.
    // pMdApi->Release();
    // pTraderApi->Release();

	printf("Finish!\n");
    return(0);
}