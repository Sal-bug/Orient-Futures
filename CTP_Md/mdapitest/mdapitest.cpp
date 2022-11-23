
#include "mdapitest.h"
#include "utils.h"

// tradeapitest.cpp :
// 一个简单的例子，介绍CThostFtdcTraderApi和CThostFtdcTraderSpi接口的使用。
// 本例将演示一个报单录入操作的过程


// 报单录入操作是否完成的标志
// Create a manual reset event with no signal
// 这段需要注释掉，这是windows下的代码
// TODO 映射功能到linux
//HANDLE g_hEvent = CreateEvent(NULL, true, false, NULL);


using namespace std;

sem_t sem;

class CMdHandler : public CThostFtdcMdSpi{

	public:

    CMdHandler(CThostFtdcMdApi *pMdApi) : m_pMdApi(pMdApi) {
        printf("CMdHandler:called.");
    }
    ~CMdHandler() {}

    virtual void OnFrontConnected() {
		static int i = 0;
		if (i++ == 0) {
        	printf("OnFrontConnected:called.\n");
            sem_post(&sem);
		}
    }

    virtual void OnFrontDisconnected(int nReason)
    {
     	printf("OnFrontDisconnected.\n");
    }

    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
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

	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {	
		printf("OnReqUserLogout:called\n");
		if (pRspInfo->ErrorID == 0) {
			printf("Logout Success!\n");
			sem_post(&sem);
		}else{
			printf("Logout Failure!\n");
		}
	}


    virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
        printf("OnRspError:called\n");

        char ErrorMsg[243];
        gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
        printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, ErrorMsg);
        printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
    }

    virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
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


    virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField * pDepthMarketData) {
        printf("OnRtnDepthMarketData:called");

        printf("InstrumentID=%s, UpdateTime=%s, UpdateMillisec=%d, LastPrice=%lf, Volume=%d\n",pDepthMarketData->InstrumentID,pDepthMarketData->UpdateTime,pDepthMarketData->UpdateMillisec, pDepthMarketData->LastPrice, pDepthMarketData->Volume);
        printf("UpperLimitPrice=%lf, LowerLimitPrice=%lf\n", pDepthMarketData->UpperLimitPrice, pDepthMarketData->LowerLimitPrice);
    }



   
    private:

    CThostFtdcMdApi *m_pMdApi;
};



int main()
{
    int requestID = 0;
    
    // Initialization of variables
	sem_init(&sem,0,0);
    CThostFtdcReqUserLoginField userLoginField;
    CThostFtdcUserLogoutField userLogoutField;
    CThostFtdcReqAuthenticateField userAuthField;

    // Create Trader API instance and register Trader SPI.
    CThostFtdcMdApi *pMdApi = CThostFtdcMdApi::CreateFtdcMdApi();
    CMdHandler sh(pMdApi);
    pMdApi->RegisterSpi(&sh);

    pMdInit(pMdApi, &userLoginField, &userAuthField, &sem, &requestID);

    printf("Current API version: %s \n", pMdApi->GetApiVersion());
    pMdSubscribe(pMdApi, &sem);

	// pMdLogout(pMdApi, &userLoginField, &userLogoutField, &sem, &requestID);
    // sleep(1000);
    
    pMdApi->Release();
	printf("Finish!\n");
    return(0);
}