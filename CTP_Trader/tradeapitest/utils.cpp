#include "utils.h"

// Initialization of the trader API, including register, subscription, authentication and login.
void pTraderInit(CThostFtdcTraderApi *pTraderApi, CThostFtdcReqUserLoginField *userLoginField, 
    CThostFtdcReqAuthenticateField *userAuthField) {
    strcpy(userLoginField->BrokerID, "9999");
    strcpy(userLoginField->UserID, "206045");
    strcpy(userLoginField->Password, "Thzss200718!");

    strcpy(userAuthField->AppID, "simnow_client_test");
    strcpy(userAuthField->UserID, "206045");
    strcpy(userAuthField->AuthCode, "0000000000000000");
    strcpy(userAuthField->BrokerID, "9999");

    // 订阅私有流
    // TERT_RESTART:从本交易日开始重传
    // TERT_RESUME:从上次收到的续传
    // TERT_QUICK:只传送登录后私有流的内容
	pTraderApi->SubscribePrivateTopic(THOST_TERT_RESUME);
    // 订阅公共流
    // TERT_RESTART:从本交易日开始重传
    // TERT_RESUME:从上次收到的续传
    // TERT_QUICK:只传送登录后公共流的内容
	pTraderApi->SubscribePublicTopic(THOST_TERT_RESUME);

    // Set the server address.
    pTraderApi->RegisterFront((char *)"tcp://180.168.146.187:10201");
    // pTraderApi->RegisterFront((char *)"tcp://180.168.146.187:10202");

    // Link to the trading system.
    pTraderApi->Init();
	sem_wait(&sem);

    pTraderApi->ReqAuthenticate(userAuthField, requestID++);
    sem_wait(&sem);

    pTraderApi->ReqUserLogin(userLoginField, requestID++);
    sem_wait(&sem);
}

// User logout and record the user information.
void pTraderLogout(CThostFtdcTraderApi *pTraderApi, CThostFtdcReqUserLoginField *userLoginField, 
    CThostFtdcUserLogoutField *userLogoutField) {
    // Copy logout information.
	strcpy(userLogoutField->BrokerID,userLoginField->BrokerID);
	strcpy(userLogoutField->UserID, userLoginField->UserID);
	pTraderApi->ReqUserLogout(userLogoutField, requestID++);
	sem_wait(&sem);
}

// Query Instrument ID.
void pTraderQryInstrument(CThostFtdcTraderApi *pTraderApi) {
    CThostFtdcQryInstrumentField requestData;
    memset(&requestData, 0, sizeof(requestData));
    // strcpy(requestData.InstrumentID, "bc2211");
    // strcpy(requestData.ExchangeID, "SHFE");
    // strcpy(requestData.ExchangeID, "CFFEX");

    pTraderApi->ReqQryInstrument(&requestData, requestID++);
    sem_wait(&sem);
}


// Query trading account.
void pTraderQryTradingAccount(CThostFtdcTraderApi *pTraderApi) {
    CThostFtdcQryTradingAccountField requestData;
    memset(&requestData, 0, sizeof(requestData));
    strcpy(requestData.BrokerID, "9999");
    strcpy(requestData.InvestorID, "206045");
    pTraderApi->ReqQryTradingAccount(&requestData, requestID++);
    sem_wait(&sem);
}

// Query investor position.
void pTraderInvestorPosition(CThostFtdcTraderApi *pTraderApi, char *InstrumentID) {
    CThostFtdcQryInvestorPositionField requestData;
    memset(&requestData, 0, sizeof(requestData));
    // strcpy(requestData.BrokerID, "9999");
    // strcpy(requestData.InvestorID, "206045");
    strcpy(requestData.InstrumentID, InstrumentID);
    pTraderApi->ReqQryInvestorPosition(&requestData, requestID++);
    sem_wait(&sem);
}

// Insert trading order.
void pTraderOrderInsert(CThostFtdcTraderApi *pTraderApi, char *InstrumentID, int order_type) {
    // Use order_type to determine open/close/buy/sell.
    char openclose_indicator;
    char buysell_indicator;

    if (order_type == 0) {
        openclose_indicator = '0';
        buysell_indicator = '0';
    }
    else if (order_type == 1) {
        openclose_indicator = '0';
        buysell_indicator = '1';
    }
    if (order_type == 2) {
        openclose_indicator = '1';
        buysell_indicator = '0';
    }
    if (order_type == 3) {
        openclose_indicator = '1';
        buysell_indicator = '1';
    }



    CThostFtdcInputOrderField requestData;
    memset(&requestData,0,sizeof(requestData));
    strcpy(requestData.BrokerID,"9999");
    strcpy(requestData.InvestorID,"206045");
    strcpy(requestData.InstrumentID, InstrumentID);
    strcpy(requestData.OrderRef,"");
    strcpy(requestData.UserID,"206045");
    //////////////////////////////////////////////////////////
    ///报单价格条件 TThostFtdcOrderPriceTypeType char
    //// THOST_FTDC_OPT_AnyPrice '1' 任意价
    //// THOST_FTDC_OPT_LimitPrice '2' 限价
    //// THOST_FTDC_OPT_BestPrice '3' 最优价
    //// THOST_FTDC_OPT_LastPrice '4' 最新价
    //// THOST_FTDC_OPT_LastPricePlusOneTicks '5' 最新价浮动上浮1个ticks
    //// THOST_FTDC_OPT_LastPricePlusTwoTicks '6' 最新价浮动上浮2个ticks
    //// THOST_FTDC_OPT_LastPricePlusThreeTicks '7' 最新价浮动上浮3个ticks
    //// THOST_FTDC_OPT_AskPrice1 '8' 卖一价
    //// THOST_FTDC_OPT_AskPrice1PlusOneTicks '9' 卖一价浮动上浮1个ticks
    //// THOST_FTDC_OPT_AskPrice1PlusTwoTicks 'A' 卖一价浮动上浮2个ticks
    //// THOST_FTDC_OPT_AskPrice1PlusThreeTicks 'B' 卖一价浮动上浮3个ticks
    //// THOST_FTDC_OPT_BidPrice1 'C' 买一价
    //// THOST_FTDC_OPT_BidPrice1PlusOneTicks 'D' 买一价浮动上浮1个ticks
    //// THOST_FTDC_OPT_BidPrice1PlusTwoTicks 'E' 买一价浮动上浮2个ticks
    //// THOST_FTDC_OPT_BidPrice1PlusThreeTicks 'F' 买一价浮动上浮3个ticks
    requestData.OrderPriceType = '2';
    //////////////////////////////////////////////////////////
    ///买卖方向 TThostFtdcDirectionType char
    //// THOST_FTDC_D_Buy '0' 买
    //// THOST_FTDC_D_Sell '1' 卖
    requestData.Direction = buysell_indicator;
    //////////////////////////////////////////////////////////
    ///组合开平标志 TThostFtdcCombOffsetFlagType char[5]
    //// THOST_FTDC_OF_Open '0' 开仓
    //// THOST_FTDC_OF_Close '1' 平仓
    //// THOST_FTDC_OF_ForceClose '2' 强平
    //// THOST_FTDC_OF_CloseToday '3' 平今
    //// THOST_FTDC_OF_CloseYesterday '4' 平昨
    //// THOST_FTDC_OF_ForceOff '5' 强减
    //// THOST_FTDC_OF_LocalForceClose '6' 本地强平
    strcpy(requestData.CombOffsetFlag, &openclose_indicator);
    //////////////////////////////////////////////////////////
    ///组合投机套保标志 TThostFtdcCombHedgeFlagType char[5]
    //// THOST_FTDC_HF_Speculation '1' 投机
    //// THOST_FTDC_HF_Arbitrage '2' 套利
    //// THOST_FTDC_HF_Hedge '3' 套保
    strcpy(requestData.CombHedgeFlag, "1");
    //////////////////////////////////////////////////////////
    ///价格 TThostFtdcPriceType double
    requestData.LimitPrice = 15;
    //////////////////////////////////////////////////////////
    ///数量 TThostFtdcVolumeType int
    requestData.VolumeTotalOriginal = 1;
    //////////////////////////////////////////////////////////
    ///有效期类型 TThostFtdcTimeConditionType char
    //// THOST_FTDC_TC_IOC '1' 立即完成，否则撤销
    //// THOST_FTDC_TC_GFS '2' 本节有效
    //// THOST_FTDC_TC_GFD '3' 当日有效
    //// THOST_FTDC_TC_GTD '4' 指定日期前有效
    //// THOST_FTDC_TC_GTC '5' 撤销前有效
    //// THOST_FTDC_TC_GFA '6' 集合竞价有效
    requestData.TimeCondition = '1';
    //////////////////////////////////////////////////////////
    ///GTD日期 TThostFtdcDateType char[9]
    strcpy(requestData.GTDDate,"");
    //////////////////////////////////////////////////////////
    ///成交量类型 TThostFtdcVolumeConditionType char
    //// THOST_FTDC_VC_AV '1' 任何数量
    //// THOST_FTDC_VC_MV '2' 最小数量
    //// THOST_FTDC_VC_CV '3' 全部数量
    requestData.VolumeCondition = '1';
    //////////////////////////////////////////////////////////
    ///最小成交量 TThostFtdcVolumeType int
    requestData.MinVolume = 1;
    //////////////////////////////////////////////////////////
    ///触发条件 TThostFtdcContingentConditionType char
    //// THOST_FTDC_CC_Immediately '1' 立即
    //// THOST_FTDC_CC_Touch '2' 止损
    //// THOST_FTDC_CC_TouchProfit '3' 止赢
    //// THOST_FTDC_CC_ParkedOrder '4' 预埋单
    //// THOST_FTDC_CC_LastPriceGreaterThanStopPrice '5' 最新价大于条件价
    //// THOST_FTDC_CC_LastPriceGreaterEqualStopPrice '6' 最新价大于等于条件价
    //// THOST_FTDC_CC_LastPriceLesserThanStopPrice '7' 最新价小于条件价
    //// THOST_FTDC_CC_LastPriceLesserEqualStopPrice '8' 最新价小于等于条件价
    //// THOST_FTDC_CC_AskPriceGreaterThanStopPrice '9' 卖一价大于条件价
    //// THOST_FTDC_CC_AskPriceGreaterEqualStopPrice 'A' 卖一价大于等于条件价
    //// THOST_FTDC_CC_AskPriceLesserThanStopPrice 'B' 卖一价小于条件价
    //// THOST_FTDC_CC_AskPriceLesserEqualStopPrice 'C' 卖一价小于等于条件价
    //// THOST_FTDC_CC_BidPriceGreaterThanStopPrice 'D' 买一价大于条件价
    //// THOST_FTDC_CC_BidPriceGreaterEqualStopPrice 'E' 买一价大于等于条件价
    //// THOST_FTDC_CC_BidPriceLesserThanStopPrice 'F' 买一价小于条件价
    //// THOST_FTDC_CC_BidPriceLesserEqualStopPrice 'H' 买一价小于等于条件价
    requestData.ContingentCondition = '1';
    //////////////////////////////////////////////////////////
    ///止损价 TThostFtdcPriceType double
    requestData.StopPrice = 0;
    //////////////////////////////////////////////////////////
    ///强平原因 TThostFtdcForceCloseReasonType char
    //// THOST_FTDC_FCC_NotForceClose '0' 非强平
    //// THOST_FTDC_FCC_LackDeposit '1' 资金不足
    //// THOST_FTDC_FCC_ClientOverPositionLimit '2' 客户超仓
    //// THOST_FTDC_FCC_MemberOverPositionLimit '3' 会员超仓
    //// THOST_FTDC_FCC_NotMultiple '4' 持仓非整数倍
    //// THOST_FTDC_FCC_Violation '5' 违规
    //// THOST_FTDC_FCC_Other '6' 其它
    //// THOST_FTDC_FCC_PersonDeliv '7' 自然人临近交割
    requestData.ForceCloseReason = '0';
    //////////////////////////////////////////////////////////
    ///自动挂起标志 TThostFtdcBoolType int
    requestData.IsAutoSuspend = 0;
    //////////////////////////////////////////////////////////
    ///业务单元 TThostFtdcBusinessUnitType char[21]
    strcpy(requestData.BusinessUnit,"");
    //////////////////////////////////////////////////////////
    ///请求编号 TThostFtdcRequestIDType int
    requestData.RequestID = 0;
    //////////////////////////////////////////////////////////
    ///用户强评标志 TThostFtdcBoolType int
    requestData.UserForceClose = 0;
    //////////////////////////////////////////////////////////
    ///互换单标志 TThostFtdcBoolType int
    requestData.IsSwapOrder = 0;
    //////////////////////////////////////////////////////////


    // 调用API,并等待响应函数返回
    int result = pTraderApi->ReqOrderInsert(&requestData, requestID++);
    sem_wait(&sem);
}

// Query trading account.
void pTraderSettmentInfoConfirm(CThostFtdcTraderApi *pTraderApi) {
    CThostFtdcSettlementInfoConfirmField requestData;
    memset(&requestData, 0, sizeof(requestData));
    strcpy(requestData.BrokerID, "9999");
    strcpy(requestData.InvestorID, "206045");
    pTraderApi->ReqSettlementInfoConfirm(&requestData, requestID++);
    sem_wait(&sem);
}

// Query all the orders.
void pTraderQryOrder(CThostFtdcTraderApi *pTraderApi) {
    CThostFtdcQryOrderField requestData;
    memset(&requestData,0,sizeof(requestData));
    strcpy(requestData.BrokerID, "9999");
    strcpy(requestData.InvestorID, "206045");
    strcpy(requestData.InstrumentID,"au2304P400");
    strcpy(requestData.ExchangeID,"");
    strcpy(requestData.OrderSysID,"");
    strcpy(requestData.InsertTimeStart,"");
    strcpy(requestData.InsertTimeEnd,"");

    pTraderApi->ReqQryOrder(&requestData, requestID++);
    sem_wait(&sem);
}

// Delete an order.
void pTraderOrderAction(CThostFtdcTraderApi *pTraderApi) {
    CThostFtdcInputOrderActionField requestData;
    strcpy(requestData.BrokerID, "9999");
    strcpy(requestData.InvestorID, "206045");
    strcpy(requestData.UserID, "206045");

    strcpy(requestData.InstrumentID,"cu2306");

    requestData.OrderActionRef = 0;
    strcpy(requestData.OrderRef,"000000000001");
    requestData.FrontID = 3;
    requestData.SessionID = 266981291;
    strcpy(requestData.ExchangeID, "SHFE");
    strcpy(requestData.OrderSysID,"      587740");
    //////////////////////////////////////////////////////////
    ///操作标志 TThostFtdcActionFlagType char
    //// THOST_FTDC_AF_Delete '0' 删除
    //// THOST_FTDC_AF_Modify '3' 修改
    requestData.ActionFlag = '0';
    // requestData.LimitPrice = 0;
    // requestData.VolumeChange = 0;

    pTraderApi->ReqOrderAction(&requestData, requestID++);
    sem_wait(&sem);
}

// Query market data
void pTraderQryDepthMarketData(CThostFtdcTraderApi *pTraderApi) {
    CThostFtdcQryDepthMarketDataField  requestData;
    memset(&requestData,0,sizeof(requestData));

    strcpy(requestData.InstrumentID,"");
    pTraderApi->ReqQryDepthMarketData(&requestData, requestID++);
    sem_wait(&sem);
}

// Option self close insert.
void pTraderReqOptionSelfCloseInsert(CThostFtdcTraderApi *pTraderApi) {
    CThostFtdcInputOptionSelfCloseField requestData;
    memset(&requestData,0,sizeof(requestData));

    strcpy(requestData.BrokerID, "9999");
    strcpy(requestData.InvestorID, "206045");
    strcpy(requestData.InstrumentID,"sc2301C590");
    strcpy(requestData.UserID, "206045");
    strcpy(requestData.ExchangeID, "SHFE");
    strcpy(requestData.CurrencyID, "CNY");


    requestData.Volume = 1;
    requestData.HedgeFlag = THOST_FTDC_HF_Speculation;
    requestData.OptSelfCloseFlag = THOST_FTDC_OSCF_CloseSelfOptionPosition;

    pTraderApi->ReqOptionSelfCloseInsert(&requestData, requestID++);
    sem_wait(&sem);
}

// Qry inserted self close.
void pTraderQryOptionSelfClose(CThostFtdcTraderApi *pTraderApi) {
    CThostFtdcQryOptionSelfCloseField requestData;

    strcpy(requestData.BrokerID, "9999");
    strcpy(requestData.InvestorID, "206045");
    // strcpy(requestData.InstrumentID,"sc2301C590");
    strcpy(requestData.ExchangeID, "SHFE");

    pTraderApi->ReqQryOptionSelfClose(&requestData, requestID++);
    sem_wait(&sem);
}

// Cancel the query of self close.
void pTraderOptionSelfCloseAction(CThostFtdcTraderApi *pTraderApi) {
    CThostFtdcInputOptionSelfCloseActionField requestData;

    strcpy(requestData.BrokerID, "9999");
    strcpy(requestData.InvestorID, "206045");
    strcpy(requestData.InstrumentID,"sc2301C590");

    pTraderApi->ReqOptionSelfCloseAction(&requestData, requestID++);
    sem_wait(&sem);
}

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
void pMdSubscribe(CThostFtdcMdApi *pMdApi, char **pInstrumentID, sem_t *sem) {
    pMdApi->SubscribeMarketData(pInstrumentID, 2);
    sem_wait(sem);
}




