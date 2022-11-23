#pragma once
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "ThostFtdcTraderApi.h"

class CtpTraderDemo :
	public CThostFtdcTraderSpi
{
public:
	CtpTraderDemo();
	~CtpTraderDemo();
	void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	inline void PrintRspInfo(const char* pFuncName, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	inline char* getOrderRef();
	void Run();
	void InitParam(const char *arg[]);
	void RequestAuth();
	void ReqUserLogin();
	void ReqUserLogout();
	///报单录入请求
	void ReqOrderInsert();
	///报单操作请求
	void ReqOrderAction();
	///请求查询投资者持仓
	void ReqQryInvestorPosition();
	///请求查询资金账户
	void ReqQryTradingAccount();
private:
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason);

	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	///@param nTimeLapse 距离上次接收报文的时间
	virtual void OnHeartBeatWarning(int nTimeLapse);
	
	///客户端认证响应
	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);


	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///报单录入请求响应
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///报单操作请求响应
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///报单通知
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///成交通知
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	///报单录入错误回报
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);

	///报单操作错误回报
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);


	///请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	
	CThostFtdcTraderApi *traderApi;
	char t_FrontAddr[28];//tcp://180.166.103.34:51215
	TThostFtdcBrokerIDType m_BrokerID;
	TThostFtdcInvestorIDType m_InvestorID;
	TThostFtdcUserIDType m_UserID;
	TThostFtdcPasswordType m_PassWord;
	TThostFtdcAppIDType m_AppID;
	TThostFtdcAuthCodeType	m_AuthCode;
	int tRequestID;
	TThostFtdcOrderRefType	m_OrderRef;
};

