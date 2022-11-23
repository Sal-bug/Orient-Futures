#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <iostream>

#include "../api/ThostFtdcTraderApi.h"
#include "../api/ThostFtdcMdApi.h"
#include "../api/ThostFtdcUserApiDataType.h"
#include "../api/ThostFtdcUserApiStruct.h"

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

void pMdInit(CThostFtdcMdApi *pMdApi, CThostFtdcReqUserLoginField *userLoginField, 
    CThostFtdcReqAuthenticateField *userAuthField, sem_t *sem, int *requestID);

void pMdLogout(CThostFtdcMdApi *pMdApi, CThostFtdcReqUserLoginField *userLoginField, 
    CThostFtdcUserLogoutField *userLogoutField, sem_t *sem, int *requestID);

void pMdSubscribe(CThostFtdcMdApi *pMdApi, sem_t *sem);
