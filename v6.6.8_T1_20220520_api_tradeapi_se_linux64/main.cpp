#include <iostream>
#include "ThostFtdcTraderApi.h"
#include <cstring>
using namespace std;

int main(int argc, char* argv[]) {
    char frontAddress[] = "tcp://180.168.146.187:10201";
    char BROKERID[] = "9999";
    char USERID[] = "9999";
    char PASSWORD[] = "9999";


    CThostFtdcMdApi* pUserApi = CThostFtdcMdApi::CreateFtdcMdApi("./", false, false);
    CThostFtdcMdSpi* pUserSpi = new CThostFtdcMdSpi();
    pUserApi->RegisterSpi(pUserSpi);
    pUserApi->RegisterFront(frontAddress);
    pUserApi->Init();
    pUserApi->Join();

    CThostFtdcRspUserLoginField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, BROKERID);
    strcpy(req.UserID, USERID);
    strcpy(req.Password, PASSWORD);



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
	printf("BrokerID:");
    scanf("%s", (char*) &userLoginField.BrokerID);
    
    printf("userid:");
    scanf("%s", (char*) &userLoginField.UserID);
        
    printf("password:");
    scanf("%s", (char*) &userLoginField.Password);
    
	// 发出登陆请求
	pTraderApi->ReqUserLogin(&userLoginField, 0);

	// 等待登录成功消息
	sem_wait(&sem);

	// 等待5秒钟
	sleep(5);
	printf("等待5秒完毕\n");

	// 拷贝用户登录信息到登出信息
	strcpy(userLogoutField.BrokerID,userLoginField.BrokerID);
	strcpy(userLogoutField.UserID, userLoginField.UserID);
	pTraderApi->ReqUserLogout(&userLogoutField, 1);

	// 等待登出成功
	sem_wait(&sem);
		
	
	printf("主线程执行完毕!\n");

}
