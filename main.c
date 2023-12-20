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
int i = 0;

void main(){
	LCD_Init();
	// LCD_ShowChar(1, 1, 'A');
	// LCD_ShowString(1, 1, "Day shift mode");
	// LCD_ShowString(1, 1, "Night shift mode");
	Timer0_Init();
	LCD_ShowString(2, 1, "Go!  ");
	//while(1){}
	while(1){
		Delay(1000);
		i += 1;
		LCD_ShowNum(1, 1, i, 3);
	}
}

unsigned int T0Count;
void Timer0_Routine() interrupt 1{
	TH0 = 64536/256;
	TL0 = 64536%256;
	T0Count ++;
	if (T0Count >= 3000){
		T0Count = 0;
		LCD_ShowString(2, 1, "STOP!");
		Delay(3000);
		LCD_ShowString(2, 1, "Go!  ");
	}
}