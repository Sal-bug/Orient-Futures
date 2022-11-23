#include "CtpTraderDemo.h"

CtpTraderDemo::CtpTraderDemo() :traderApi(CThostFtdcTraderApi::CreateFtdcTraderApi())
{

}


CtpTraderDemo::~CtpTraderDemo()
{
if (traderApi) {
	traderApi->Release();
	traderApi = 0;
}
}

void CtpTraderDemo::OnRspError(CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	PrintRspInfo("OnRspError", pRspInfo, nRequestID, bIsLast);
}

inline void CtpTraderDemo::PrintRspInfo(const char * pFuncName, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspInfo)
		printf("%s:nRequestID=%d,bIsLast=%d,ErrorID=%d\n", pFuncName, nRequestID, bIsLast, pRspInfo->ErrorID, pRspInfo->ErrorMsg);
}

inline char* CtpTraderDemo::getOrderRef()
{
	TThostFtdcOrderRefType rs;
	int temp = atoi(m_OrderRef);
	sprintf(rs, "%d", ++temp);
	strcpy(m_OrderRef + 12 - strlen(rs), rs);
	return m_OrderRef;
}

void CtpTraderDemo::Run()
{
	const char *arg[] = { "tcp://180.166.103.34:51215","6666","66617019","66617019","1","client_wuroni_1.0.3","RW7QSCRUAJTHHHPS" };
	InitParam(arg);
	int i = 1;
	while (i) {
		printf("-------------------------0.exit--------------------------------\n");
		printf("-------------------------1.ReqUserLogin--------------------------------\n");
		printf("-------------------------2.ReqUserLogout--------------------------------\n");
		printf("-------------------------3.ReqOrderInsert----------------------------\n");
		printf("-------------------------4.ReqOrderAction-----------------------------\n");
		printf("-------------------------5.ReqQryInvestorPosition-----------------------------\n");
		printf("-------------------------6.ReqQryTradingAccount-----------------------------\n");
		scanf("%d", &i);
		switch (i) {
		case 0: printf("0.exit\n"); break;
		case 1: printf("1.ReqUserLogin\n"); ReqUserLogin(); break;
		case 2: printf("2.ReqUserLogout\n"); ReqUserLogout(); break;
		case 3: printf("3.ReqOrderInsert\n"); ReqOrderInsert(); break;
		case 4: printf("4.ReqOrderAction\n"); ReqOrderAction(); break;
		case 5: printf("5.ReqQryInvestorPosition\n"); ReqQryInvestorPosition(); break;
		case 6: printf("6.ReqQryTradingAccount\n"); ReqQryTradingAccount(); break;
		default: printf("Invalid input\n"); break;
		}
	}
	printf("main end\n");
}

void CtpTraderDemo::InitParam(const char * arg[])
{
	strcpy(t_FrontAddr, arg[0]);
	strcpy(m_BrokerID, arg[2]);
	strcpy(m_InvestorID, arg[3]);
	strcpy(m_UserID, arg[4]);
	strcpy(m_PassWord, arg[5]);
	strcpy(m_AppID, arg[6]);
	strcpy(m_AuthCode, arg[7]);

	tRequestID = 0;
	strcpy(m_OrderRef, "000000000000");

	traderApi->RegisterSpi(this);
	traderApi->SubscribePrivateTopic(THOST_TERT_RESTART);
	traderApi->SubscribePublicTopic(THOST_TERT_RESTART);
	traderApi->RegisterFront(t_FrontAddr);
	traderApi->Init();
	printf("CtpTraderDemo::InitParam OK,Version=%s\n", traderApi->GetApiVersion());
}

void CtpTraderDemo::RequestAuth()
{
	CThostFtdcReqAuthenticateField authField;
	memset(&authField, 0, sizeof(authField));
	strcpy(authField.BrokerID, m_BrokerID); //期货公司代码
	strcpy(authField.UserID, m_UserID); //客户账号
	strcpy(authField.AppID, m_AppID); //客户交易终端软件的唯一标识码
	strcpy(authField.AuthCode, m_AuthCode); //与上述标识配套取得的认证码
	int ret=traderApi->ReqAuthenticate(&authField, ++tRequestID);
	printf("ReqAuthenticate:%d\n", ret);
}

void CtpTraderDemo::ReqUserLogin()
{
	RequestAuth();
}

void CtpTraderDemo::ReqUserLogout()
{
	CThostFtdcUserLogoutField a;
	memset(&a, 0, sizeof(CThostFtdcUserLogoutField));
	strcpy(a.BrokerID, m_BrokerID);
	strcpy(a.UserID, m_UserID);
	int ret = traderApi->ReqUserLogout(&a, tRequestID++);
	printf("ReqUserLogout:%d\n",ret);

}

void CtpTraderDemo::ReqOrderInsert()
{
	CThostFtdcInputOrderField ord;
	memset(&ord,0,sizeof(CThostFtdcInputOrderField));
	printf("Input arg:InstrumentID(IF1907) CombOffsetFlag(0|1|2(OPEN|CLOSE|CLOSE_TD)) Direction(0|1(LONG|SHORT) LimitPrice(3800) VolumeTotalOriginal(1)\n");

	char CombOffsetFlag = '0';
	char Direction = '0';
	double LimitPrice = 3800;
	int VolumeTotalOriginal = 1;
	char InstrumentID[] = "IF1908";
	scanf(" %s %c %c %lf %d", InstrumentID,&CombOffsetFlag, &Direction,&LimitPrice,&VolumeTotalOriginal);
	
	strcpy(ord.BrokerID, m_BrokerID);
	//投资者代码
	strcpy(ord.InvestorID, m_InvestorID);
	// 合约代码
	//const char* instrument[] = { "IF1908" };
	//strcpy(ord.InstrumentID, instrument[0]);
	strcpy(ord.InstrumentID, InstrumentID);
	//报单引用
	//sprintf(orderref, "%d", ++OrderRef);
	//strcpy(ord.OrderRef, "1");
	strcpy(ord.OrderRef, getOrderRef());
	// 用户代码
	strcpy(ord.UserID, m_UserID);
	// 报单价格条件
	ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	// 买卖方向
	//ord.Direction = THOST_FTDC_D_Buy;
	ord.Direction = Direction;
	// 组合开平标志
	//strcpy(ord.CombOffsetFlag, "0");
	ord.CombOffsetFlag[0] = CombOffsetFlag;
	// 组合投机套保标志
	strcpy(ord.CombHedgeFlag, "1");
	// 价格
	//ord.LimitPrice = 3826.8;
	ord.LimitPrice = LimitPrice;
	// 数量
	//ord.VolumeTotalOriginal = 1;
	ord.VolumeTotalOriginal = VolumeTotalOriginal;
	// 有效期类型
	ord.TimeCondition = THOST_FTDC_TC_GFD;
	// GTD日期
	strcpy(ord.GTDDate, "");
	// 成交量类型
	ord.VolumeCondition = THOST_FTDC_VC_AV;
	// 最小成交量
	ord.MinVolume = 0;
	// 触发条件
	ord.ContingentCondition = THOST_FTDC_CC_Immediately;
	// 止损价
	ord.StopPrice = 0;
	// 强平原因
	ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	// 自动挂起标志
	ord.IsAutoSuspend = 0;
	

	int ret=traderApi->ReqOrderInsert(&ord, tRequestID++);
	printf("ReqOrderInsert:%d\n",ret);
}

void CtpTraderDemo::ReqOrderAction()
{
	CThostFtdcInputOrderActionField a;
	strcpy(a.BrokerID, m_BrokerID);
	strcpy(a.InvestorID,m_InvestorID);
	strcpy(a.UserID,m_UserID);
	printf("Input arg:OrderSysID(131)\n");
	TThostFtdcOrderSysIDType	OrderSysID;
	scanf(" %s", OrderSysID);
	printf("OrderSysID=%s\n", OrderSysID);
	strcpy(a.OrderSysID, OrderSysID);  //对应要撤报单的OrderSysID
	//strcpy(a.ExchangeID, "SHFE");
	//strcpy(a.InstrumentID, "IF1907");
	a.ActionFlag = THOST_FTDC_AF_Delete;
	int ret=traderApi->ReqOrderAction(&a, tRequestID++);
	printf("ReqOrderAction:%d\n", ret);
}

void CtpTraderDemo::ReqQryInvestorPosition()
{
	CThostFtdcQryInvestorPositionField ap;
	memset(&ap, 0, sizeof(CThostFtdcQryInvestorPositionField));
	strcpy(ap.BrokerID, m_BrokerID);
	strcpy(ap.InvestorID, m_InvestorID);
	int ret =traderApi->ReqQryInvestorPosition(&ap, tRequestID++);
	printf("ReqQryInvestorPosition:%d\n", ret);
}

void CtpTraderDemo::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField af;
	memset(&af, 0, sizeof(af));
	strcpy(af.BrokerID, m_BrokerID);
	strcpy(af.InvestorID, m_InvestorID);
	strcpy(af.CurrencyID, "CNY");
	int ret = traderApi->ReqQryTradingAccount(&af, tRequestID++);
	printf("ReqQryTradingAccount:%d\n",ret);
}

void CtpTraderDemo::OnFrontConnected()
{
	printf("TraderOnFrontConnected\n");
}

void CtpTraderDemo::OnFrontDisconnected(int nReason)
{
	printf("TraderOnFrontDisconnected:nReason=%d\n", nReason);
}

void CtpTraderDemo::OnHeartBeatWarning(int nTimeLapse)
{
}

void CtpTraderDemo::OnRspAuthenticate(CThostFtdcRspAuthenticateField * pRspAuthenticateField, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	PrintRspInfo("OnRspAuthenticate", pRspInfo, nRequestID, bIsLast);
	if (pRspInfo->ErrorID == 0) {
		CThostFtdcReqUserLoginField traderLoginField;
		memset(&traderLoginField, 0, sizeof(traderLoginField));
		strcpy(traderLoginField.BrokerID, m_BrokerID); //期货公司代码
		strcpy(traderLoginField.UserID, m_UserID); //客户账号
		strcpy(traderLoginField.Password, m_PassWord); //密码
		printf("m_BrokerID=%s,m_UserID=%s,m_PassWord=%s\n", m_BrokerID, m_UserID, m_PassWord);
		int ret = traderApi->ReqUserLogin(&traderLoginField, ++tRequestID); //接口内部会采集客户交易终端信息（直连模式）
		printf("ReqUserLogin:%d\n", ret);
	}
}

void CtpTraderDemo::OnRspUserLogin(CThostFtdcRspUserLoginField * pRspUserLogin, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	PrintRspInfo("OnRspUserLogin", pRspInfo, nRequestID, bIsLast);
	if(pRspInfo->ErrorID==0){
	CThostFtdcSettlementInfoConfirmField t;
	memset(&t,0,sizeof(CThostFtdcSettlementInfoConfirmField));
	strcpy(t.BrokerID, m_BrokerID);
	strcpy(t.InvestorID, m_InvestorID);
	int ret = traderApi->ReqSettlementInfoConfirm(&t, ++tRequestID);
	printf("ReqSettlementInfoConfirm:%d\n",ret);
	}

}

void CtpTraderDemo::OnRspUserLogout(CThostFtdcUserLogoutField * pUserLogout, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	PrintRspInfo("OnRspUserLogout", pRspInfo, nRequestID, bIsLast);
}

void CtpTraderDemo::OnRspOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	PrintRspInfo("OnRspOrderInsert", pRspInfo, nRequestID, bIsLast);
	printf("pInputOrder:OrderRef=%s\n", pInputOrder->OrderRef);
}

void CtpTraderDemo::OnRspOrderAction(CThostFtdcInputOrderActionField * pInputOrderAction, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	PrintRspInfo("OnRspOrderAction", pRspInfo, nRequestID, bIsLast);
}

void CtpTraderDemo::OnRtnOrder(CThostFtdcOrderField * pOrder)
{
	printf("OnRtnOrder:OrderRef=%s,", pOrder->OrderRef);
	printf("OrderLocalID=%s,",pOrder->OrderLocalID);
	printf("OrderStatus=%c,", pOrder->OrderStatus);
	printf("OrderSysID=%s,", pOrder->OrderSysID);
	printf("VolumeTotalOriginal=%d,", pOrder->VolumeTotalOriginal);
	printf("VolumeTotal=%d,", pOrder->VolumeTotal);
	printf("VolumeTraded=%d", pOrder->VolumeTraded);
	printf("InstrumentID=%s,", pOrder->InstrumentID);
	printf("CombOffsetFlag=%c,", pOrder->CombOffsetFlag[0]);
	printf("Direction=%c,", pOrder->Direction);
	printf("LimitPrice=%lf,", pOrder->LimitPrice);
	printf("CombHedgeFlag=%s,",pOrder->CombHedgeFlag);
	printf("ExchangeID=%s,InvestorID=%s,OrderPriceType=%c\n",pOrder->ExchangeID,pOrder->InvestorID,pOrder->OrderPriceType);
}

void CtpTraderDemo::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
	printf("OnRtnTrade:TradeID=%s,", pTrade->TradeID);
	printf("OrderSysID=%s,", pTrade->OrderSysID);
	printf("OrderLocalID=%s,", pTrade->OrderLocalID);
	printf("InstrumentID=%s,", pTrade->InstrumentID);
	printf("OffsetFlag=%c,", pTrade->OffsetFlag);
	printf("Direction=%c,", pTrade->Direction);
	printf("Price=%lf,", pTrade->Price);
	printf("Volume=%d\n", pTrade->Volume);
}

void CtpTraderDemo::OnErrRtnOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo)
{
	printf("OnErrRtnOrderInsert\n");
	if(pRspInfo) printf("OnErrRtnOrderInsert:ErrorID=%d,ErrorMsg=%s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	if (pInputOrder) { 
		printf("OnErrRtnOrderInsert:OrderRef=%s,", pInputOrder->OrderRef);
		printf("VolumeTotalOriginal=%d,", pInputOrder->VolumeTotalOriginal);
		printf("InstrumentID=%s,", pInputOrder->InstrumentID);
		printf("CombOffsetFlag=%c,", pInputOrder->CombOffsetFlag[0]);
		printf("Direction=%c,", pInputOrder->Direction);
		printf("LimitPrice=%lf,", pInputOrder->LimitPrice);
		printf("CombHedgeFlag=%s,", pInputOrder->CombHedgeFlag);
		printf("ExchangeID=%s,InvestorID=%s,OrderPriceType=%c\n", pInputOrder->ExchangeID, pInputOrder->InvestorID, pInputOrder->OrderPriceType);
		}
}

void CtpTraderDemo::OnErrRtnOrderAction(CThostFtdcOrderActionField * pOrderAction, CThostFtdcRspInfoField * pRspInfo)
{
	if (pRspInfo) printf("OnErrRtnOrderAction:ErrorID=%d,ErrorMsg=%s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	if (pOrderAction) printf("pOrderAction:OrderSysID=%s\n",pOrderAction->OrderSysID);
}

void CtpTraderDemo::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField * pSettlementInfoConfirm, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	PrintRspInfo("OnRspSettlementInfoConfirm", pRspInfo, nRequestID, bIsLast);
}

void CtpTraderDemo::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField * pInvestorPosition, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	PrintRspInfo("OnRspQryInvestorPosition", pRspInfo, nRequestID, bIsLast);
	if (pInvestorPosition) {
		printf("InstrumentID=%s,",pInvestorPosition->InstrumentID);
		printf("PosiDirection=%c,", pInvestorPosition->PosiDirection);
		printf("Position=%d\n", pInvestorPosition->Position);
	}
}

void CtpTraderDemo::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	PrintRspInfo("OnRspQrySecAgentTradingAccount", pRspInfo, nRequestID, bIsLast);
	if (pTradingAccount) {
		printf("Balance=%10.2f,", pTradingAccount->Balance);
		printf("Available=%10.2f,", pTradingAccount->Available);
		printf("CurrMargin=%10.2f\n", pTradingAccount->CurrMargin);
	}
}
