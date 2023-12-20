#include <regx52.h>
#include "LCD1602.h"
#include "Key.h"
#include "Timer.h"

int i = 0;
double price;
int flag = 1;  							// 1为白班模式,0为夜班模式.
int run = 1;							// 1为行驶状态,0为停滞状态
int begun = 0;							// 1为已经启动，此时按K2按键不会增加起步费，0为还未启动，此时按K2按键会增加起步费。
int keyNum;								// 按键代数
int dist = 0;							// 路程
char Hour = 0, Sec = 0, Min = 0;		// 等待时间
int Mins = 0;							// 总等待时间（单位为分钟）
unsigned int runSec = 0, runMin = 0;	// 行驶时间（单位为分钟）
char s10 = 0;							// 表示10秒过没过，为0则没过，为1则过了，用于while(1){}中最后一个判断

void main(){
	LCD_Init();
	Timer0Init();					// 定时器0计算等待时间
	Timer1Init();					// 定时器1计算行驶时间
	LCD_ShowString(1, 1, "Day");	// 默认为白班模式
	LCD_ShowString(2, 5,"km");
	LCD_ShowString(2, 11,":  :  ");
	while(1){
		// 显示路程
		LCD_ShowNum(2, 1, dist, 4);
		// 显示等待时间
		LCD_ShowNum(2, 9, Hour,2);
		LCD_ShowNum(2, 12, Min, 2);
		LCD_ShowNum(2, 15, Sec, 2);
		keyNum = Key();
		if (keyNum == 1){			// 切换黑/白班模式
			flag = 1 - flag;
			if(flag){
				LCD_ShowString(1, 1, "Day  ");
			} else {
				LCD_ShowString(1, 1, "Night");
			}
		}else if (keyNum == 2){		// 开始计费、启动
			if(begun == 0){
				if (flag){
					price += 10.0;
				} else {
					price += 11.0;
				}
				LCD_ShowString(1, 9, "stop");
			}
			begun = 1;
			TR0 = 1;				// 打开定时器0,计算等待时间
			

		}else if (keyNum == 3){		// 切换行驶/停滞状态
			run = 1 - run;
			if(TR0 == 0){
				TR0 = 1;
				TR1 = 0;
			} else {
				TR0 = 0;
				TR1 = 1;
			}
			if (run){
				LCD_ShowString(1, 9, "run ");
			} else {
				LCD_ShowString(1, 9, "stop");
			}
		}else if (keyNum == 4){		// 停止计费
			LCD_ShowString(1, 1, "Price: ");
			LCD_ShowString(1, 10, "    ");
			LCD_WriteDecimal(1, 9, price);
		}
		if(Sec == 10 && s10 == 0){	// 计时开始十秒内为等待时间，10秒时切换状态;   与上后面的判断的意义是：如果只有前面的判断，那么TR1开启的情况下，Sec永远不变，永远是10，那么按键K3失去意义
			s10 = 1;				// 因为，每次按完K3切换完模式后，运行到这里又会切换回去
			TR0 = 0;				// 关闭定时器0
			TR1 = 1;				// 打开定时器1
			LCD_ShowString(1, 9, "run ");
		}
	}
}



void Timer0_Routine() interrupt 1{
	static unsigned int T0Count;
	TL0 = 0x66;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	T0Count++;
	if(T0Count>=1000)	//定时器分频，1s
	{
		T0Count=0;
		Sec++;			//1秒到，Sec自增
		if(Sec>=60)
		{
			Sec=0;		//60秒到，Sec清0，Min自增
			Min++;
			if(flag){
				price += 2.0;		// 白班等待1分钟2块钱
			} else {
				price += 2.5;	// 夜班等待1分钟2.5块钱
			}
			if(Min>=60)
			{
				Min=0;	//60分钟到，Min清0，Hour自增
				Hour++;
				if(Hour>=24)
				{
					Hour=0;	//24小时到，Hour清0
				}
			}
		}
	}
	
}

void Timer1_Routine() interrupt 3{
	static unsigned int T1Count;
	TL1 = 0x66;		//设置定时初值
	TH1 = 0xFC;		//设置定时初值
	T1Count++;
	if(T1Count>=1000)	//定时器分频，1s
	{
		T1Count=0;
		runSec++;			//1秒到，runSec自增
		if(runSec>=60)
		{
			runSec=0;			// 60秒到，runSec清0，runMin自增
			runMin++;			// 60秒->1分钟，单纯放在这，看得更明白些
			dist ++;			// 1分钟->1公里（1小时60公里）
			if(flag){
				price += 2.0;		// 白班1公里2块钱
			} else {
				price += 2.5;	// 夜班1公里2.5块钱
			}
		}
	}
}