#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "Serial.h"

char Serial1_RxPacket[100];				//����������ݰ�����
uint8_t Serial1_RxFlag;					//����������ݰ���־λ

char Serial2_RxPacket[100];              // ����������ݰ�����
uint8_t Serial2_RxFlag;                  // ����������ݰ���־λ

char Serial3_RxPacket[100];              // ����������ݰ�����
uint8_t Serial3_RxFlag;                  // ����������ݰ���־λ



/**
  * ��    �����η��������ڲ�ʹ�ã�
  * �� �� ֵ������ֵ����X��Y�η�
  */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//���ý����ֵΪ1
	while (Y --)			//ִ��Y��
	{
		Result *= X;		//��X�۳˵����
	}
	return Result;
}


/**
  * ��    ��������1��ʼ��
  * ��    ������
  * �� �� ֵ����
  */
void Serial1_Init(void)
{
	/*����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//����USART1��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//����GPIOA��ʱ��
	
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//��PA9���ų�ʼ��Ϊ�����������
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//��PA10���ų�ʼ��Ϊ��������
	
	/*USART��ʼ��*/
	USART_InitTypeDef USART_InitStructure;					//����ṹ�����
	USART_InitStructure.USART_BaudRate = UART1_BaudRate;				//������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//Ӳ�������ƣ�����Ҫ
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//ģʽ������ģʽ�ͽ���ģʽ��ѡ��
	USART_InitStructure.USART_Parity = USART_Parity_No;		//��żУ�飬����Ҫ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//ֹͣλ��ѡ��1λ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//�ֳ���ѡ��8λ
	USART_Init(USART1, &USART_InitStructure);				//���ṹ���������USART_Init������USART1
	
	/*�ж��������*/
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			//�������ڽ������ݵ��ж�
	
	/*NVIC�жϷ���*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			//����NVICΪ����2
	
	/*NVIC����*/
	NVIC_InitTypeDef NVIC_InitStructure;					//����ṹ�����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//ѡ������NVIC��USART1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//ָ��NVIC��·ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//ָ��NVIC��·����ռ���ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//ָ��NVIC��·����Ӧ���ȼ�Ϊ1
	NVIC_Init(&NVIC_InitStructure);							//���ṹ���������NVIC_Init������NVIC����
	
	/*USARTʹ��*/
	USART_Cmd(USART1, ENABLE);								//ʹ��USART1�����ڿ�ʼ����
}

/**
  * ��    ��������1����һ���ֽ�
  * ��    ����Byte Ҫ���͵�һ���ֽ�
  * �� �� ֵ����
  */
void Serial1_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);		//���ֽ�����д�����ݼĴ�����д���USART�Զ�����ʱ����
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//�ȴ��������
	/*�´�д�����ݼĴ������Զ����������ɱ�־λ���ʴ�ѭ�������������־λ*/
}

/**
  * ��    ��������1����һ������
  * ��    ����Array Ҫ����������׵�ַ
  * ��    ����Length Ҫ��������ĳ���
  * �� �� ֵ����
  */
void Serial1_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//��������
	{
		Serial1_SendByte(Array[i]);		//���ε���Serial_SendByte����ÿ���ֽ�����
	}
}

/**
  * ��    ��������1����һ���ַ���
  * ��    ����String Ҫ�����ַ������׵�ַ
  * �� �� ֵ����
  */
void Serial1_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//�����ַ����飨�ַ������������ַ���������־λ��ֹͣ
	{
		Serial1_SendByte(String[i]);		//���ε���Serial_SendByte����ÿ���ֽ�����
	}
}



/**
  * ��    ��������1��������
  * ��    ����Number Ҫ���͵����֣���Χ��0~4294967295
  * ��    ����Length Ҫ�������ֵĳ��ȣ���Χ��0~10
  * �� �� ֵ����
  */
void Serial1_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)		//�������ֳ��ȱ������ֵ�ÿһλ
	{
		Serial1_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');	//���ε���Serial_SendByte����ÿλ����
	}
}

/**
  * ��    ����ʹ��printf��Ҫ�ض���ĵײ㺯��
  * ��    ��������ԭʼ��ʽ���ɣ�����䶯
  * �� �� ֵ������ԭʼ��ʽ���ɣ�����䶯
  */
int fputc(int ch, FILE *f)
{
	Serial1_SendByte(ch);			//��printf�ĵײ��ض����Լ��ķ����ֽں���
	return ch;
}

/**
  * ��    �����Լ���װ��prinf����1
  * ��    ����format ��ʽ���ַ���
  * ��    ����... �ɱ�Ĳ����б�
  * �� �� ֵ����
  */
void Serial_Printf(char *format, ...)
{
	char String[100];				//�����ַ�����
	va_list arg;					//����ɱ�����б��������͵ı���arg
	va_start(arg, format);			//��format��ʼ�����ղ����б�arg����
	vsprintf(String, format, arg);	//ʹ��vsprintf��ӡ��ʽ���ַ����Ͳ����б��ַ�������
	va_end(arg);					//��������arg
	Serial1_SendString(String);		//���ڷ����ַ����飨�ַ�����
}

/**
  * ��    ����USART1�жϺ���
  * ��    ������
  * �� �� ֵ����
  * ע������˺���Ϊ�жϺ�����������ã��жϴ������Զ�ִ��
  *           ������ΪԤ����ָ�����ƣ����Դ������ļ�����
  *           ��ȷ����������ȷ���������κβ��죬�����жϺ��������ܽ���
  */
void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;		//�����ʾ��ǰ״̬��״̬�ľ�̬����
	static uint8_t pRxPacket = 0;	//�����ʾ��ǰ��������λ�õľ�̬����
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)	//�ж��Ƿ���USART1�Ľ����¼��������ж�
	{
		uint8_t RxData = USART_ReceiveData(USART1);			//��ȡ���ݼĴ���������ڽ��յ����ݱ���
		
		/*ʹ��״̬����˼·�����δ������ݰ��Ĳ�ͬ����*/
		
		/*��ǰ״̬Ϊ0���������ݰ���ͷ*/
		if (RxState == 0)
		{
			if (Serial1_RxFlag == 0)		//�������ȷʵ�ǰ�ͷ��������һ�����ݰ��Ѵ������
			{
				RxState = 1;			//����һ��״̬
				pRxPacket = 0;			//���ݰ���λ�ù���
				Serial1_RxPacket[pRxPacket] = RxData;		//�����ݴ������ݰ������ָ��λ��
				pRxPacket ++;			//���ݰ���λ������
			}
		}
		/*��ǰ״̬Ϊ1���������ݰ����ݣ�ͬʱ�ж��Ƿ���յ��˵�һ����β*/
		else if (RxState == 1)
		{
			if (RxData == '\r')			//����յ���һ����β
			{
				RxState = 2;			//����һ��״̬
			}
			else						//���յ�������������
			{
				Serial1_RxPacket[pRxPacket] = RxData;		//�����ݴ������ݰ������ָ��λ��
				pRxPacket ++;			//���ݰ���λ������
			}
		}
		/*��ǰ״̬Ϊ2���������ݰ��ڶ�����β*/
		else if (RxState == 2)
		{
			if (RxData == '\n')			//����յ��ڶ�����β
			{
				RxState = 0;			//״̬��0
				Serial1_RxPacket[pRxPacket] = '\0';			//���յ����ַ����ݰ����һ���ַ���������־
				Serial1_RxFlag = 1;		//�������ݰ���־λ��1���ɹ�����һ�����ݰ�
			}
		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);		//�����־λ
	}
}




/**
  * ��    ��������2��ʼ��
  * ��    ������
  * �� �� ֵ����
  */
void Serial2_Init(void)
{
    /* ����ʱ�� */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  // ����USART2��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   // ����GPIOA��ʱ��
    
    /* GPIO��ʼ�� */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  // ��PA2���ų�ʼ��Ϊ�����������
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  // ��PA3���ų�ʼ��Ϊ��������
    
    /* USART��ʼ�� */
    USART_InitTypeDef USART_InitStructure;                  // ����ṹ�����
    USART_InitStructure.USART_BaudRate = UART2_BaudRate;              // ������
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // Ӳ�������ƣ�����Ҫ
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  // ģʽ������ģʽ�ͽ���ģʽ��ѡ��
    USART_InitStructure.USART_Parity = USART_Parity_No;     // ��żУ�飬����Ҫ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  // ֹͣλ��ѡ��1λ
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  // �ֳ���ѡ��8λ
    USART_Init(USART2, &USART_InitStructure);               // ���ṹ���������USART_Init������USART2
    
    /* �ж�������� */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);          // �������ڽ������ݵ��ж�
    
    /* NVIC�жϷ��� */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);         // ����NVICΪ����2
    
    /* NVIC���� */
    NVIC_InitTypeDef NVIC_InitStructure;                    // ����ṹ�����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;       // ѡ������NVIC��USART2��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         // ָ��NVIC��·ʹ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // ָ��NVIC��·����ռ���ȼ�Ϊ1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;      // ָ��NVIC��·����Ӧ���ȼ�Ϊ1
    NVIC_Init(&NVIC_InitStructure);                         // ���ṹ���������NVIC_Init������NVIC����
    
    /* USARTʹ�� */
    USART_Cmd(USART2, ENABLE);                              // ʹ��USART2�����ڿ�ʼ����
}

/**
  * ��    ��������2����һ���ֽ�
  * ��    ����Byte Ҫ���͵�һ���ֽ�
  * �� �� ֵ����
  */
void Serial2_SendByte(uint8_t Byte)
{
    USART_SendData(USART2, Byte);        // ���ֽ�����д�����ݼĴ�����д���USART�Զ�����ʱ����
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);  // �ȴ��������
}


/**
  * ��    ��������2����һ������
  * ��    ����Array Ҫ����������׵�ַ
  * ��    ����Length Ҫ��������ĳ���
  * �� �� ֵ����
  */
void Serial2_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i ++)        // ��������
    {
        Serial2_SendByte(Array[i]);      // ���ε���Serial2_SendByte����ÿ���ֽ�����
    }
}

/**
  * ��    ��������2����һ���ַ���
  * ��    ����String Ҫ�����ַ������׵�ַ
  * �� �� ֵ����
  */
void Serial2_SendString(char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i ++)  // �����ַ����飨�ַ������������ַ���������־λ��ֹͣ
    {
        Serial2_SendByte(String[i]);      // ���ε���Serial2_SendByte����ÿ���ֽ�����
    }
}


void Serial2_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i ++)        // �������ֳ��ȱ������ֵ�ÿһλ
    {
        Serial2_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');  // ���ε���Serial2_SendByte����ÿλ����
    }
}


/**
  * ��    ����USART2�жϺ���
  * ��    ������
  * �� �� ֵ����
  * ע������˺���Ϊ�жϺ�����������ã��жϴ������Զ�ִ��
  *           ������ΪԤ����ָ�����ƣ����Դ������ļ�����
  *           ��ȷ����������ȷ���������κβ��죬�����жϺ��������ܽ���
  */
void USART2_IRQHandler(void)
{
    static uint8_t Rx2State = 0;     // �����ʾ��ǰ״̬��״̬�ľ�̬����
    static uint8_t pRx2Packet = 0;   // �����ʾ��ǰ��������λ�õľ�̬����
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)  // �ж��Ƿ���USART2�Ľ����¼��������ж�
    {
        uint8_t Rx2Data = USART_ReceiveData(USART2);      // ��ȡ���ݼĴ���������ڽ��յ����ݱ���
        
        if (Rx2State == 0)
        {
            if (Serial2_RxFlag == 0)    // �������ȷʵ�ǰ�ͷ��������һ�����ݰ��Ѵ������
            {
                Rx2State = 1;            // ����һ��״̬
                pRx2Packet = 0;          // ���ݰ���λ�ù���
                Serial2_RxPacket[pRx2Packet] = Rx2Data;  // �����ݴ������ݰ������ָ��λ��
                pRx2Packet ++;           // ���ݰ���λ������
            }
        }
        else if (Rx2State == 1)
        {
            if (Rx2Data == '\r')         // ����յ���һ����β
            {
                Rx2State = 2;            // ����һ��״̬
            }
            else                        // ���յ�������������
            {
                Serial2_RxPacket[pRx2Packet] = Rx2Data;  // �����ݴ������ݰ������ָ��λ��
                pRx2Packet ++;           // ���ݰ���λ������
            }
        }
        else if (Rx2State == 2)
        {
            if (Rx2Data == '\n')         // ����յ��ڶ�����β
            {
                Rx2State = 0;            // ״̬��0
                Serial2_RxPacket[pRx2Packet] = '\0';     // ���յ����ַ����ݰ����һ���ַ���������־
                Serial2_RxFlag = 1;      // �������ݰ���־λ��1���ɹ�����һ�����ݰ�
            }
        }
        
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);  // �����־λ
    }
}


/**
  * ��    ��������3��ʼ��
  * ��    ������
  * �� �� ֵ����
  */
void Serial3_Init(void)
{
    /* ����ʱ�� */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);  // ����USART3��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   // ����GPIOB��ʱ��
    
    /* GPIO��ʼ�� */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);                  // ��PA2���ų�ʼ��Ϊ�����������
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);                  // ��PA3���ų�ʼ��Ϊ��������
    
    /* USART��ʼ�� */
    USART_InitTypeDef USART_InitStructure;                  // ����ṹ�����
    USART_InitStructure.USART_BaudRate = UART3_BaudRate;              // ������
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // Ӳ�������ƣ�����Ҫ
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  // ģʽ������ģʽ�ͽ���ģʽ��ѡ��
    USART_InitStructure.USART_Parity = USART_Parity_No;     // ��żУ�飬����Ҫ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  // ֹͣλ��ѡ��1λ
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  // �ֳ���ѡ��8λ
    USART_Init(USART3, &USART_InitStructure);               // ���ṹ���������USART_Init������USART3
    
    /* �ж�������� */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);          // �������ڽ������ݵ��ж�
    
    /* NVIC�жϷ��� */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);         // ����NVICΪ����3
    
    /* NVIC���� */
    NVIC_InitTypeDef NVIC_InitStructure;                    // ����ṹ�����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;       // ѡ������NVIC��USART2��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         // ָ��NVIC��·ʹ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // ָ��NVIC��·����ռ���ȼ�Ϊ1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;      // ָ��NVIC��·����Ӧ���ȼ�Ϊ1
    NVIC_Init(&NVIC_InitStructure);                         // ���ṹ���������NVIC_Init������NVIC����
    
    /* USARTʹ�� */
    USART_Cmd(USART3, ENABLE);                              // ʹ��USART3�����ڿ�ʼ����
}


/**
  * ��    ��������3����һ���ֽ�
  * ��    ����Byte Ҫ���͵�һ���ֽ�
  * �� �� ֵ����
  */
void Serial3_SendByte(uint8_t Byte)
{
    USART_SendData(USART3, Byte);        // ���ֽ�����д�����ݼĴ�����д���USART�Զ�����ʱ����
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);  // �ȴ��������
}

/**
  * ��    ��������3����һ������
  * ��    ����Array Ҫ����������׵�ַ
  * ��    ����Length Ҫ��������ĳ���
  * �� �� ֵ����
  */
void Serial3_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i ++)        // ��������
    {
        Serial3_SendByte(Array[i]);      // ���ε���Serial3_SendByte����ÿ���ֽ�����
    }
}

/**
  * ��    ��������3����һ���ַ���
  * ��    ����String Ҫ�����ַ������׵�ַ
  * �� �� ֵ����
  */
void Serial3_SendString(char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i ++)  // �����ַ����飨�ַ������������ַ���������־λ��ֹͣ
    {
        Serial3_SendByte(String[i]);      // ���ε���Serial3_SendByte����ÿ���ֽ�����
    }
}



void Serial3_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i ++)        // �������ֳ��ȱ������ֵ�ÿһλ
    {
        Serial3_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');  // ���ε���Serial3_SendByte����ÿλ����
    }
}


/**
  * ��    ����USART3�жϺ���
  * ��    ������
  * �� �� ֵ����
  * ע������˺���Ϊ�жϺ�����������ã��жϴ������Զ�ִ��
  *           ������ΪԤ����ָ�����ƣ����Դ������ļ�����
  *           ��ȷ����������ȷ���������κβ��죬�����жϺ��������ܽ���
  */
void USART3_IRQHandler(void)
{
    static uint8_t Rx3State = 0;     // �����ʾ��ǰ״̬��״̬�ľ�̬����
    static uint8_t pRx3Packet = 0;   // �����ʾ��ǰ��������λ�õľ�̬����
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)  // �ж��Ƿ���USART3�Ľ����¼��������ж�
    {
        uint8_t Rx3Data = USART_ReceiveData(USART3);      // ��ȡ���ݼĴ���������ڽ��յ����ݱ���
        
        if (Rx3State == 0)
        {
            if (Serial3_RxFlag == 0)    // �������ȷʵ�ǰ�ͷ��������һ�����ݰ��Ѵ������
            {
                Rx3State = 1;            // ����һ��״̬
                pRx3Packet = 0;          // ���ݰ���λ�ù���
                Serial3_RxPacket[pRx3Packet] = Rx3Data;  // �����ݴ������ݰ������ָ��λ��
                pRx3Packet ++;           // ���ݰ���λ������
            }
        }
        else if (Rx3State == 1)
        {
            if (Rx3Data == '\r')         // ����յ���һ����β
            {
                Rx3State = 2;            // ����һ��״̬
            }
            else                        // ���յ�������������
            {
                Serial3_RxPacket[pRx3Packet] = Rx3Data;  // �����ݴ������ݰ������ָ��λ��
                pRx3Packet ++;           // ���ݰ���λ������
            }
        }
        else if (Rx3State == 2)
        {
            if (Rx3Data == '\n')         // ����յ��ڶ�����β
            {
                Rx3State = 0;            // ״̬��0
                Serial3_RxPacket[pRx3Packet] = '\0';     // ���յ����ַ����ݰ����һ���ַ���������־
                Serial3_RxFlag = 1;      // �������ݰ���־λ��1���ɹ�����һ�����ݰ�
            }
        }
        
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);  // �����־λ
    }
}





