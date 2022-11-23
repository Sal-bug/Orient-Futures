#include"CtpMdDemo.h"
#include"CtpTraderDemo.h"
int main() {
	
	//CtpMdDemo ctpMdDemo;
	//ctpMdDemo.Run();
	//CtpTraderDemo  ctpTraderDemo;
	//ctpTraderDemo.Run();
	const char  *inputParm[][8] = {
		//{ "tcp://180.166.103.34:51215","tcp://180.166.103.34:51218","6666","66617019","66617019","dz123456","client_wuroni_1.0.3","RW7QSCRUAJTHHHPS" },
		//{ "tcp://180.166.103.34:51215","tcp://180.166.103.34:51218","6666","66618192","66618192","dz123456","client_zhong_1.0.0","VBK5MY506A89PTT9" },
		//{ "tcp://180.166.103.34:51215","tcp://180.166.103.34:51218","6666","66616971","66616971","dz123456","client_ctpyy_1.0","M98C9M1PSFLBYIUG" },
		//{ "tcp://180.166.103.34:51215","tcp://180.166.103.34:51218","6666","66617226","66617226","dz123456","client_chentao_1.1c","L7NEZY2CHYCZ7FGV" },
		//{ "tcp://180.166.103.34:51215","tcp://180.166.103.34:51218","6666","66619719","66619719","dz123456","client_jmx2_1.0","LT1SFKR0UBSCQHGV" },
		//{ "tcp://180.166.103.34:51215","tcp://180.166.103.34:51218","6666","66618283","66618283","dz123456","client_winwin1_v1","V22VLKHKCDFLTW4B" }
		//{ "tcp://180.166.103.34:51215","tcp://180.166.103.34:51218","6666","66619860","66619860","dz123456","client_Cthunder11_v1.1","UX7BKION5VW8AJPD" },  
		//{ "tcp://180.166.103.34:51215","tcp://180.166.103.34:51218","6666","66618288","66618288","dz123456","client_flyfish1_1.0.0","L8PJJF1HM0G2KWTU" },
		//{ "tcp://180.166.103.34:51215","tcp://180.166.103.34:51218","6666","66619883","66619883","dz123456","client_fastone1_1.0","KY6ILKWC6WTGG3VQ" }
		//{ "tcp://180.166.103.34:51215","tcp://180.166.103.34:51218","6666","66619957","66619957","dz123456","client_newton1_0","LXYPE6VCUUU04XAM" }
		//{ "tcp://180.166.103.34:51215","tcp://180.166.103.34:51218","6666","66619983","66619983","dz123456","client_dzfhctp_1.0","PRZIH5PRHZK6WPUN" }
		{ "tcp://180.166.103.34:51215","tcp://180.166.103.34:51218","6666","66620070","66620070","dzqh1234","client_5y2_1.0.0","9TRZ1SV3WIJXDIR9" }
	};
	//printf("sizeof(inputParm)=%d\n", sizeof(inputParm)/sizeof(string[5]));
	CtpTraderDemo  *ctpTraderDemo= 0;
	CtpMdDemo *ctpMdDemo=0;

	for (int j = 0; j != sizeof(inputParm) / sizeof(char*[8]); ++j) {
		printf("start test of %s\n", inputParm[j][3]);
		ctpTraderDemo = new CtpTraderDemo();
		ctpMdDemo = new CtpMdDemo();
		ctpTraderDemo->InitParam(inputParm[j]);
		ctpMdDemo->InitParam(inputParm[j]);

		int i = 1;
		while (i) {
			printf("-------------------------0.exit----------------------------------------\n");
			printf("-------------------------1.ReqUserLogin--------------------------------\n");
			printf("-------------------------2.ReqUserLogout-------------------------------\n");
			printf("-------------------------3.ReqOrderInsert------------------------------\n");
			printf("-------------------------4.ReqOrderAction------------------------------\n");
			printf("-------------------------5.ReqQryInvestorPosition----------------------\n");
			printf("-------------------------6.ReqQryTradingAccount------------------------\n");
			printf("-------------------------91.MDReqUserLogin-----------------------------\n");
			printf("-------------------------92.MDReqUserLogout----------------------------\n");
			printf("-------------------------93.SubscribeMarketData------------------------\n");
			printf("-------------------------94.UnSubscribeMarketData----------------------\n");
			scanf("%d", &i);
			switch (i) {
			case 0: printf("0.exit\n"); break;
			case 1: printf("1.ReqUserLogin\n"); ctpTraderDemo->ReqUserLogin(); break;
			case 2: printf("2.ReqUserLogout\n"); ctpTraderDemo->ReqUserLogout(); break;
			case 3: printf("3.ReqOrderInsert\n"); ctpTraderDemo->ReqOrderInsert(); break;
			case 4: printf("4.ReqOrderAction\n"); ctpTraderDemo->ReqOrderAction(); break;
			case 5: printf("5.ReqQryInvestorPosition\n"); ctpTraderDemo->ReqQryInvestorPosition(); break;
			case 6: printf("6.ReqQryTradingAccount\n"); ctpTraderDemo->ReqQryTradingAccount(); break;
			case 91: printf("1.ReqUserLogin\n"); ctpMdDemo->ReqUserLogin(); break;
			case 92: printf("2.ReqUserLogout\n"); ctpMdDemo->ReqUserLogout(); break;
			case 93: printf("2.SubscribeMarketData\n"); ctpMdDemo->SubscribeMarketData(); break;
			case 94: printf("3.UnSubscribeMarketData\n"); ctpMdDemo->UnSubscribeMarketData(); break;
			default: printf("Invalid input\n"); break;
			}
		}
		delete ctpTraderDemo;
		delete ctpMdDemo;
		ctpTraderDemo = 0;
		ctpMdDemo = 0;
	}
	printf("main end\n");

	return 0;
}