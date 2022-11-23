#include<stdio.h>
#include<string.h>
#include "CtpMdDemo.h"



CtpMdDemo::CtpMdDemo():mdApi(CThostFtdcMdApi::CreateFtdcMdApi()){}


CtpMdDemo::~CtpMdDemo()
{
	if (mdApi) { 
		mdApi->Release(); 
		mdApi = 0;
	}
}

inline void CtpMdDemo::PrintRspInfo(const char * pFuncName, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	if(pRspInfo)
	printf("%s:nRequestID=%d,bIsLast=%d,ErrorID=%d,ErrorMsg=%s\n", pFuncName, nRequestID, bIsLast, pRspInfo->ErrorID, pRspInfo->ErrorMsg);
}

void CtpMdDemo::Run()
{
	const char *arg[] = { "tcp://180.166.103.34:51218","6666","66617019","123456" };
	InitParam(arg);
	int i = 1;
	while (i) {
		printf("-------------------------0.exit--------------------------------\n");
		printf("-------------------------1.ReqUserLogin--------------------------------\n");
		printf("-------------------------2.Logout--------------------------------\n");
		printf("-------------------------3.SubscribeMarketData----------------------------\n");
		printf("-------------------------4.UnSubscribeMarketData-----------------------------\n");
		scanf("%d", &i);
		switch (i) {
		case 0: printf("0.exit\n"); break;
		case 1: printf("1.ReqUserLogin\n"); ReqUserLogin(); break;
		case 2: printf("2.ReqUserLogout\n"); ReqUserLogout(); break;
		case 3: printf("2.SubscribeMarketData\n"); SubscribeMarketData(); break;
		case 4: printf("3.UnSubscribeMarketData\n"); UnSubscribeMarketData(); break;
		default: printf("Invalid input\n"); break;
		}
	}
	printf("main end\n");

}

void CtpMdDemo::InitParam(const char *arg[])
{
	strcpy(m_FrontAddr, arg[1]);
	strcpy(m_BrokerID, arg[2]);
	//strcpy(m_InvestorID, arg[3]);
	strcpy(m_UserID, arg[4]);
	strcpy(m_PassWord, arg[5]);
	//strcpy(m_AppID, arg[6]);
	//strcpy(m_AuthCode, arg[7]);
	
	mRequestID=0;

	mdApi->RegisterSpi(this);
	mdApi->RegisterFront(m_FrontAddr);

	mdApi->Init();
	printf("CtpMdDemo::InitParam OK,Version=%s\n", mdApi->GetApiVersion());
}

void CtpMdDemo::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_BrokerID); 
	strcpy(req.UserID, m_UserID); 
	strcpy(req.Password, m_PassWord);
	int ret = mdApi->ReqUserLogin(&req, ++mRequestID);
	printf("ReqUserLogin:%d\n",ret);
}

void CtpMdDemo::ReqUserLogout()
{
	CThostFtdcUserLogoutField pUserLogout;
	memset(&pUserLogout,0,sizeof(CThostFtdcUserLogoutField));
	int ret = mdApi->ReqUserLogout(&pUserLogout, ++mRequestID);
	printf("ReqUserLogin:%d\n", ret);
}

void CtpMdDemo::SubscribeMarketData()
{
	TThostFtdcInstrumentIDType pInstrumentID;
	printf("Input arg:ppInstrumentID(IF1097)\n");
	scanf(" %s", pInstrumentID);
	char *ppInstrumentID[1];
	ppInstrumentID[0]= pInstrumentID;
	int ret=mdApi->SubscribeMarketData(ppInstrumentID,1);
	printf("SubscribeMarketData:%d\n", ret);
}

void CtpMdDemo::UnSubscribeMarketData()
{
	TThostFtdcInstrumentIDType pInstrumentID;
	printf("Input arg:ppInstrumentID(IF1097)\n");
	scanf(" %s", pInstrumentID);
	char *ppInstrumentID[1];
	ppInstrumentID[0] = pInstrumentID;
	int ret = mdApi->UnSubscribeMarketData(ppInstrumentID, 1);
	printf("UnSubscribeMarketData:%d\n", ret);
}

void CtpMdDemo::OnFrontConnected()
{
	printf("MdOnFrontConnected\n");
}

void CtpMdDemo::OnFrontDisconnected(int nReason)
{
	printf("MdOnFrontDisconnected:nReason=%d\n", nReason);
}

void CtpMdDemo::OnHeartBeatWarning(int nTimeLapse)
{
}

void CtpMdDemo::OnRspUserLogin(CThostFtdcRspUserLoginField * pRspUserLogin, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	PrintRspInfo("OnRspUserLogin", pRspInfo, nRequestID, bIsLast);
}

void CtpMdDemo::OnRspUserLogout(CThostFtdcUserLogoutField * pUserLogout, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	PrintRspInfo("OnRspUserLogout", pRspInfo, nRequestID, bIsLast);
}

void CtpMdDemo::OnRspError(CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	PrintRspInfo("OnRspError", pRspInfo, nRequestID, bIsLast);
}

void CtpMdDemo::OnRspSubMarketData(CThostFtdcSpecificInstrumentField * pSpecificInstrument, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	PrintRspInfo("OnRspSubMarketData", pRspInfo, nRequestID, bIsLast);
}

void CtpMdDemo::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField * pSpecificInstrument, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	PrintRspInfo("OnRspUnSubMarketData", pRspInfo, nRequestID, bIsLast);
}

void CtpMdDemo::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField * pSpecificInstrument, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
}

void CtpMdDemo::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField * pSpecificInstrument, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
}

void CtpMdDemo::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField * pDepthMarketData)
{
	printf("OnRtnDepthMarketData:InstrumentID=%s,UpdateTime=%s,UpdateMillisec=%d,LastPrice=%lf,Volume=%d\n",pDepthMarketData->InstrumentID,pDepthMarketData->UpdateTime,pDepthMarketData->UpdateMillisec, pDepthMarketData->LastPrice, pDepthMarketData->Volume);
	printf("UpperLimitPrice=%lf,LowerLimitPrice=%lf\n", pDepthMarketData->UpperLimitPrice, pDepthMarketData->LowerLimitPrice);
}

void CtpMdDemo::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField * pForQuoteRsp)
{
}
