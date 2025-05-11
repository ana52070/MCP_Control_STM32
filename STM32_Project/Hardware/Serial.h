#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"                  // Device header
#include <stdio.h>


#define UART1_BaudRate	115200
#define UART2_BaudRate	115200
#define UART3_BaudRate	115200


//串口1函数声明部分
extern char Serial1_RxPacket[];
extern uint8_t Serial1_RxFlag;

void Serial1_Init(void);
void Serial1_SendByte(uint8_t Byte);
void Serial1_SendArray(uint8_t *Array, uint16_t Length);
void Serial1_SendString(char *String);
void Serial1_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);


//串口2函数声明部分
extern char Serial2_RxPacket[];  
extern uint8_t Serial2_RxFlag;   

void Serial2_Init(void);
void Serial2_SendByte(uint8_t Byte);            
void Serial2_SendArray(uint8_t *Array, uint16_t Length); 
void Serial2_SendString(char *String);          
void Serial2_SendNumber(uint32_t Number, uint8_t Length); 


//串口3函数声明部分
extern char Serial3_RxPacket[];  
extern uint8_t Serial3_RxFlag;   

void Serial3_Init(void);
void Serial3_SendByte(uint8_t Byte); 
void Serial3_SendArray(uint8_t *Array, uint16_t Length);
void Serial3_SendString(char *String); 
void Serial3_SendNumber(uint32_t Number, uint8_t Length); 

void Serial_Init(uint8_t Serialx);
  

#endif
