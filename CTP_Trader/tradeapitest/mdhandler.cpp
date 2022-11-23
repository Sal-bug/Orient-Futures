#include "mdhandler.h"

CMdHandler::CMdHandler(CThostFtdcMdApi *pMdApi) : m_pMdApi(pMdApi) {
    printf("CMdHandler:called.\n");
    this->tick_change = false;
}

CMdHandler::~CMdHandler() {

    for(map<string, CThostFtdcDepthMarketDataField *>::iterator it = this->marketData.begin(); it != this->marketData.end(); ++it) {
        delete it->second; 
    }

}

void CMdHandler::OnFrontConnected() {
    static int i = 0;
    if (i++ == 0) {
        printf("OnFrontConnected:called.\n");
        sem_post(&sem);
    }
}

void CMdHandler::OnFrontDisconnected(int nReason) {
    printf("OnFrontDisconnected.\n");
}

void CMdHandler::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    printf("OnRspUserLogin:called\n");

    if (pRspInfo->ErrorID == 0) {
        printf("Login Success!\n");			
        sem_post(&sem);
    }
    else {
        printf("Failed to login, errorcode=%d errormsg=%s requestid=%d chain=%d",
        pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
        exit(-1);
    }
}

void CMdHandler::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {	
    printf("OnReqUserLogout:called\n");
    if (pRspInfo->ErrorID == 0) {
        printf("Logout Success!\n");
        sem_post(&sem);
    }
    else{
        printf("Logout Failure!\n");
    }
}


void CMdHandler::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
        printf("OnRspError:called\n");

        char ErrorMsg[243];
        gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
        printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, ErrorMsg);
        printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
}

void CMdHandler::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    printf("OnRspSubMarketData:called...\n");
    // 进程返回结果检查
    if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
        char ErrorMsg[243];
        gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
        printf("OnRspSubMarketData:Error:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
        exit(-1);
    }

    if (pSpecificInstrument != NULL) {
        char InstrumentID[81];
        gbk2utf8(pSpecificInstrument ->InstrumentID, InstrumentID, sizeof(InstrumentID));
        printf("InstrumentID=%s\n", InstrumentID);
    }


    if (bIsLast) {
        // 通知主过程，响应函数将结束
        sem_post(&sem);
    }
};


void CMdHandler::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField * pDepthMarketData) {
    printf("OnRtnDepthMarketData:called\n");

    printf("InstrumentID=%s, UpdateTime=%s, UpdateMillisec=%d, LastPrice=%lf, Volume=%d\n",pDepthMarketData->InstrumentID,pDepthMarketData->UpdateTime,pDepthMarketData->UpdateMillisec, pDepthMarketData->LastPrice, pDepthMarketData->Volume);
    // printf("UpperLimitPrice=%lf, LowerLimitPrice=%lf\n", pDepthMarketData->UpperLimitPrice, pDepthMarketData->LowerLimitPrice);
    printf("AskPrice=%f, AskVolume=%d, BidPrice=%f, BidVolume=%d\n", pDepthMarketData->AskPrice1, pDepthMarketData->AskVolume1, pDepthMarketData->BidPrice1, pDepthMarketData->BidVolume1);
    
    // Record market data.
    string InsturmentID((*pDepthMarketData).InstrumentID);
    if (this->marketData.count(InsturmentID) > 0) {
        delete this->marketData[InsturmentID];
    }

    this->marketData[InsturmentID] = new CThostFtdcDepthMarketDataField (*pDepthMarketData);
    this->tick_change = true;

    // TODO: Record the strike price and the expire date of the instrument from the name of the instrument.
    // this->strikePrice[] = ...
    // this->expireDate[] = ...
}