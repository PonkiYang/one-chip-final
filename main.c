#include<regx52.h>
#include"LCD1602.h"
#include"Delay.h"
#include"Key.h"

void Timer0_Init(){
	TMOD = 0x01;
	TF0 = 0;
	TR0 = 1;
	TH0 = 64536/256;
	TL0 = 64536%256;
	ET0 = 1;
	EA = 1;
	PT0 = 0;
}

int countDigits(int num) {  
    int count = 0;  
    while (num != 0) {  
        count++;  
        num /= 10;  
    }  
    return count;  
}  

void LCD_WriteDecimal(unsigned char Line,unsigned char Column, double num){
	
	int integerPart = (int)(num);
	double decimalPart = num - integerPart;  
	int len = countDigits(integerPart);
	unsigned char i;
	int j = 2;				// 	价格保留两位小数
	LCD_SetCursor(Line,Column);
	
	// 打印整数段
	for(i = len;i>0;i--){
		LCD_WriteData(integerPart/LCD_Pow(10,i-1)%10+'0');
	}
	
	// 打印小数点
	LCD_WriteData('.');	
	
	// 打印小数段
	while(j > 0){
		int digit = (int)(decimalPart * 10);
		LCD_WriteData(digit+'0');
		decimalPart = decimalPart*10 - digit;  
		j-- ;
	}
}



int i = 0;
double price;
int flag = 1;  				// 1为白班模式,0为夜班模式.
int run = 1;				// 1为行驶状态,0为停滞状态
int begun = 0;				// 1为已经启动，此时按K2按键不会增加起步费，0为还未启动，此时按K2按键会增加起步费。
int keyNum;					// 按键代数
int dist = 0;				// 路程
char Hour = 0, Sec = 0;		// 等待时间

void main(){
	LCD_Init();
	LCD_ShowString(1, 1, "Day");	// 默认为白班模式
	
	while(1){
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
				LCD_ShowString(1, 10, "run ");
			}
			begun = 1;
			Timer0_Init();
		}else if (keyNum == 3){		// 切换行驶/停滞状态
			run = 1 - run;
			if (run){
				LCD_ShowString(1, 10, "run ");
			} else {
				LCD_ShowString(1, 10, "stop");
			}
		}else if (keyNum == 4){		// 停止计费
			LCD_ShowString(1, 1, "Price: ");
			LCD_ShowString(1, 10, "    ");
			LCD_WriteDecimal(1, 8, price);
		}
		// 显示路程
		LCD_ShowNum(2, 1, dist)
		// 显示等待时间
	}
}


unsigned int T0Count;
void Timer0_Routine() interrupt 1{
	TH0 = 64536/256;
	TL0 = 64536%256;
	T0Count ++;

}

