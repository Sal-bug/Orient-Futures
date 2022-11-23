#include "common.h"

class CTraderHandler : public CThostFtdcTraderSpi{

	public:
    map<string, CThostFtdcInvestorPositionField *> positionLongData;
    map<string, CThostFtdcInvestorPositionField *> positionShortData;

    CTraderHandler(CThostFtdcTraderApi *pTraderApi);

    ~CTraderHandler();

    void OnFrontConnected();

    void OnFrontDisconnected(int nReason);

    void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    void OnRspQryInstrument(CThostFtdcInstrumentField * pInstrument, 
        CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast);

    void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    void OnRspQryOrder(CThostFtdcOrderField * pOrder, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast);

    void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField * pDepthMarketData, 
        CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast);

    void OnRtnOrder(CThostFtdcOrderField * pOrder);

    void OnErrRtnOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo);

    void OnRtnTrade(CThostFtdcTradeField * pTrade);

    void OnRspOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo,int nRequestID, bool bIsLast);

    void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    
    void OnRspOrderAction(CThostFtdcInputOrderActionField * pInputOrderAction, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast);

    void OnRspQryOptionSelfClose(CThostFtdcOptionSelfCloseField *pOptionSelfClose, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    void OnRspOptionSelfCloseInsert(CThostFtdcInputOptionSelfCloseField *pInputOptionSelfClose, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    void OnErrRtnOptionSelfCloseInsert(CThostFtdcInputOptionSelfCloseField *pInputOptionSelfClose, CThostFtdcRspInfoField *pRspInfo);

    void OnRtnOptionSelfClose(CThostFtdcOptionSelfCloseField *pOptionSelfClose);

    void OnRspOptionSelfCloseAction(CThostFtdcInputOptionSelfCloseActionField *pInputOptionSelfCloseAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    private:
    CThostFtdcTraderApi *m_pTraderApi;
};
