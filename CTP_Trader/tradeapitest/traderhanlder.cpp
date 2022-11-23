#include "traderhandler.h"

CTraderHandler::CTraderHandler(CThostFtdcTraderApi *pTraderApi) : m_pTraderApi(pTraderApi) {
    printf("CTraderHandler:called.\n");
}

CTraderHandler::~CTraderHandler() {

    for(map<string, CThostFtdcInvestorPositionField *>::iterator it = this->positionLongData.begin(); it != this->positionLongData.end(); ++it) {
        delete it->second; 
    }

    for(map<string, CThostFtdcInvestorPositionField *>::iterator it = this->positionShortData.begin(); it != this->positionShortData.end(); ++it) {
        delete it->second; 
    }

}

void CTraderHandler::OnFrontConnected() {
    static int i = 0;
    // 在登出后系统会重新调用OnFrontConnected，这里简单判断并忽略第1次之后的所有调用。
    if (i++ == 0) {
        printf("OnFrontConnected:called.\n");
        sem_post(&sem);
    }
}

void CTraderHandler::OnFrontDisconnected(int nReason)
{
    // 当发生这个情况后，API会自动重新连接，客户端可不做处理
    printf("OnFrontDisconnected.\n");
}

void CTraderHandler::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
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

void CTraderHandler::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {	
    printf("OnReqUserLogout:called\n");
    if (pRspInfo->ErrorID == 0) {
        printf("Logout Success!\n");
        sem_post(&sem);
    }else{
        printf("Logout Failure!\n");
    }
}

void CTraderHandler::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)  {
    printf("OnRspAuthenticate:called\n");
    if (pRspInfo->ErrorID == 0) {
        printf("Authentication Success!\n");			
        sem_post(&sem);
    }
    else {
        printf("Authentication Failure!\n");
    }
}

void CTraderHandler::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    printf("OnRspError:called\n");
    printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID,
    pRspInfo->ErrorMsg);
    printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
    // 客户端需进行错误处理
    //{客户端的错误处理}
}

void CTraderHandler::OnRspQryInstrument(CThostFtdcInstrumentField * pInstrument, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast) {
    printf("OnRspQryInstrument:called...\n");

    // 进程返回结果检查
    if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
        // typedef int TThostFtdcErrorIDType;
        // typedef char TThostFtdcErrorMsgType[81];
        char ErrorMsg[243];
        gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
        printf("OnRspQryInstrument():Error:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
        exit(-1);
    }

    // 如果有返回结果读取返回信息
    if ( pInstrument != NULL ) {
        // 读取返回信息,并做编码转化
        ///合约代码 TThostFtdcInstrumentIDType char[31]
        char InstrumentID[93];
        gbk2utf8(pInstrument->InstrumentID,InstrumentID,sizeof(InstrumentID));
        ///交易所代码 TThostFtdcExchangeIDType char[9]
        char ExchangeID[27];
        gbk2utf8(pInstrument->ExchangeID,ExchangeID,sizeof(ExchangeID));
        ///合约名称 TThostFtdcInstrumentNameType char[21]
        char InstrumentName[63];
        gbk2utf8(pInstrument->InstrumentName,InstrumentName,sizeof(InstrumentName));
        ///合约在交易所的代码 TThostFtdcExchangeInstIDType char[31]
        char ExchangeInstID[93];
        gbk2utf8(pInstrument->ExchangeInstID,ExchangeInstID,sizeof(ExchangeInstID));
        ///产品代码 TThostFtdcInstrumentIDType char[31]
        char ProductID[93];
        gbk2utf8(pInstrument->ProductID,ProductID,sizeof(ProductID));
        ///产品类型 TThostFtdcProductClassType char
        //// THOST_FTDC_PC_Futures '1' 期货
        //// THOST_FTDC_PC_Options '2' 期权
        //// THOST_FTDC_PC_Combination '3' 组合
        //// THOST_FTDC_PC_Spot '4' 即期
        //// THOST_FTDC_PC_EFP '5' 期转现
        char ProductClass = pInstrument->ProductClass;
        ///交割年份 TThostFtdcYearType int
        int DeliveryYear = pInstrument->DeliveryYear;
        ///交割月 TThostFtdcMonthType int
        int DeliveryMonth = pInstrument->DeliveryMonth;
        ///市价单最大下单量 TThostFtdcVolumeType int
        int MaxMarketOrderVolume = pInstrument->MaxMarketOrderVolume;
        ///市价单最小下单量 TThostFtdcVolumeType int
        int MinMarketOrderVolume = pInstrument->MinMarketOrderVolume;
        ///限价单最大下单量 TThostFtdcVolumeType int
        int MaxLimitOrderVolume = pInstrument->MaxLimitOrderVolume;
        ///限价单最小下单量 TThostFtdcVolumeType int
        int MinLimitOrderVolume = pInstrument->MinLimitOrderVolume;
        ///合约数量乘数 TThostFtdcVolumeMultipleType int
        int VolumeMultiple = pInstrument->VolumeMultiple;
        ///最小变动价位 TThostFtdcPriceType double
        double PriceTick = pInstrument->PriceTick;
        ///创建日 TThostFtdcDateType char[9]
        char CreateDate[27];
        gbk2utf8(pInstrument->CreateDate,CreateDate,sizeof(CreateDate));
        ///上市日 TThostFtdcDateType char[9]
        char OpenDate[27];
        gbk2utf8(pInstrument->OpenDate,OpenDate,sizeof(OpenDate));
        ///到期日 TThostFtdcDateType char[9]
        char ExpireDate[27];
        gbk2utf8(pInstrument->ExpireDate,ExpireDate,sizeof(ExpireDate));
        ///开始交割日 TThostFtdcDateType char[9]
        char StartDelivDate[27];
        gbk2utf8(pInstrument->StartDelivDate,StartDelivDate,sizeof(StartDelivDate));
        ///结束交割日 TThostFtdcDateType char[9]
        char EndDelivDate[27];
        gbk2utf8(pInstrument->EndDelivDate,EndDelivDate,sizeof(EndDelivDate));
        ///合约生命周期状态 TThostFtdcInstLifePhaseType char
        //// THOST_FTDC_IP_NotStart '0' 未上市
        //// THOST_FTDC_IP_Started '1' 上市
        //// THOST_FTDC_IP_Pause '2' 停牌
        //// THOST_FTDC_IP_Expired '3' 到期
        char InstLifePhase = pInstrument->InstLifePhase;
        ///当前是否交易 TThostFtdcBoolType int
        int IsTrading = pInstrument->IsTrading;
        ///持仓类型 TThostFtdcPositionTypeType char
        //// THOST_FTDC_PT_Net '1' 净持仓
        //// THOST_FTDC_PT_Gross '2' 综合持仓
        char PositionType = pInstrument->PositionType;
        ///持仓日期类型 TThostFtdcPositionDateTypeType char
        //// THOST_FTDC_PDT_UseHistory '1' 使用历史持仓
        //// THOST_FTDC_PDT_NoUseHistory '2' 不使用历史持仓
        char PositionDateType = pInstrument->PositionDateType;
        ///多头保证金率 TThostFtdcRatioType double
        double LongMarginRatio = pInstrument->LongMarginRatio;
        ///空头保证金率 TThostFtdcRatioType double
        double ShortMarginRatio = pInstrument->ShortMarginRatio;
        ///是否使用大额单边保证金算法 TThostFtdcMaxMarginSideAlgorithmType char
        //// THOST_FTDC_MMSA_NO '0' 不使用大额单边保证金算法
        //// THOST_FTDC_MMSA_YES '1' 使用大额单边保证金算法
        char MaxMarginSideAlgorithm = pInstrument->MaxMarginSideAlgorithm;
        // 1 for buy, 2 for sell.
        char OptionsType = pInstrument->OptionsType;
        double StrikePrice = pInstrument->StrikePrice;

        printf("InstrumentID=%s, InstrumentName=%s DeliverYear=%i DeliveryMonth=%i OptionsType=%c ProductClass=%c ExchangeID=%s \n", 
            InstrumentID, InstrumentName, DeliveryYear, DeliveryMonth, OptionsType, ProductClass, ExchangeID);
    }

    // 如果响应函数已经返回最后一条信息
    if (bIsLast) {
        // 通知主过程，响应函数将结束
        sem_post(&sem);
    }
}

void CTraderHandler::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    printf("OnRspQryTradingAccount:called...\n");
    if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
        char ErrorMsg[243];
        gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
        printf("OnRspQryTradingAccount:Error:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
        exit(-1);
    }

    if (pTradingAccount != NULL) {
        char BrokerID[11];
        gbk2utf8(pTradingAccount->BrokerID, BrokerID, sizeof(BrokerID));
        char AccountID[13];
        gbk2utf8(pTradingAccount->AccountID, AccountID, sizeof(AccountID));
        double Available;
        Available = pTradingAccount->Available;

        printf("BrokerID=%s, AccountID=%s, Available=%f\n", BrokerID, AccountID, Available);
    }
    

    if (bIsLast) {
        sem_post(&sem);
    }
}

void CTraderHandler::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    printf("OnRspQryInvestorPosition:called...\n");
    if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
        char ErrorMsg[243];
        gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
        printf("OnRspQryInvestorPosition:Error:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
        exit(-1);
    }

    if (pInvestorPosition != NULL) {
        char InstrumentID[81];
        gbk2utf8(pInvestorPosition->InstrumentID, InstrumentID, sizeof(InstrumentID));
        int	Position;
        Position = pInvestorPosition->Position;
        double PositionCost;
        PositionCost = pInvestorPosition->PositionCost;
        double CloseProfit;
        CloseProfit = pInvestorPosition->CloseProfit;
        char PosiDirection;
        PosiDirection = pInvestorPosition->PosiDirection;

        printf("InstrumentID=%s, Position=%i, PositionCost=%f, CloseProfit=%f PosiDirection=%c\n", 
            InstrumentID, Position, PositionCost, CloseProfit, PosiDirection);

        
        string InsturmentID((*pInvestorPosition).InstrumentID);
        if (PosiDirection == '2') {
            if (this->positionLongData.count(InsturmentID) > 0) {
                delete this->positionLongData[InsturmentID];
            }
            this->positionLongData[InsturmentID] = new CThostFtdcInvestorPositionField (*pInvestorPosition);
        }
        if (PosiDirection == '3') {
            if (this->positionShortData.count(InsturmentID) > 0) {
                delete this->positionShortData[InsturmentID];
            }
            this->positionShortData[InsturmentID] = new CThostFtdcInvestorPositionField (*pInvestorPosition);
        }
        
    }
    
    if (bIsLast) {
        sem_post(&sem);
    } 
}

void CTraderHandler::OnRspQryOrder(CThostFtdcOrderField * pOrder, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast) {
    printf("OnRspQryOrder():called...\n");

    // 进程返回结果检查
    if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
        // typedef int TThostFtdcErrorIDType;
        // typedef char TThostFtdcErrorMsgType[81];
        char ErrorMsg[243];
        gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
        printf("OnRspQryOrder():Error:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
    }

    // 如果有返回结果读取返回信息
    if ( pOrder != NULL ) {
        // 读取返回信息,并做编码转化
        ///经纪公司代码 TThostFtdcBrokerIDType char[11]
        char BrokerID[33];
        gbk2utf8(pOrder->BrokerID,BrokerID,sizeof(BrokerID));
        ///投资者代码 TThostFtdcInvestorIDType char[13]
        char InvestorID[39];
        gbk2utf8(pOrder->InvestorID,InvestorID,sizeof(InvestorID));
        ///合约代码 TThostFtdcInstrumentIDType char[31]
        char InstrumentID[93];
        gbk2utf8(pOrder->InstrumentID,InstrumentID,sizeof(InstrumentID));
        ///报单引用 TThostFtdcOrderRefType char[13]
        char OrderRef[39];
        gbk2utf8(pOrder->OrderRef,OrderRef,sizeof(OrderRef));
        ///用户代码 TThostFtdcUserIDType char[16]
        char UserID[48];
        gbk2utf8(pOrder->UserID,UserID,sizeof(UserID));
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
        char OrderPriceType = pOrder->OrderPriceType;
        ///买卖方向 TThostFtdcDirectionType char
        //// THOST_FTDC_D_Buy '0' 买
        //// THOST_FTDC_D_Sell '1' 卖
        char Direction = pOrder->Direction;
        ///组合开平标志 TThostFtdcCombOffsetFlagType char[5]
        char CombOffsetFlag[15];
        gbk2utf8(pOrder->CombOffsetFlag,CombOffsetFlag,sizeof(CombOffsetFlag));
        ///组合投机套保标志 TThostFtdcCombHedgeFlagType char[5]
        char CombHedgeFlag[15];
        gbk2utf8(pOrder->CombHedgeFlag,CombHedgeFlag,sizeof(CombHedgeFlag));
        ///价格 TThostFtdcPriceType double
        double LimitPrice = pOrder->LimitPrice;
        ///数量 TThostFtdcVolumeType int
        int VolumeTotalOriginal = pOrder->VolumeTotalOriginal;
        ///有效期类型 TThostFtdcTimeConditionType char
        //// THOST_FTDC_TC_IOC '1' 立即完成，否则撤销
        //// THOST_FTDC_TC_GFS '2' 本节有效
        //// THOST_FTDC_TC_GFD '3' 当日有效
        //// THOST_FTDC_TC_GTD '4' 指定日期前有效
        //// THOST_FTDC_TC_GTC '5' 撤销前有效
        //// THOST_FTDC_TC_GFA '6' 集合竞价有效
        char TimeCondition = pOrder->TimeCondition;
        ///GTD日期 TThostFtdcDateType char[9]
        char GTDDate[27];
        gbk2utf8(pOrder->GTDDate,GTDDate,sizeof(GTDDate));
        ///成交量类型 TThostFtdcVolumeConditionType char
        //// THOST_FTDC_VC_AV '1' 任何数量
        //// THOST_FTDC_VC_MV '2' 最小数量
        //// THOST_FTDC_VC_CV '3' 全部数量
        char VolumeCondition = pOrder->VolumeCondition;
        ///最小成交量 TThostFtdcVolumeType int
        int MinVolume = pOrder->MinVolume;
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
        char ContingentCondition = pOrder->ContingentCondition;
        ///止损价 TThostFtdcPriceType double
        double StopPrice = pOrder->StopPrice;
        ///强平原因 TThostFtdcForceCloseReasonType char
        //// THOST_FTDC_FCC_NotForceClose '0' 非强平
        //// THOST_FTDC_FCC_LackDeposit '1' 资金不足
        //// THOST_FTDC_FCC_ClientOverPositionLimit '2' 客户超仓
        //// THOST_FTDC_FCC_MemberOverPositionLimit '3' 会员超仓
        //// THOST_FTDC_FCC_NotMultiple '4' 持仓非整数倍
        //// THOST_FTDC_FCC_Violation '5' 违规
        //// THOST_FTDC_FCC_Other '6' 其它
        //// THOST_FTDC_FCC_PersonDeliv '7' 自然人临近交割
        char ForceCloseReason = pOrder->ForceCloseReason;
        ///自动挂起标志 TThostFtdcBoolType int
        int IsAutoSuspend = pOrder->IsAutoSuspend;
        ///业务单元 TThostFtdcBusinessUnitType char[21]
        char BusinessUnit[63];
        gbk2utf8(pOrder->BusinessUnit,BusinessUnit,sizeof(BusinessUnit));
        ///请求编号 TThostFtdcRequestIDType int
        int RequestID = pOrder->RequestID;
        ///本地报单编号 TThostFtdcOrderLocalIDType char[13]
        char OrderLocalID[39];
        gbk2utf8(pOrder->OrderLocalID,OrderLocalID,sizeof(OrderLocalID));
        ///交易所代码 TThostFtdcExchangeIDType char[9]
        char ExchangeID[27];
        gbk2utf8(pOrder->ExchangeID,ExchangeID,sizeof(ExchangeID));
        ///会员代码 TThostFtdcParticipantIDType char[11]
        char ParticipantID[33];
        gbk2utf8(pOrder->ParticipantID,ParticipantID,sizeof(ParticipantID));
        ///客户代码 TThostFtdcClientIDType char[11]
        char ClientID[33];
        gbk2utf8(pOrder->ClientID,ClientID,sizeof(ClientID));
        ///合约在交易所的代码 TThostFtdcExchangeInstIDType char[31]
        char ExchangeInstID[93];
        gbk2utf8(pOrder->ExchangeInstID,ExchangeInstID,sizeof(ExchangeInstID));
        ///交易所交易员代码 TThostFtdcTraderIDType char[21]
        char TraderID[63];
        gbk2utf8(pOrder->TraderID,TraderID,sizeof(TraderID));
        ///安装编号 TThostFtdcInstallIDType int
        int InstallID = pOrder->InstallID;
        ///报单提交状态 TThostFtdcOrderSubmitStatusType char
        //// THOST_FTDC_OSS_InsertSubmitted '0' 已经提交
        //// THOST_FTDC_OSS_CancelSubmitted '1' 撤单已经提交
        //// THOST_FTDC_OSS_ModifySubmitted '2' 修改已经提交
        //// THOST_FTDC_OSS_Accepted '3' 已经接受
        //// THOST_FTDC_OSS_InsertRejected '4' 报单已经被拒绝
        //// THOST_FTDC_OSS_CancelRejected '5' 撤单已经被拒绝
        //// THOST_FTDC_OSS_ModifyRejected '6' 改单已经被拒绝
        char OrderSubmitStatus = pOrder->OrderSubmitStatus;
        ///报单提示序号 TThostFtdcSequenceNoType int
        int NotifySequence = pOrder->NotifySequence;
        ///交易日 TThostFtdcDateType char[9]
        char TradingDay[27];
        gbk2utf8(pOrder->TradingDay,TradingDay,sizeof(TradingDay));
        ///结算编号 TThostFtdcSettlementIDType int
        int SettlementID = pOrder->SettlementID;
        ///报单编号 TThostFtdcOrderSysIDType char[21]
        char OrderSysID[63];
        gbk2utf8(pOrder->OrderSysID,OrderSysID,sizeof(OrderSysID));
        ///报单来源 TThostFtdcOrderSourceType char
        //// THOST_FTDC_OSRC_Participant '0' 来自参与者
        //// THOST_FTDC_OSRC_Administrator '1' 来自管理员
        char OrderSource = pOrder->OrderSource;
        ///报单状态 TThostFtdcOrderStatusType char
        //// THOST_FTDC_OST_AllTraded '0' 全部成交
        //// THOST_FTDC_OST_PartTradedQueueing '1' 部分成交还在队列中
        //// THOST_FTDC_OST_PartTradedNotQueueing '2' 部分成交不在队列中
        //// THOST_FTDC_OST_NoTradeQueueing '3' 未成交还在队列中
        //// THOST_FTDC_OST_NoTradeNotQueueing '4' 未成交不在队列中
        //// THOST_FTDC_OST_Canceled '5' 撤单
        //// THOST_FTDC_OST_Unknown 'a' 未知
        //// THOST_FTDC_OST_NotTouched 'b' 尚未触发
        //// THOST_FTDC_OST_Touched 'c' 已触发
        char OrderStatus = pOrder->OrderStatus;
        ///报单类型 TThostFtdcOrderTypeType char
        //// THOST_FTDC_ORDT_Normal '0' 正常
        //// THOST_FTDC_ORDT_DeriveFromQuote '1' 报价衍生
        //// THOST_FTDC_ORDT_DeriveFromCombination '2' 组合衍生
        //// THOST_FTDC_ORDT_Combination '3' 组合报单
        //// THOST_FTDC_ORDT_ConditionalOrder '4' 条件单
        //// THOST_FTDC_ORDT_Swap '5' 互换单
        char OrderType = pOrder->OrderType;
        ///今成交数量 TThostFtdcVolumeType int
        int VolumeTraded = pOrder->VolumeTraded;
        ///剩余数量 TThostFtdcVolumeType int
        int VolumeTotal = pOrder->VolumeTotal;
        ///报单日期 TThostFtdcDateType char[9]
        char InsertDate[27];
        gbk2utf8(pOrder->InsertDate,InsertDate,sizeof(InsertDate));
        ///委托时间 TThostFtdcTimeType char[9]
        char InsertTime[27];
        gbk2utf8(pOrder->InsertTime,InsertTime,sizeof(InsertTime));
        ///激活时间 TThostFtdcTimeType char[9]
        char ActiveTime[27];
        gbk2utf8(pOrder->ActiveTime,ActiveTime,sizeof(ActiveTime));
        ///挂起时间 TThostFtdcTimeType char[9]
        char SuspendTime[27];
        gbk2utf8(pOrder->SuspendTime,SuspendTime,sizeof(SuspendTime));
        ///最后修改时间 TThostFtdcTimeType char[9]
        char UpdateTime[27];
        gbk2utf8(pOrder->UpdateTime,UpdateTime,sizeof(UpdateTime));
        ///撤销时间 TThostFtdcTimeType char[9]
        char CancelTime[27];
        gbk2utf8(pOrder->CancelTime,CancelTime,sizeof(CancelTime));
        ///最后修改交易所交易员代码 TThostFtdcTraderIDType char[21]
        char ActiveTraderID[63];
        gbk2utf8(pOrder->ActiveTraderID,ActiveTraderID,sizeof(ActiveTraderID));
        ///结算会员编号 TThostFtdcParticipantIDType char[11]
        char ClearingPartID[33];
        gbk2utf8(pOrder->ClearingPartID,ClearingPartID,sizeof(ClearingPartID));
        ///序号 TThostFtdcSequenceNoType int
        int SequenceNo = pOrder->SequenceNo;
        ///前置编号 TThostFtdcFrontIDType int
        int FrontID = pOrder->FrontID;
        ///会话编号 TThostFtdcSessionIDType int
        int SessionID = pOrder->SessionID;
        ///用户端产品信息 TThostFtdcProductInfoType char[11]
        char UserProductInfo[33];
        gbk2utf8(pOrder->UserProductInfo,UserProductInfo,sizeof(UserProductInfo));
        ///状态信息 TThostFtdcErrorMsgType char[81]
        char StatusMsg[243];
        gbk2utf8(pOrder->StatusMsg,StatusMsg,sizeof(StatusMsg));
        ///用户强评标志 TThostFtdcBoolType int
        int UserForceClose = pOrder->UserForceClose;
        ///操作用户代码 TThostFtdcUserIDType char[16]
        char ActiveUserID[48];
        gbk2utf8(pOrder->ActiveUserID,ActiveUserID,sizeof(ActiveUserID));
        ///经纪公司报单编号 TThostFtdcSequenceNoType int
        int BrokerOrderSeq = pOrder->BrokerOrderSeq;
        ///相关报单 TThostFtdcOrderSysIDType char[21]
        char RelativeOrderSysID[63];
        gbk2utf8(pOrder->RelativeOrderSysID,RelativeOrderSysID,sizeof(RelativeOrderSysID));
        ///郑商所成交数量 TThostFtdcVolumeType int
        int ZCETotalTradedVolume = pOrder->ZCETotalTradedVolume;
        ///互换单标志 TThostFtdcBoolType int
        int IsSwapOrder = pOrder->IsSwapOrder;

        printf("InsertTime=[%s], InstrumentID=[%s], OrderRef=[%s], Direction=[%c], TimeCondition=[%c], OrderStatus=[%c], OrderSubmitStatus=[%c], OrderSysID=[%s], FrontID=[%d], SessionID=[%d], ExchangeID=[%s]\n",
            InsertTime, InstrumentID, OrderRef, Direction, TimeCondition, OrderStatus, OrderSubmitStatus, OrderSysID, FrontID, SessionID, ExchangeID);
        // printf("OrderPriceType=%c,",OrderPriceType); // 报单价格条件
        // printf("LimitPrice=%f,",LimitPrice); // 价格
        // printf("VolumeTotalOriginal=%d,",VolumeTotalOriginal); // 数量
    }

    // 如果响应函数已经返回最后一条信息
    if(bIsLast) {
        // 通知主过程，响应函数将结束
        sem_post(&sem);
    }
}

void CTraderHandler::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField * pDepthMarketData, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast) {
    printf("OnRspQryDepthMarketData():called...\n");

    // 进程返回结果检查
    if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
        // typedef int TThostFtdcErrorIDType;
        // typedef char TThostFtdcErrorMsgType[81];
        char ErrorMsg[243];
        gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
        printf("OnRspQryDepthMarketData():Error:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
    }

    // 如果有返回结果读取返回信息
    if ( pDepthMarketData != NULL ) {
        // 读取返回信息,并做编码转化
        ///交易日 TThostFtdcDateType char[9]
        char TradingDay[27];
        gbk2utf8(pDepthMarketData->TradingDay,TradingDay,sizeof(TradingDay));
        ///合约代码 TThostFtdcInstrumentIDType char[31]
        char InstrumentID[93];
        gbk2utf8(pDepthMarketData->InstrumentID,InstrumentID,sizeof(InstrumentID));
        ///交易所代码 TThostFtdcExchangeIDType char[9]
        char ExchangeID[27];
        gbk2utf8(pDepthMarketData->ExchangeID,ExchangeID,sizeof(ExchangeID));
        ///合约在交易所的代码 TThostFtdcExchangeInstIDType char[31]
        char ExchangeInstID[93];
        gbk2utf8(pDepthMarketData->ExchangeInstID,ExchangeInstID,sizeof(ExchangeInstID));
        ///最新价 TThostFtdcPriceType double
        double LastPrice = pDepthMarketData->LastPrice;
        ///上次结算价 TThostFtdcPriceType double
        double PreSettlementPrice = pDepthMarketData->PreSettlementPrice;
        ///昨收盘 TThostFtdcPriceType double
        double PreClosePrice = pDepthMarketData->PreClosePrice;
        ///昨持仓量 TThostFtdcLargeVolumeType double
        double PreOpenInterest = pDepthMarketData->PreOpenInterest;
        ///今开盘 TThostFtdcPriceType double
        double OpenPrice = pDepthMarketData->OpenPrice;
        ///最高价 TThostFtdcPriceType double
        double HighestPrice = pDepthMarketData->HighestPrice;
        ///最低价 TThostFtdcPriceType double
        double LowestPrice = pDepthMarketData->LowestPrice;
        ///数量 TThostFtdcVolumeType int
        int Volume = pDepthMarketData->Volume;
        ///成交金额 TThostFtdcMoneyType double
        double Turnover = pDepthMarketData->Turnover;
        ///持仓量 TThostFtdcLargeVolumeType double
        double OpenInterest = pDepthMarketData->OpenInterest;
        ///今收盘 TThostFtdcPriceType double
        double ClosePrice = pDepthMarketData->ClosePrice;
        ///本次结算价 TThostFtdcPriceType double
        double SettlementPrice = pDepthMarketData->SettlementPrice;
        ///涨停板价 TThostFtdcPriceType double
        double UpperLimitPrice = pDepthMarketData->UpperLimitPrice;
        ///跌停板价 TThostFtdcPriceType double
        double LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
        ///昨虚实度 TThostFtdcRatioType double
        double PreDelta = pDepthMarketData->PreDelta;
        ///今虚实度 TThostFtdcRatioType double
        double CurrDelta = pDepthMarketData->CurrDelta;
        ///最后修改时间 TThostFtdcTimeType char[9]
        char UpdateTime[27];
        gbk2utf8(pDepthMarketData->UpdateTime,UpdateTime,sizeof(UpdateTime));
        ///最后修改毫秒 TThostFtdcMillisecType int
        int UpdateMillisec = pDepthMarketData->UpdateMillisec;
        ///申买价一 TThostFtdcPriceType double
        double BidPrice1 = pDepthMarketData->BidPrice1;
        ///申买量一 TThostFtdcVolumeType int
        int BidVolume1 = pDepthMarketData->BidVolume1;
        ///申卖价一 TThostFtdcPriceType double
        double AskPrice1 = pDepthMarketData->AskPrice1;
        ///申卖量一 TThostFtdcVolumeType int
        int AskVolume1 = pDepthMarketData->AskVolume1;
        ///申买价二 TThostFtdcPriceType double
        double BidPrice2 = pDepthMarketData->BidPrice2;
        ///申买量二 TThostFtdcVolumeType int
        int BidVolume2 = pDepthMarketData->BidVolume2;
        ///申卖价二 TThostFtdcPriceType double
        double AskPrice2 = pDepthMarketData->AskPrice2;
        ///申卖量二 TThostFtdcVolumeType int
        int AskVolume2 = pDepthMarketData->AskVolume2;
        ///申买价三 TThostFtdcPriceType double
        double BidPrice3 = pDepthMarketData->BidPrice3;
        ///申买量三 TThostFtdcVolumeType int
        int BidVolume3 = pDepthMarketData->BidVolume3;
        ///申卖价三 TThostFtdcPriceType double
        double AskPrice3 = pDepthMarketData->AskPrice3;
        ///申卖量三 TThostFtdcVolumeType int
        int AskVolume3 = pDepthMarketData->AskVolume3;
        ///申买价四 TThostFtdcPriceType double
        double BidPrice4 = pDepthMarketData->BidPrice4;
        ///申买量四 TThostFtdcVolumeType int
        int BidVolume4 = pDepthMarketData->BidVolume4;
        ///申卖价四 TThostFtdcPriceType double
        double AskPrice4 = pDepthMarketData->AskPrice4;
        ///申卖量四 TThostFtdcVolumeType int
        int AskVolume4 = pDepthMarketData->AskVolume4;
        ///申买价五 TThostFtdcPriceType double
        double BidPrice5 = pDepthMarketData->BidPrice5;
        ///申买量五 TThostFtdcVolumeType int
        int BidVolume5 = pDepthMarketData->BidVolume5;
        ///申卖价五 TThostFtdcPriceType double
        double AskPrice5 = pDepthMarketData->AskPrice5;
        ///申卖量五 TThostFtdcVolumeType int
        int AskVolume5 = pDepthMarketData->AskVolume5;
        ///当日均价 TThostFtdcPriceType double
        double AveragePrice = pDepthMarketData->AveragePrice;
        ///业务日期 TThostFtdcDateType char[9]
        char ActionDay[27];
        gbk2utf8(pDepthMarketData->ActionDay,ActionDay,sizeof(ActionDay));

        printf("TradingDay=%s,",TradingDay);    // 交易日
        printf("InstrumentID=%s,",InstrumentID);  // 合约代码
        printf("LastPrice=%f\n",LastPrice);  // 最新价
    }

    // 如果响应函数已经返回最后一条信息
    if(bIsLast) {
        // 通知主过程，响应函数将结束
        sem_post(&sem);
    }
}

void CTraderHandler::OnRtnOrder(CThostFtdcOrderField * pOrder) {
    printf("OnRtnOrder():is called...\n");
    // 如果有返回结果读取返回信息
    if ( pOrder != NULL ) {
        // 读取返回信息,并做编码转化
        ///经纪公司代码 TThostFtdcBrokerIDType char[11]
        char BrokerID[33];
        gbk2utf8(pOrder->BrokerID,BrokerID,sizeof(BrokerID));
        ///投资者代码 TThostFtdcInvestorIDType char[13]
        char InvestorID[39];
        gbk2utf8(pOrder->InvestorID,InvestorID,sizeof(InvestorID));
        ///合约代码 TThostFtdcInstrumentIDType char[31]
        char InstrumentID[93];
        gbk2utf8(pOrder->InstrumentID,InstrumentID,sizeof(InstrumentID));
        ///报单引用 TThostFtdcOrderRefType char[13]
        char OrderRef[39];
        gbk2utf8(pOrder->OrderRef,OrderRef,sizeof(OrderRef));
        ///用户代码 TThostFtdcUserIDType char[16]
        char UserID[48];
        gbk2utf8(pOrder->UserID,UserID,sizeof(UserID));
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
        char OrderPriceType = pOrder->OrderPriceType;
        ///买卖方向 TThostFtdcDirectionType char
        //// THOST_FTDC_D_Buy '0' 买
        //// THOST_FTDC_D_Sell '1' 卖
        char Direction = pOrder->Direction;
        ///组合开平标志 TThostFtdcCombOffsetFlagType char[5]
        //// THOST_FTDC_OF_Open '0' 开仓
        //// THOST_FTDC_OF_Close '1' 平仓
        //// THOST_FTDC_OF_ForceClose '2' 强平
        //// THOST_FTDC_OF_CloseToday '3' 平今
        //// THOST_FTDC_OF_CloseYesterday '4' 平昨
        //// THOST_FTDC_OF_ForceOff '5' 强减
        //// THOST_FTDC_OF_LocalForceClose '6' 本地强平
        char CombOffsetFlag[15];
        gbk2utf8(pOrder->CombOffsetFlag,CombOffsetFlag,sizeof(CombOffsetFlag));
        ///组合投机套保标志 TThostFtdcCombHedgeFlagType char[5]
        //// THOST_FTDC_HF_Speculation '1' 投机
        //// THOST_FTDC_HF_Arbitrage '2' 套利
        //// THOST_FTDC_HF_Hedge '3' 套保
        char CombHedgeFlag[15];
        gbk2utf8(pOrder->CombHedgeFlag,CombHedgeFlag,sizeof(CombHedgeFlag));
        ///价格 TThostFtdcPriceType double
        double LimitPrice = pOrder->LimitPrice;
        ///数量 TThostFtdcVolumeType int
        int VolumeTotalOriginal = pOrder->VolumeTotalOriginal;
        ///有效期类型 TThostFtdcTimeConditionType char
        //// THOST_FTDC_TC_IOC '1' 立即完成，否则撤销
        //// THOST_FTDC_TC_GFS '2' 本节有效
        //// THOST_FTDC_TC_GFD '3' 当日有效
        //// THOST_FTDC_TC_GTD '4' 指定日期前有效
        //// THOST_FTDC_TC_GTC '5' 撤销前有效
        //// THOST_FTDC_TC_GFA '6' 集合竞价有效
        char TimeCondition = pOrder->TimeCondition;
        ///GTD日期 TThostFtdcDateType char[9]
        char GTDDate[27];
        gbk2utf8(pOrder->GTDDate,GTDDate,sizeof(GTDDate));
        ///成交量类型 TThostFtdcVolumeConditionType char
        //// THOST_FTDC_VC_AV '1' 任何数量
        //// THOST_FTDC_VC_MV '2' 最小数量
        //// THOST_FTDC_VC_CV '3' 全部数量
        char VolumeCondition = pOrder->VolumeCondition;
        ///最小成交量 TThostFtdcVolumeType int
        int MinVolume = pOrder->MinVolume;
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
        char ContingentCondition = pOrder->ContingentCondition;
        ///止损价 TThostFtdcPriceType double
        double StopPrice = pOrder->StopPrice;
        ///强平原因 TThostFtdcForceCloseReasonType char
        //// THOST_FTDC_FCC_NotForceClose '0' 非强平
        //// THOST_FTDC_FCC_LackDeposit '1' 资金不足
        //// THOST_FTDC_FCC_ClientOverPositionLimit '2' 客户超仓
        //// THOST_FTDC_FCC_MemberOverPositionLimit '3' 会员超仓
        //// THOST_FTDC_FCC_NotMultiple '4' 持仓非整数倍
        //// THOST_FTDC_FCC_Violation '5' 违规
        //// THOST_FTDC_FCC_Other '6' 其它
        //// THOST_FTDC_FCC_PersonDeliv '7' 自然人临近交割
        char ForceCloseReason = pOrder->ForceCloseReason;
        ///自动挂起标志 TThostFtdcBoolType int
        int IsAutoSuspend = pOrder->IsAutoSuspend;
        ///业务单元 TThostFtdcBusinessUnitType char[21]
        char BusinessUnit[63];
        gbk2utf8(pOrder->BusinessUnit,BusinessUnit,sizeof(BusinessUnit));
        ///请求编号 TThostFtdcRequestIDType int
        int RequestID = pOrder->RequestID;
        ///本地报单编号 TThostFtdcOrderLocalIDType char[13]
        char OrderLocalID[39];
        gbk2utf8(pOrder->OrderLocalID,OrderLocalID,sizeof(OrderLocalID));
        ///交易所代码 TThostFtdcExchangeIDType char[9]
        char ExchangeID[27];
        gbk2utf8(pOrder->ExchangeID,ExchangeID,sizeof(ExchangeID));
        ///会员代码 TThostFtdcParticipantIDType char[11]
        char ParticipantID[33];
        gbk2utf8(pOrder->ParticipantID,ParticipantID,sizeof(ParticipantID));
        ///客户代码 TThostFtdcClientIDType char[11]
        char ClientID[33];
        gbk2utf8(pOrder->ClientID,ClientID,sizeof(ClientID));
        ///合约在交易所的代码 TThostFtdcExchangeInstIDType char[31]
        char ExchangeInstID[93];
        gbk2utf8(pOrder->ExchangeInstID,ExchangeInstID,sizeof(ExchangeInstID));
        ///交易所交易员代码 TThostFtdcTraderIDType char[21]
        char TraderID[63];
        gbk2utf8(pOrder->TraderID,TraderID,sizeof(TraderID));
        ///安装编号 TThostFtdcInstallIDType int
        int InstallID = pOrder->InstallID;
        ///报单提交状态 TThostFtdcOrderSubmitStatusType char
        //// THOST_FTDC_OSS_InsertSubmitted '0' 已经提交
        //// THOST_FTDC_OSS_CancelSubmitted '1' 撤单已经提交
        //// THOST_FTDC_OSS_ModifySubmitted '2' 修改已经提交
        //// THOST_FTDC_OSS_Accepted '3' 已经接受
        //// THOST_FTDC_OSS_InsertRejected '4' 报单已经被拒绝
        //// THOST_FTDC_OSS_CancelRejected '5' 撤单已经被拒绝
        //// THOST_FTDC_OSS_ModifyRejected '6' 改单已经被拒绝
        char OrderSubmitStatus = pOrder->OrderSubmitStatus;
        ///报单提示序号 TThostFtdcSequenceNoType int
        int NotifySequence = pOrder->NotifySequence;
        ///交易日 TThostFtdcDateType char[9]
        char TradingDay[27];
        gbk2utf8(pOrder->TradingDay,TradingDay,sizeof(TradingDay));
        ///结算编号 TThostFtdcSettlementIDType int
        int SettlementID = pOrder->SettlementID;
        ///报单编号 TThostFtdcOrderSysIDType char[21]
        char OrderSysID[63];
        gbk2utf8(pOrder->OrderSysID,OrderSysID,sizeof(OrderSysID));
        ///报单来源 TThostFtdcOrderSourceType char
        //// THOST_FTDC_OSRC_Participant '0' 来自参与者
        //// THOST_FTDC_OSRC_Administrator '1' 来自管理员
        char OrderSource = pOrder->OrderSource;
        ///报单状态 TThostFtdcOrderStatusType char
        //// THOST_FTDC_OST_AllTraded '0' 全部成交
        //// THOST_FTDC_OST_PartTradedQueueing '1' 部分成交还在队列中
        //// THOST_FTDC_OST_PartTradedNotQueueing '2' 部分成交不在队列中
        //// THOST_FTDC_OST_NoTradeQueueing '3' 未成交还在队列中
        //// THOST_FTDC_OST_NoTradeNotQueueing '4' 未成交不在队列中
        //// THOST_FTDC_OST_Canceled '5' 撤单
        //// THOST_FTDC_OST_Unknown 'a' 未知
        //// THOST_FTDC_OST_NotTouched 'b' 尚未触发
        //// THOST_FTDC_OST_Touched 'c' 已触发
        char OrderStatus = pOrder->OrderStatus;
        ///报单类型 TThostFtdcOrderTypeType char
        //// THOST_FTDC_ORDT_Normal '0' 正常
        //// THOST_FTDC_ORDT_DeriveFromQuote '1' 报价衍生
        //// THOST_FTDC_ORDT_DeriveFromCombination '2' 组合衍生
        //// THOST_FTDC_ORDT_Combination '3' 组合报单
        //// THOST_FTDC_ORDT_ConditionalOrder '4' 条件单
        //// THOST_FTDC_ORDT_Swap '5' 互换单
        char OrderType = pOrder->OrderType;
        ///今成交数量 TThostFtdcVolumeType int
        int VolumeTraded = pOrder->VolumeTraded;
        ///剩余数量 TThostFtdcVolumeType int
        int VolumeTotal = pOrder->VolumeTotal;
        ///报单日期 TThostFtdcDateType char[9]
        char InsertDate[27];
        gbk2utf8(pOrder->InsertDate,InsertDate,sizeof(InsertDate));
        ///委托时间 TThostFtdcTimeType char[9]
        char InsertTime[27];
        gbk2utf8(pOrder->InsertTime,InsertTime,sizeof(InsertTime));
        ///激活时间 TThostFtdcTimeType char[9]
        char ActiveTime[27];
        gbk2utf8(pOrder->ActiveTime,ActiveTime,sizeof(ActiveTime));
        ///挂起时间 TThostFtdcTimeType char[9]
        char SuspendTime[27];
        gbk2utf8(pOrder->SuspendTime,SuspendTime,sizeof(SuspendTime));
        ///最后修改时间 TThostFtdcTimeType char[9]
        char UpdateTime[27];
        gbk2utf8(pOrder->UpdateTime,UpdateTime,sizeof(UpdateTime));
        ///撤销时间 TThostFtdcTimeType char[9]
        char CancelTime[27];
        gbk2utf8(pOrder->CancelTime,CancelTime,sizeof(CancelTime));
        ///最后修改交易所交易员代码 TThostFtdcTraderIDType char[21]
        char ActiveTraderID[63];
        gbk2utf8(pOrder->ActiveTraderID,ActiveTraderID,sizeof(ActiveTraderID));
        ///结算会员编号 TThostFtdcParticipantIDType char[11]
        char ClearingPartID[33];
        gbk2utf8(pOrder->ClearingPartID,ClearingPartID,sizeof(ClearingPartID));
        ///序号 TThostFtdcSequenceNoType int
        int SequenceNo = pOrder->SequenceNo;
        ///前置编号 TThostFtdcFrontIDType int
        int FrontID = pOrder->FrontID;
        ///会话编号 TThostFtdcSessionIDType int
        int SessionID = pOrder->SessionID;
        ///用户端产品信息 TThostFtdcProductInfoType char[11]
        char UserProductInfo[33];
        gbk2utf8(pOrder->UserProductInfo,UserProductInfo,sizeof(UserProductInfo));
        ///状态信息 TThostFtdcErrorMsgType char[81]
        char StatusMsg[243];
        gbk2utf8(pOrder->StatusMsg,StatusMsg,sizeof(StatusMsg));
        ///用户强评标志 TThostFtdcBoolType int
        int UserForceClose = pOrder->UserForceClose;
        ///操作用户代码 TThostFtdcUserIDType char[16]
        char ActiveUserID[48];
        gbk2utf8(pOrder->ActiveUserID,ActiveUserID,sizeof(ActiveUserID));
        ///经纪公司报单编号 TThostFtdcSequenceNoType int
        int BrokerOrderSeq = pOrder->BrokerOrderSeq;
        ///相关报单 TThostFtdcOrderSysIDType char[21]
        char RelativeOrderSysID[63];
        gbk2utf8(pOrder->RelativeOrderSysID,RelativeOrderSysID,sizeof(RelativeOrderSysID));
        ///郑商所成交数量 TThostFtdcVolumeType int
        int ZCETotalTradedVolume = pOrder->ZCETotalTradedVolume;
        ///互换单标志 TThostFtdcBoolType int
        int IsSwapOrder = pOrder->IsSwapOrder;

        printf("OrderSubmitStatus=[%c] OrderStatus=[%c] StatusMsg=[%s] VolumeTraded=[%i]\n", OrderSubmitStatus, OrderStatus, StatusMsg, VolumeTraded);
    }
}

void CTraderHandler::OnErrRtnOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo) {
    printf("OnErrRtnOrderInsert():is called...\n");
    // 如果有返回结果读取返回信息
    if ( pInputOrder != NULL ) {
        // 读取返回信息,并做编码转化
        ///经纪公司代码 TThostFtdcBrokerIDType char[11]
        char BrokerID[33];
        gbk2utf8(pInputOrder->BrokerID,BrokerID,sizeof(BrokerID));
        ///投资者代码 TThostFtdcInvestorIDType char[13]
        char InvestorID[39];
        gbk2utf8(pInputOrder->InvestorID,InvestorID,sizeof(InvestorID));
        ///合约代码 TThostFtdcInstrumentIDType char[31]
        char InstrumentID[93];
        gbk2utf8(pInputOrder->InstrumentID,InstrumentID,sizeof(InstrumentID));
        ///报单引用 TThostFtdcOrderRefType char[13]
        char OrderRef[39];
        gbk2utf8(pInputOrder->OrderRef,OrderRef,sizeof(OrderRef));
        ///用户代码 TThostFtdcUserIDType char[16]
        char UserID[48];
        gbk2utf8(pInputOrder->UserID,UserID,sizeof(UserID));
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
        char OrderPriceType = pInputOrder->OrderPriceType;
        ///买卖方向 TThostFtdcDirectionType char
        //// THOST_FTDC_D_Buy '0' 买
        //// THOST_FTDC_D_Sell '1' 卖
        char Direction = pInputOrder->Direction;
        ///组合开平标志 TThostFtdcCombOffsetFlagType char[5]
        //// THOST_FTDC_OF_Open '0' 开仓
        //// THOST_FTDC_OF_Close '1' 平仓
        //// THOST_FTDC_OF_ForceClose '2' 强平
        //// THOST_FTDC_OF_CloseToday '3' 平今
        //// THOST_FTDC_OF_CloseYesterday '4' 平昨
        //// THOST_FTDC_OF_ForceOff '5' 强减
        //// THOST_FTDC_OF_LocalForceClose '6' 本地强平
        char CombOffsetFlag[15];
        gbk2utf8(pInputOrder->CombOffsetFlag,CombOffsetFlag,sizeof(CombOffsetFlag));
        ///组合投机套保标志 TThostFtdcCombHedgeFlagType char[5]
        //// THOST_FTDC_HF_Speculation '1' 投机
        //// THOST_FTDC_HF_Arbitrage '2' 套利
        //// THOST_FTDC_HF_Hedge '3' 套保
        char CombHedgeFlag[15];
        gbk2utf8(pInputOrder->CombHedgeFlag,CombHedgeFlag,sizeof(CombHedgeFlag));
        ///价格 TThostFtdcPriceType double
        double LimitPrice = pInputOrder->LimitPrice;
        ///数量 TThostFtdcVolumeType int
        int VolumeTotalOriginal = pInputOrder->VolumeTotalOriginal;
        ///有效期类型 TThostFtdcTimeConditionType char
        //// THOST_FTDC_TC_IOC '1' 立即完成，否则撤销
        //// THOST_FTDC_TC_GFS '2' 本节有效
        //// THOST_FTDC_TC_GFD '3' 当日有效
        //// THOST_FTDC_TC_GTD '4' 指定日期前有效
        //// THOST_FTDC_TC_GTC '5' 撤销前有效
        //// THOST_FTDC_TC_GFA '6' 集合竞价有效
        char TimeCondition = pInputOrder->TimeCondition;
        ///GTD日期 TThostFtdcDateType char[9]
        char GTDDate[27];
        gbk2utf8(pInputOrder->GTDDate,GTDDate,sizeof(GTDDate));
        ///成交量类型 TThostFtdcVolumeConditionType char
        //// THOST_FTDC_VC_AV '1' 任何数量
        //// THOST_FTDC_VC_MV '2' 最小数量
        //// THOST_FTDC_VC_CV '3' 全部数量
        char VolumeCondition = pInputOrder->VolumeCondition;
        ///最小成交量 TThostFtdcVolumeType int
        int MinVolume = pInputOrder->MinVolume;
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
        char ContingentCondition = pInputOrder->ContingentCondition;
        ///止损价 TThostFtdcPriceType double
        double StopPrice = pInputOrder->StopPrice;
        ///强平原因 TThostFtdcForceCloseReasonType char
        //// THOST_FTDC_FCC_NotForceClose '0' 非强平
        //// THOST_FTDC_FCC_LackDeposit '1' 资金不足
        //// THOST_FTDC_FCC_ClientOverPositionLimit '2' 客户超仓
        //// THOST_FTDC_FCC_MemberOverPositionLimit '3' 会员超仓
        //// THOST_FTDC_FCC_NotMultiple '4' 持仓非整数倍
        //// THOST_FTDC_FCC_Violation '5' 违规
        //// THOST_FTDC_FCC_Other '6' 其它
        //// THOST_FTDC_FCC_PersonDeliv '7' 自然人临近交割
        char ForceCloseReason = pInputOrder->ForceCloseReason;
        ///自动挂起标志 TThostFtdcBoolType int
        int IsAutoSuspend = pInputOrder->IsAutoSuspend;
        ///业务单元 TThostFtdcBusinessUnitType char[21]
        char BusinessUnit[63];
        gbk2utf8(pInputOrder->BusinessUnit,BusinessUnit,sizeof(BusinessUnit));
        ///请求编号 TThostFtdcRequestIDType int
        int RequestID = pInputOrder->RequestID;
        ///用户强评标志 TThostFtdcBoolType int
        int UserForceClose = pInputOrder->UserForceClose;
        ///互换单标志 TThostFtdcBoolType int
        int IsSwapOrder = pInputOrder->IsSwapOrder;

    }

}

void CTraderHandler::OnRtnTrade(CThostFtdcTradeField * pTrade) {
    printf("OnRtnTrade():is called...\n");
    // 如果有返回结果读取返回信息
    if ( pTrade != NULL ) {
        // 读取返回信息,并做编码转化
        ///经纪公司代码 TThostFtdcBrokerIDType char[11]
        char BrokerID[33];
        gbk2utf8(pTrade->BrokerID,BrokerID,sizeof(BrokerID));
        ///投资者代码 TThostFtdcInvestorIDType char[13]
        char InvestorID[39];
        gbk2utf8(pTrade->InvestorID,InvestorID,sizeof(InvestorID));
        ///合约代码 TThostFtdcInstrumentIDType char[31]
        char InstrumentID[93];
        gbk2utf8(pTrade->InstrumentID,InstrumentID,sizeof(InstrumentID));
        ///报单引用 TThostFtdcOrderRefType char[13]
        char OrderRef[39];
        gbk2utf8(pTrade->OrderRef,OrderRef,sizeof(OrderRef));
        ///用户代码 TThostFtdcUserIDType char[16]
        char UserID[48];
        gbk2utf8(pTrade->UserID,UserID,sizeof(UserID));
        ///交易所代码 TThostFtdcExchangeIDType char[9]
        char ExchangeID[27];
        gbk2utf8(pTrade->ExchangeID,ExchangeID,sizeof(ExchangeID));
        ///成交编号 TThostFtdcTradeIDType char[21]
        char TradeID[63];
        gbk2utf8(pTrade->TradeID,TradeID,sizeof(TradeID));
        ///买卖方向 TThostFtdcDirectionType char
        //// THOST_FTDC_D_Buy '0' 买
        //// THOST_FTDC_D_Sell '1' 卖
        char Direction = pTrade->Direction;
        ///报单编号 TThostFtdcOrderSysIDType char[21]
        char OrderSysID[63];
        gbk2utf8(pTrade->OrderSysID,OrderSysID,sizeof(OrderSysID));
        ///会员代码 TThostFtdcParticipantIDType char[11]
        char ParticipantID[33];
        gbk2utf8(pTrade->ParticipantID,ParticipantID,sizeof(ParticipantID));
        ///客户代码 TThostFtdcClientIDType char[11]
        char ClientID[33];
        gbk2utf8(pTrade->ClientID,ClientID,sizeof(ClientID));
        ///交易角色 TThostFtdcTradingRoleType char
        //// THOST_FTDC_ER_Broker '1' 代理
        //// THOST_FTDC_ER_Host '2' 自营
        //// THOST_FTDC_ER_Maker '3' 做市商
        char TradingRole = pTrade->TradingRole;
        ///合约在交易所的代码 TThostFtdcExchangeInstIDType char[31]
        char ExchangeInstID[93];
        gbk2utf8(pTrade->ExchangeInstID,ExchangeInstID,sizeof(ExchangeInstID));
        ///开平标志 TThostFtdcOffsetFlagType char
        //// THOST_FTDC_OF_Open '0' 开仓
        //// THOST_FTDC_OF_Close '1' 平仓
        //// THOST_FTDC_OF_ForceClose '2' 强平
        //// THOST_FTDC_OF_CloseToday '3' 平今
        //// THOST_FTDC_OF_CloseYesterday '4' 平昨
        //// THOST_FTDC_OF_ForceOff '5' 强减
        //// THOST_FTDC_OF_LocalForceClose '6' 本地强平
        char OffsetFlag = pTrade->OffsetFlag;
        ///投机套保标志 TThostFtdcHedgeFlagType char
        //// THOST_FTDC_HF_Speculation '1' 投机
        //// THOST_FTDC_HF_Arbitrage '2' 套利
        //// THOST_FTDC_HF_Hedge '3' 套保
        char HedgeFlag = pTrade->HedgeFlag;
        ///价格 TThostFtdcPriceType double
        double Price = pTrade->Price;
        ///数量 TThostFtdcVolumeType int
        int Volume = pTrade->Volume;
        ///成交时期 TThostFtdcDateType char[9]
        char TradeDate[27];
        gbk2utf8(pTrade->TradeDate,TradeDate,sizeof(TradeDate));
        ///成交时间 TThostFtdcTimeType char[9]
        char TradeTime[27];
        gbk2utf8(pTrade->TradeTime,TradeTime,sizeof(TradeTime));
        ///成交类型 TThostFtdcTradeTypeType char
        //// THOST_FTDC_TRDT_SplitCombination '#' 组合持仓拆分为单一持仓,初始化不应包含该类型的持仓
        //// THOST_FTDC_TRDT_Common '0' 普通成交
        //// THOST_FTDC_TRDT_OptionsExecution '1' 期权执行
        //// THOST_FTDC_TRDT_OTC '2' OTC成交
        //// THOST_FTDC_TRDT_EFPDerived '3' 期转现衍生成交
        //// THOST_FTDC_TRDT_CombinationDerived '4' 组合衍生成交
        char TradeType = pTrade->TradeType;
        ///成交价来源 TThostFtdcPriceSourceType char
        //// THOST_FTDC_PSRC_LastPrice '0' 前成交价
        //// THOST_FTDC_PSRC_Buy '1' 买委托价
        //// THOST_FTDC_PSRC_Sell '2' 卖委托价
        char PriceSource = pTrade->PriceSource;
        ///交易所交易员代码 TThostFtdcTraderIDType char[21]
        char TraderID[63];
        gbk2utf8(pTrade->TraderID,TraderID,sizeof(TraderID));
        ///本地报单编号 TThostFtdcOrderLocalIDType char[13]
        char OrderLocalID[39];
        gbk2utf8(pTrade->OrderLocalID,OrderLocalID,sizeof(OrderLocalID));
        ///结算会员编号 TThostFtdcParticipantIDType char[11]
        char ClearingPartID[33];
        gbk2utf8(pTrade->ClearingPartID,ClearingPartID,sizeof(ClearingPartID));
        ///业务单元 TThostFtdcBusinessUnitType char[21]
        char BusinessUnit[63];
        gbk2utf8(pTrade->BusinessUnit,BusinessUnit,sizeof(BusinessUnit));
        ///序号 TThostFtdcSequenceNoType int
        int SequenceNo = pTrade->SequenceNo;
        ///交易日 TThostFtdcDateType char[9]
        char TradingDay[27];
        gbk2utf8(pTrade->TradingDay,TradingDay,sizeof(TradingDay));
        ///结算编号 TThostFtdcSettlementIDType int
        int SettlementID = pTrade->SettlementID;
        ///经纪公司报单编号 TThostFtdcSequenceNoType int
        int BrokerOrderSeq = pTrade->BrokerOrderSeq;
        ///成交来源 TThostFtdcTradeSourceType char
        //// THOST_FTDC_TSRC_NORMAL '0' 来自交易所普通回报
        //// THOST_FTDC_TSRC_QUERY '1' 来自查询
        char TradeSource = pTrade->TradeSource;

        printf("InstrumentID=[%s] Direction=[%c] Price=[%f] Volume=[%i]\n", InstrumentID, Direction, Price, Volume);
        sem_post(&sem);
    }

}

void CTraderHandler::OnRspOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo,int nRequestID, bool bIsLast) {
    printf("OnRspOrderInsert():is called...\n");

    // 进程返回结果检查
    if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
        // typedef int TThostFtdcErrorIDType;
        // typedef char TThostFtdcErrorMsgType[81];
        char ErrorMsg[243];
        gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
        printf("OnRspOrderInsert():Error:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
    }

    // 如果有返回结果读取返回信息
    if ( pInputOrder != NULL ) {
        // 读取返回信息,并做编码转化
        ///经纪公司代码 TThostFtdcBrokerIDType char[11]
        char BrokerID[33];
        gbk2utf8(pInputOrder->BrokerID,BrokerID,sizeof(BrokerID));
        ///投资者代码 TThostFtdcInvestorIDType char[13]
        char InvestorID[39];
        gbk2utf8(pInputOrder->InvestorID,InvestorID,sizeof(InvestorID));
        ///合约代码 TThostFtdcInstrumentIDType char[31]
        char InstrumentID[93];
        gbk2utf8(pInputOrder->InstrumentID,InstrumentID,sizeof(InstrumentID));
        ///报单引用 TThostFtdcOrderRefType char[13]
        char OrderRef[39];
        gbk2utf8(pInputOrder->OrderRef,OrderRef,sizeof(OrderRef));
        ///用户代码 TThostFtdcUserIDType char[16]
        char UserID[48];
        gbk2utf8(pInputOrder->UserID,UserID,sizeof(UserID));
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
        char OrderPriceType = pInputOrder->OrderPriceType;
        ///买卖方向 TThostFtdcDirectionType char
        //// THOST_FTDC_D_Buy '0' 买
        //// THOST_FTDC_D_Sell '1' 卖
        char Direction = pInputOrder->Direction;
        ///组合开平标志 TThostFtdcCombOffsetFlagType char[5]
        //// THOST_FTDC_OF_Open '0' 开仓
        //// THOST_FTDC_OF_Close '1' 平仓
        //// THOST_FTDC_OF_ForceClose '2' 强平
        //// THOST_FTDC_OF_CloseToday '3' 平今
        //// THOST_FTDC_OF_CloseYesterday '4' 平昨
        //// THOST_FTDC_OF_ForceOff '5' 强减
        //// THOST_FTDC_OF_LocalForceClose '6' 本地强平
        char CombOffsetFlag[15];
        gbk2utf8(pInputOrder->CombOffsetFlag,CombOffsetFlag,sizeof(CombOffsetFlag));
        ///组合投机套保标志 TThostFtdcCombHedgeFlagType char[5]
        //// THOST_FTDC_HF_Speculation '1' 投机
        //// THOST_FTDC_HF_Arbitrage '2' 套利
        //// THOST_FTDC_HF_Hedge '3' 套保
        char CombHedgeFlag[15];
        gbk2utf8(pInputOrder->CombHedgeFlag,CombHedgeFlag,sizeof(CombHedgeFlag));
        ///价格 TThostFtdcPriceType double
        double LimitPrice = pInputOrder->LimitPrice;
        ///数量 TThostFtdcVolumeType int
        int VolumeTotalOriginal = pInputOrder->VolumeTotalOriginal;
        ///有效期类型 TThostFtdcTimeConditionType char
        //// THOST_FTDC_TC_IOC '1' 立即完成，否则撤销
        //// THOST_FTDC_TC_GFS '2' 本节有效
        //// THOST_FTDC_TC_GFD '3' 当日有效
        //// THOST_FTDC_TC_GTD '4' 指定日期前有效
        //// THOST_FTDC_TC_GTC '5' 撤销前有效
        //// THOST_FTDC_TC_GFA '6' 集合竞价有效
        char TimeCondition = pInputOrder->TimeCondition;
        ///GTD日期 TThostFtdcDateType char[9]
        char GTDDate[27];
        gbk2utf8(pInputOrder->GTDDate,GTDDate,sizeof(GTDDate));
        ///成交量类型 TThostFtdcVolumeConditionType char
        //// THOST_FTDC_VC_AV '1' 任何数量
        //// THOST_FTDC_VC_MV '2' 最小数量
        //// THOST_FTDC_VC_CV '3' 全部数量
        char VolumeCondition = pInputOrder->VolumeCondition;
        ///最小成交量 TThostFtdcVolumeType int
        int MinVolume = pInputOrder->MinVolume;
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
        char ContingentCondition = pInputOrder->ContingentCondition;
        ///止损价 TThostFtdcPriceType double
        double StopPrice = pInputOrder->StopPrice;
        ///强平原因 TThostFtdcForceCloseReasonType char
        //// THOST_FTDC_FCC_NotForceClose '0' 非强平
        //// THOST_FTDC_FCC_LackDeposit '1' 资金不足
        //// THOST_FTDC_FCC_ClientOverPositionLimit '2' 客户超仓
        //// THOST_FTDC_FCC_MemberOverPositionLimit '3' 会员超仓
        //// THOST_FTDC_FCC_NotMultiple '4' 持仓非整数倍
        //// THOST_FTDC_FCC_Violation '5' 违规
        //// THOST_FTDC_FCC_Other '6' 其它
        //// THOST_FTDC_FCC_PersonDeliv '7' 自然人临近交割
        char ForceCloseReason = pInputOrder->ForceCloseReason;
        ///自动挂起标志 TThostFtdcBoolType int
        int IsAutoSuspend = pInputOrder->IsAutoSuspend;
        ///业务单元 TThostFtdcBusinessUnitType char[21]
        char BusinessUnit[63];
        gbk2utf8(pInputOrder->BusinessUnit,BusinessUnit,sizeof(BusinessUnit));
        ///请求编号 TThostFtdcRequestIDType int
        int RequestID = pInputOrder->RequestID;
        ///用户强评标志 TThostFtdcBoolType int
        int UserForceClose = pInputOrder->UserForceClose;
        ///互换单标志 TThostFtdcBoolType int
        int IsSwapOrder = pInputOrder->IsSwapOrder;

    }

    // 如果响应函数已经返回最后一条信息
    if(bIsLast) {
        // 通知主过程，响应函数将结束
        sem_post(&sem);
    }
}
   
void CTraderHandler::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    printf("OnRspSettlementInfoConfirm:called...\n");
    if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
        char ErrorMsg[243];
        gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
        printf("OnRspSettlementInfoConfirm:Error:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
        exit(-1);
    }

    if (pSettlementInfoConfirm != NULL) {
        char ConfirmDate[9];
        gbk2utf8(pSettlementInfoConfirm->ConfirmDate, ConfirmDate, sizeof(ConfirmDate));
        char ConfirmTime[9];
        gbk2utf8(pSettlementInfoConfirm->ConfirmTime, ConfirmTime, sizeof(ConfirmTime));
        int SettlementID;
        SettlementID = pSettlementInfoConfirm->SettlementID;

        printf("ConfirmDate=%s, ConfirmTime=%s, SettlementID=%i\n", ConfirmDate, ConfirmTime, SettlementID);
    }

    if (bIsLast) {
        sem_post(&sem);
    }
};

void CTraderHandler::OnRspOrderAction(CThostFtdcInputOrderActionField * pInputOrderAction, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast) {
    printf("OnRspOrderAction():called...\n");

    // 进程返回结果检查
    if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
        // typedef int TThostFtdcErrorIDType;
        // typedef char TThostFtdcErrorMsgType[81];
        char ErrorMsg[243];
        gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
        printf("OnRspOrderAction():Error:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
    }

    // 如果有返回结果读取返回信息
    if ( pInputOrderAction != NULL ) {
        // 读取返回信息,并做编码转化
        ///经纪公司代码 TThostFtdcBrokerIDType char[11]
        char BrokerID[33];
        gbk2utf8(pInputOrderAction->BrokerID,BrokerID,sizeof(BrokerID));
        ///投资者代码 TThostFtdcInvestorIDType char[13]
        char InvestorID[39];
        gbk2utf8(pInputOrderAction->InvestorID,InvestorID,sizeof(InvestorID));
        ///报单操作引用 TThostFtdcOrderActionRefType int
        int OrderActionRef = pInputOrderAction->OrderActionRef;
        ///报单引用 TThostFtdcOrderRefType char[13]
        char OrderRef[39];
        gbk2utf8(pInputOrderAction->OrderRef,OrderRef,sizeof(OrderRef));
        ///请求编号 TThostFtdcRequestIDType int
        int RequestID = pInputOrderAction->RequestID;
        ///前置编号 TThostFtdcFrontIDType int
        int FrontID = pInputOrderAction->FrontID;
        ///会话编号 TThostFtdcSessionIDType int
        int SessionID = pInputOrderAction->SessionID;
        ///交易所代码 TThostFtdcExchangeIDType char[9]
        char ExchangeID[27];
        gbk2utf8(pInputOrderAction->ExchangeID,ExchangeID,sizeof(ExchangeID));
        ///报单编号 TThostFtdcOrderSysIDType char[21]
        char OrderSysID[63];
        gbk2utf8(pInputOrderAction->OrderSysID,OrderSysID,sizeof(OrderSysID));
        ///操作标志 TThostFtdcActionFlagType char
        //// THOST_FTDC_AF_Delete '0' 删除
        //// THOST_FTDC_AF_Modify '3' 修改
        char ActionFlag = pInputOrderAction->ActionFlag;
        ///价格 TThostFtdcPriceType double
        double LimitPrice = pInputOrderAction->LimitPrice;
        ///数量变化 TThostFtdcVolumeType int
        int VolumeChange = pInputOrderAction->VolumeChange;
        ///用户代码 TThostFtdcUserIDType char[16]
        char UserID[48];
        gbk2utf8(pInputOrderAction->UserID,UserID,sizeof(UserID));
        ///合约代码 TThostFtdcInstrumentIDType char[31]
        char InstrumentID[93];
        gbk2utf8(pInputOrderAction->InstrumentID,InstrumentID,sizeof(InstrumentID));

        printf("InstrumentID=[%s] ActionFlag=[%c] LimitPrice=[%f] VolumeChange=[%i]\n", InstrumentID, ActionFlag, LimitPrice, VolumeChange);
    }

    // 如果响应函数已经返回最后一条信息
    if(bIsLast) {
        // 通知主过程，响应函数将结束
        sem_post(&sem);
    }
}

void CTraderHandler::OnRspQryOptionSelfClose(CThostFtdcOptionSelfCloseField *pOptionSelfClose, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    printf("OnRspQryOptionSelfClose():called...\n");

    // 进程返回结果检查
    if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
        char ErrorMsg[243];
        gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
        printf("OnRspQryOptionSelfClose():Error:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
    }

    // 如果有返回结果读取返回信息
    if ( pOptionSelfClose!= NULL ) {
        char InstrumentID[93];
        gbk2utf8(pOptionSelfClose->InstrumentID,InstrumentID,sizeof(InstrumentID));

        int Volume;
        Volume = pOptionSelfClose->Volume;

        char OrderSubmitStatus;
        OrderSubmitStatus = pOptionSelfClose->OrderSubmitStatus;

        printf("InstrumentID=[%s] Volume=[%i] OrderSubmitStatus=[%c] \n", InstrumentID, Volume, OrderSubmitStatus);
    }

    // 如果响应函数已经返回最后一条信息
    if(bIsLast) {
        // 通知主过程，响应函数将结束
        sem_post(&sem);
    }

};

void CTraderHandler::OnRspOptionSelfCloseInsert(CThostFtdcInputOptionSelfCloseField *pInputOptionSelfClose, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    printf("OnRspOptionSelfCloseInsert():called...\n");

    // 进程返回结果检查
    if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
        char ErrorMsg[243];
        gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
        printf("OnRspOptionSelfClose():Error:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
    }

    // 如果有返回结果读取返回信息
    if ( pInputOptionSelfClose != NULL ) {
        char InstrumentID[93];
        gbk2utf8(pInputOptionSelfClose->InstrumentID,InstrumentID,sizeof(InstrumentID));

        int Volume;
        Volume = pInputOptionSelfClose->Volume;

        printf("InstrumentID=[%s] Volume=[%i] \n", InstrumentID, Volume);
    }

    // 如果响应函数已经返回最后一条信息
    if(bIsLast) {
        // 通知主过程，响应函数将结束
        sem_post(&sem);
    }
};

void CTraderHandler::OnErrRtnOptionSelfCloseInsert(CThostFtdcInputOptionSelfCloseField *pInputOptionSelfClose, CThostFtdcRspInfoField *pRspInfo) {
    printf("OnErrRtnOptionSelfCloseInsertError:called\n");

    char ErrorMsg[243];
    gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
    printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, ErrorMsg);
};

void CTraderHandler::OnRtnOptionSelfClose(CThostFtdcOptionSelfCloseField *pOptionSelfClose) {
    printf("OnRtnOptionSelfClose:called\n");

    if (pOptionSelfClose != NULL) {
        char InstrumentID[93];
        gbk2utf8(pOptionSelfClose->InstrumentID,InstrumentID,sizeof(InstrumentID));
        int Volume;
        Volume = pOptionSelfClose->Volume;
        char HedgeFlag;
        HedgeFlag = pOptionSelfClose->HedgeFlag;
        char OptSelfCloseFlag;
        OptSelfCloseFlag = pOptionSelfClose->OptSelfCloseFlag;
        char ExecResult;
        ExecResult = pOptionSelfClose->ExecResult;
        char StatusMsg[81];
        gbk2utf8(pOptionSelfClose->StatusMsg,StatusMsg,sizeof(StatusMsg));

        printf("InstrumentID=[%s], Volume=[%i], HedgeFlag=[%c], OptSelfCloseFlag=[%c], ExecResult=[%c], StatusMsg=[%s]\n", 
            InstrumentID, Volume, HedgeFlag, OptSelfCloseFlag, ExecResult, StatusMsg);
    }

    sem_post(&sem);
};

void CTraderHandler::OnRspOptionSelfCloseAction(CThostFtdcInputOptionSelfCloseActionField *pInputOptionSelfCloseAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    printf("OnRspOptionSelfCloseAction():called...\n");

    // 进程返回结果检查
    if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
        // typedef int TThostFtdcErrorIDType;
        // typedef char TThostFtdcErrorMsgType[81];
        char ErrorMsg[243];
        gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
        printf("OnRspOptionSelfCloseAction():Error:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
    }

    // 如果有返回结果读取返回信息
    if ( pInputOptionSelfCloseAction!= NULL ) {
        char InstrumentID[93];
        gbk2utf8(pInputOptionSelfCloseAction->InstrumentID,InstrumentID,sizeof(InstrumentID));

        printf("InstrumentID=[%s] \n", InstrumentID);
    }

    // 如果响应函数已经返回最后一条信息
    if(bIsLast) {
        // 通知主过程，响应函数将结束
        sem_post(&sem);
    }

};

