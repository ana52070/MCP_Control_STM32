#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Key.h"
#include "Timer.h"
#include "Serial.h"
#include <string.h>

int main(void)
{
	OLED_Init();
	LED_Init();
	Serial1_Init();
	OLED_ShowString(1,1,"Serial:");
	while (1)
	{
		if(Serial1_RxFlag == 1)
		{
			if(strstr((const char *)Serial1_RxPacket, "led_on")!= NULL)
			{
				LED1_ON();
			}
			else if(strstr((const char *)Serial1_RxPacket, "led_off")!= NULL)
			{
				LED1_OFF();
			}
			OLED_ShowString(2,1,"                   ");
			OLED_ShowString(2,1,Serial1_RxPacket);
			Serial1_RxFlag = 0;
		}

	}
}





//10ms¶¨Ê±ÖÐ¶Ï
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
