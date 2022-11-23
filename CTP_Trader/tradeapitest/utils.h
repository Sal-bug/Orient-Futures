#include "common.h"

void pTraderInit(CThostFtdcTraderApi *pTraderApi, CThostFtdcReqUserLoginField *userLoginField, 
    CThostFtdcReqAuthenticateField *userAuthField);

void pTraderLogout(CThostFtdcTraderApi *pTraderApi, CThostFtdcReqUserLoginField *userLoginField, 
    CThostFtdcUserLogoutField *userLogoutField);

void pTraderQryInstrument(CThostFtdcTraderApi *pTraderApi);

void pTraderQryTradingAccount(CThostFtdcTraderApi *pTraderApi);

void pTraderQryOrder(CThostFtdcTraderApi *pTraderApi);

void pTraderQryDepthMarketData(CThostFtdcTraderApi *pTraderApi);

void pTraderInvestorPosition(CThostFtdcTraderApi *pTraderApi, char *InstrumentID);

void pTraderOrderInsert(CThostFtdcTraderApi *pTraderApi, char *InstrumentID, int order_type);

void pTraderOrderAction(CThostFtdcTraderApi *pTraderApi);

void pTraderSettmentInfoConfirm(CThostFtdcTraderApi *pTraderApi);

void pTraderReqOptionSelfCloseInsert(CThostFtdcTraderApi *pTraderApi);

void pTraderQryOptionSelfClose(CThostFtdcTraderApi *pTraderApi);

void pTraderOptionSelfCloseAction(CThostFtdcTraderApi *pTraderApi);

void pMdInit(CThostFtdcMdApi *pMdApi, CThostFtdcReqUserLoginField *userLoginField, 
    CThostFtdcReqAuthenticateField *userAuthField, sem_t *sem, int *requestID);

void pMdLogout(CThostFtdcMdApi *pMdApi, CThostFtdcReqUserLoginField *userLoginField, 
    CThostFtdcUserLogoutField *userLogoutField, sem_t *sem, int *requestID);

void pMdSubscribe(CThostFtdcMdApi *pMdApi, char **pInstrumentID, sem_t *sem);
