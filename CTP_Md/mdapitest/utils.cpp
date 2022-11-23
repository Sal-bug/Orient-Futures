#include "utils.h"

// Initialization of the trader API, including register, subscription, authentication and login.
void pMdInit(CThostFtdcMdApi *pMdApi, CThostFtdcReqUserLoginField *userLoginField, 
    CThostFtdcReqAuthenticateField *userAuthField, sem_t *sem, int *requestID) {
    strcpy(userLoginField->BrokerID, "9999");
    strcpy(userLoginField->UserID, "206045");
    strcpy(userLoginField->Password, "Thzss200718!");

    // strcpy(userLoginField->BrokerID, "6666");
    // strcpy(userLoginField->UserID, "66620070");
    // strcpy(userLoginField->Password, "dzqh1234");

    // Set the server address.
    // pMdApi->RegisterFront((char *)"tcp://180.168.146.187:10201");
    pMdApi->RegisterFront((char *)"tcp://180.166.103.34:51218");
    // pMdApi->RegisterFront((char *)"tcp://218.202.237.33:10203");

    // Link to the trading system.
    pMdApi->Init();
	sem_wait(sem);

    pMdApi->ReqUserLogin(userLoginField, *requestID++);
    sem_wait(sem);
}

// User logout and record the user information.
void pMdLogout(CThostFtdcMdApi *pMdApi, CThostFtdcReqUserLoginField *userLoginField, 
    CThostFtdcUserLogoutField *userLogoutField, sem_t *sem, int *requestID) {
    // Copy logout information.
	strcpy(userLogoutField->BrokerID,userLoginField->BrokerID);
	strcpy(userLogoutField->UserID, userLoginField->UserID);

	pMdApi->ReqUserLogout(userLogoutField, *requestID++);
	sem_wait(sem);
}

// Subscribe current instrument information.
void pMdSubscribe(CThostFtdcMdApi *pMdApi, sem_t *sem) {
    TThostFtdcInstrumentIDType pInstrumentID = "cu2308";
	char *ppInstrumentID[1];
	ppInstrumentID[0]= pInstrumentID;   
    pMdApi->SubscribeMarketData(ppInstrumentID, 1);
	sem_wait(sem);
}