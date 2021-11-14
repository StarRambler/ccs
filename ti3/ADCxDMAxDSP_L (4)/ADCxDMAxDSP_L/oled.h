#ifndef __OLED_H
#define __OLED_H

#include "ti/devices/msp432e4/driverlib/driverlib.h"
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char

#define OLED_SCLK_Clr() GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0); //SCL
#define OLED_SCLK_Set() GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 1); //SCL

#define OLED_SDIN_Clr() GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0, 0);
#define OLED_SDIN_Set() GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0, 1);

#define OLED_CMD  0 //写命令
#define OLED_DATA 1 //写数据

void OLED_ClearPoint(u8 x,u8 y);
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_WaitAck(void);
void Send_Byte(u8 dat);
void OLED_WR_Byte(u8 dat,u8 mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y);
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2);
void OLED_DrawCircle(u8 x,u8 y,u8 r);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1);
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1);
void OLED_ScrollDisplay(u8 num,u8 space);
void OLED_WR_BP(u8 x,u8 y);
void OLED_ShowPicture(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[]);
void OLED_Init(void);
void OLED_Showdecimal(u8 x,u8 y,float num,u8 z_len,u8 f_len,u8 size2);   //z_len为整数显示位数，f_len为小数显示位数，size2为字体大小
void Wave_Display(u8 wave[128]);            //输入128个点  范围0-64
void Thd_Display(float thd);       //输入THD,显示百分数
void Amplitude_Display(float a,float b,float c, float d, float e,float emo,float rms);       //输入幅值 基波 & 谐波*4*/

#endif
