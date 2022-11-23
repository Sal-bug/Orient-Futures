#include "common.h"
#include "utils.h"
#include "traderhandler.h"

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

    // Create Trader API instance and register Trader SPI.
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
    CTraderHandler sh(pTraderApi);
    pTraderApi->RegisterSpi(&sh);

    printf("Current API version: %s \n", pTraderApi->GetApiVersion());

    pTraderInit(pTraderApi, &userLoginField, &userAuthField);
    // Needs to be called everytime after login in.
    pTraderSettmentInfoConfirm(pTraderApi);

    // Query information from the api.
    pTraderQryInstrument(pTraderApi);
    // pTraderQryTradingAccount(pTraderApi);
    // pTraderQryOrder(pTraderApi, &sem, &requestID);
    // pTraderQryDepthMarketData(pTraderApi, &sem, &requestID);
    // pTraderQryOptionSelfClose(pTraderApi);

    // Insert or delete an order.
    // pTraderOrderInsert(pTraderApi);
    // pTraderOrderAction(pTraderApi, &sem, &requestID);
    // pTraderReqOptionSelfCloseInsert(pTraderApi);
    // pTraderOptionSelfCloseAction(pTraderApi);
    
    // pTraderInvestorPosition(pTraderApi);
    

	// pTraderLogout(pTraderApi, &userLoginField, &userLogoutField);

    pTraderApi->Release();
	printf("Finish!\n");
    return(0);
}
