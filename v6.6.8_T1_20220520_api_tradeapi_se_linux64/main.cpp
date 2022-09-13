#include <iostream>
#include <cstring>

#include "ThostFtdcTraderApi.h"
#include "ThostFtdcMdApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

using namespace std;

sem_t sem;

class CTraderHandler : public CThostFtdcTraderSpi{

	public:

    CTraderHandler(){
        printf("CTraderHandler:called.\n");
    }

    virtual void OnFrontConnected() {
		static int i = 0;
		// 在登出后系统会重新调用OnFrontConnected，这里简单判断并忽略第1次之后的所有调用。
		if (i++ == 0) {
        	printf("OnFrontConnected:called.\n");
			sem_post(&sem);
		}
    }

    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
        printf("OnRspUserLogin:called\n");
		if (pRspInfo->ErrorID == 0) {
			printf("Login Success!\n");			
			sem_post(&sem);
		}
        else{
			printf("Login Failure!\n");
		}
    }

	// virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, 
	// 		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {	
	// 	printf("OnReqUserLogout:called\n");
	// 	if (pRspInfo->ErrorID == 0) {
	// 		printf("登出成功!\n");
	// 		sem_post(&sem);
	// 	}else{
	// 		printf("登出失败!\n");
	// 	}
	// }
};

int main(int argc, char* argv[]) {
    // 初始化变量
	sem_init(&sem,0,0);
    char * serverAddress = (char *)"tcp://180.168.146.187:10201";
    CThostFtdcReqUserLoginField userLoginField;

    // 创建TraderAPI和回调响应控制器的实例
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
    CTraderHandler traderHandler = CTraderHandler();
    CTraderHandler * pTraderHandler = &traderHandler;
    pTraderApi->RegisterSpi(pTraderHandler);

    // 设置服务器地址
    pTraderApi->RegisterFront(serverAddress);
    // 链接交易系统
    pTraderApi->Init();

	// 等待服务器发出登录消息
	sem_wait(&sem);
	
	// 读取登录信息
	strcpy(userLoginField.BrokerID, "9999");
    strcpy(userLoginField.UserID, "206045");
    scanf(userLoginField.Password, "Thzss200718");
    
	// 发出登陆请求
	pTraderApi->ReqUserLogin(&userLoginField, 0);

	// 等待登录成功消息
	sem_wait(&sem);

	// 等待5秒钟
	sleep(5);
	printf("等待5秒完毕\n");

	// // 拷贝用户登录信息到登出信息
	// strcpy(userLogoutField.BrokerID,userLoginField.BrokerID);
	// strcpy(userLogoutField.UserID, userLoginField.UserID);
	// pTraderApi->ReqUserLogout(&userLogoutField, 1);

	// // 等待登出成功
	// sem_wait(&sem);
	
	printf("主线程执行完毕!\n");
    return(0);

}
