#include "common.h"

using namespace std;

class CMdHandler : public CThostFtdcMdSpi {

	public:
    // vector<CThostFtdcDepthMarketDataField *> marketData;
    map<string, CThostFtdcDepthMarketDataField *> marketData;
    map<string, double> strikePrice;
    map<string, string> expireDate;

    double stockPrice;

    bool tick_change;

    CMdHandler(CThostFtdcMdApi *pMdApi);

    ~CMdHandler();

    void OnFrontConnected();

    void OnFrontDisconnected(int nReason);

    void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField * pDepthMarketData);

    private:

    CThostFtdcMdApi *m_pMdApi;

    
};