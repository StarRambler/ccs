#include <oled.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <UART.h>

/* Display Include via console */
#include "uartstdio.h"
#include "oled.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "math.h"
/* 定义抽样长度和采样频率 */
//1024个点的采样点数
uint32_t SAMP_FREQ = 250000;//100KHz的采样频率
uint32_t mean = 0;
//#define SAMP_FREQ 250000
arm_cfft_radix4_instance_f32 scfft;     /*参数存储指针*/

/********************************************/
#define NUM_SAMPLES 1024
float32_t dis_freq;
float32_t sum;
float32_t INPUT[1024];
float32_t FFT_INPUT[2048];
float32_t   THD,
            emo,
            THDx = 0.000f;
            MAX = 0,
            MAX1=0,MAX2=0,MAX3=0,MAX4=0;
int         num,num1,num2,num3,num4,heng,heng1,heng2,heng3,heng4,heng_Fre,MAX_Fre = 0;
unsigned char turn = 0,emp = 1,Flag_A = 0;
unsigned char Fre_Flag = 0,
              S_Flag = 0;
//                S_Flag = 0;
float rms;
int k;
int maxxxx,minnnn,delta;
float32_t Amp[5] = {0};
uint8_t Re_Appear[240];
/********************************************/
/* DMA Buffer declaration and buffer complet flag */
static  uint32_t dstBufferA[NUM_SAMPLES];
static uint32_t dstBufferB[NUM_SAMPLES];
volatile bool setBufAReady = false;
volatile bool setBufBReady = false;

//float32_t Re_Appear[256] = {};


/* Global variables and defines for FFT */
#define IFFTFLAG   0
#define BITREVERSE 1
//volatile int16_t fftOutput[NUM_SAMPLES*2];
float32_t fftOutput[NUM_SAMPLES*2];
/* Global variables for RMS and DC calculation */
volatile float32_t rmsBuff;
volatile float32_t dcBuff;
float32_t rmsCalculation;

/* The control table used by the uDMA controller.  This table must be aligned
 * to a 1024 byte boundary. */
#if defined(__ICCARM__)
#pragma data_alignment=1024
uint8_t pui8ControlTable[1024];
#elif defined(__TI_ARM__)
#pragma DATA_ALIGN(pui8ControlTable, 1024)
uint8_t pui8ControlTable[1024];
#else
uint8_t pui8ControlTable[1024] __attribute__ ((aligned(1024)));
#endif

void bubble_sort(float32_t arr[], int len)
{
        int i, j, temp;
        for (i = 0; i < len - 1; i++)
                for (j = 0; j < len - 1 - i; j++)
                        if (arr[j] > arr[j + 1]) {
                                temp = arr[j];
                                arr[j] = arr[j + 1];
                                arr[j + 1] = temp;
                        }
}

void f_char_printf(float Xangle)
{
float temp = Xangle;

if(Xangle>=0){
UARTprintf("%d.%d",(int32_t)temp ,(int32_t)((temp -(int32_t)temp )*1000));

}

else{

temp = 0-temp;

UARTprintf("-%d.%d",(int32_t)temp ,(int32_t)((temp -(int32_t)temp )*1000));

}

}


void display_wave(void)/*画图函数*/
{
    int i;

            for(i = 0;i<=20;i++)
            {
        //         Re_Appear[i] =  (u8)MAX/14041*5  *( sin(6.28  / 240 * i))+
        //                         (u8)MAX1/14041*5 * (sin(12.56 / 240 * i))+
        //                         (u8)MAX2/14041*5 *( sin(18.84 / 240 * i))+
         //                        (u8)MAX3/14041*5 *( sin(25.12 / 240 * i))+
         //                        (u8)MAX4/14041*5 *( sin(31.4  / 240 * i))+10;
                Re_Appear[i]=dstBufferA[300+i]+delta-mean;

            }

            Wave_Display(Re_Appear);



}


///*小oled*/
//void display_wave_2(void)
//{
//    int i;
//
//    for(i = 0 ; i < 120 ;i++)
//    {
//
//    }
//}


int UART_LY(void)
{
    uint32_t systemClock;


    //double lzd[8]={0.48,0.51,0.01,0.48,86.26,0,0,0};

    /* Configure the system clock for 120 MHz */
    systemClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                          SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                                          120000000);

    /* Enable the clock to the GPIO Port A and wait for it to be ready */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    while(!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC)))
    {

    }

    /* Configure the GPIO PA0-1 as U0RX-U0TX */
    MAP_GPIOPinConfigure(GPIO_PC4_U7RX);
    MAP_GPIOPinConfigure(GPIO_PC5_U7TX);
    MAP_GPIOPinTypeUART(GPIO_PORTC_BASE, (GPIO_PIN_4 | GPIO_PIN_5));

    /* Enable the clock to the UART-0 and wait for it to be ready */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
    while(!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART7)))
    {
    }

    /* Configure UART for 115200 bps, 8-N-1 format. Enable the DMA Request
     * generation and interrupt on DMARX Done. Also make sure that the FIFO is
     * disabled. */
    MAP_UARTConfigSetExpClk(UART7_BASE, systemClock, 9600,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE |
                             UART_CONFIG_STOP_ONE));
    MAP_UARTDMAEnable(UART7_BASE, UART_DMA_RX);
    MAP_UARTIntEnable(UART7_BASE, UART_INT_DMARX);
    MAP_UARTEnable(UART7_BASE);
    MAP_UARTFIFODisable(UART7_BASE);

    MAP_IntEnable(INT_UART3);

    /* Enable the DMA and Configure Channel for UART0 RX for Basic mode of
     * transfer */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    while(!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UDMA)))
    {
    }

    MAP_uDMAEnable();

    /* Point at the control table to use for channel control structures. */
    MAP_uDMAControlBaseSet(pui8ControlTable);

    /* Map the UART0 RX DMA channel */
    MAP_uDMAChannelAssign(UDMA_CH20_UART7RX);

    /* Put the attributes in a known state for the uDMA UART0 RX channel.
     * These should already be disabled by default. */
    MAP_uDMAChannelAttributeDisable(UDMA_CH20_UART7RX,
                                    UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
                                    UDMA_ATTR_HIGH_PRIORITY |
                                    UDMA_ATTR_REQMASK);

    /* Configure the control parameters for the primary control structure for
     * the UART0 RX channel. The primary control structure is used for copying
     * the data from srcBuffer to UART0 Data register. The transfer data size
     * is 32 bits and the source & destination address are not incremented. */
    MAP_uDMAChannelControlSet(UDMA_CH20_UART7RX | UDMA_PRI_SELECT,
                              UDMA_SIZE_32 | UDMA_SRC_INC_NONE | UDMA_DST_INC_NONE |
                              UDMA_ARB_1);

    /* Set up the transfer parameters for the UART0 RX primary control
     * structure. The mode is Ping-Pong mode so it will run to completion. */
    MAP_uDMAChannelTransferSet(UDMA_CH20_UART7RX | UDMA_PRI_SELECT,
                               UDMA_MODE_BASIC,
                               (void *)&UART7->DR, (void *)&UART7->DR,
                               100);

    /* The uDMA UART0 RX channel is primed to start a transfer. As soon as
     * the channel is enabled and the UART0 receives a byte of data it will
     * issue a DMA Request, and data transfers will begin. */
    MAP_uDMAChannelEnable(UDMA_CH20_UART7RX);




}
void uart_output()
{int dic ;
    char a='b';
      char b='c';
    printf ("%c ",a);

for (dic=0;dic<4;dic++)
{
  printf("%f ",Amp[dic]);
}

printf("%f ",THDx);

printf ("%c ",b);

}





int qwe;

//ADC0中断服务函数
void ADC0SS2_IRQHandler(void)
{
    uint32_t getIntStatus;
    uint32_t getDMAStatus;
    uint32_t i;
    qwe++;

      int k ;
      char a='b';
      char b='c';
      float lzd[8]={0.48,0.51,0.01,0.48,86.260000,0,0,0};

//    MAP_ADCSequenceDMADisable(ADC0_BASE, 2);
//    MAP_ADCSequenceDisable(ADC0_BASE, 2);
//    MAP_TimerDisable(TIMER0_BASE, TIMER_A);
//
//    MAP_uDMAChannelDisable(UDMA_CH16_ADC0_2);




        /* Get the interrupt status from the ADC */
        getIntStatus = MAP_ADCIntStatusEx(ADC0_BASE, true);

        /* Clear the ADC interrupt flag. */
        MAP_ADCIntClearEx(ADC0_BASE, getIntStatus);

//        MAP_ADCSequenceDisable(ADC0_BASE, 2);
//        MAP_TimerDisable(TIMER0_BASE, TIMER_A);




        /* Read the primary and alternate control structures to find out which
         * of the structure has completed and generated the done interrupt. Then
         * re-initialize the appropriate structure */
        getDMAStatus = MAP_uDMAChannelModeGet(UDMA_CH16_ADC0_2 |
                                              UDMA_PRI_SELECT);

        /* Check if the primary or alternate channel has completed. On completion
         * re-initalize the channel control structure. If the Primary channel has
         * completed then set Buffer-A ready flag so that the main application
         * may perform the DSP computation. Similarly if the Alternate channel
         * has completed then set Buffer-B ready flag so that the main application
         * may perform the DSP computation. */
        if(getDMAStatus == UDMA_MODE_STOP)
        {
            MAP_ADCSequenceDisable(ADC0_BASE, 2);
            MAP_TimerDisable(TIMER0_BASE, TIMER_A);



            MAP_uDMAChannelTransferSet(UDMA_CH16_ADC0_2 | UDMA_PRI_SELECT,
                                       UDMA_MODE_BASIC,
                                       (void *)&ADC0->SSFIFO2, (void *)&dstBufferA,
                                       sizeof(dstBufferA)/4);
            setBufAReady = true;

            while(!setBufAReady);
            setBufAReady = false;
            dcBuff       = 0.0f;
            rmsBuff      = 0.0f;//均方根


//            MAP_ADCSequenceDMADisable(ADC0_BASE, 2);
////            MAP_ADCSequenceDisable(ADC0_BASE, 2);
////            MAP_TimerDisable(TIMER0_BASE, TIMER_A);
//
//            MAP_uDMAChannelDisable(UDMA_CH16_ADC0_2);




            S_Flag = 1;


    }


       MAP_uDMAChannelEnable(UDMA_CH16_ADC0_2);

}

void ConfigureUART(uint32_t systemClock)
{
    /* Enable the clock to GPIO port A and UART 0 */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    /* Configure the GPIO Port A for UART 0 */
    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Configure the UART for 115200 bps 8-N-1 format */
    UARTStdioConfig(0, 115200, systemClock);
}

int main(void)
{
    uint32_t systemClock;//时钟主频率
    uint32_t ii;
    uint32_t setFFTmaxValue;                //FFT最大电压值
    uint32_t setFFTmaxFreqIndex;            //FFT最大频率值
    int_fast32_t i32IPart[3];               //整型数据数组    0:均方根 1:电压平均值 2:频率
    int_fast32_t i32FPart[3];               //浮点型数据数组   0:均方根 1:电压平均值 2:频率

    /* 时钟周期设置为 120 MHz */
    systemClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                          SYSCTL_OSC_MAIN |
                                          SYSCTL_USE_PLL |
                                          SYSCTL_CFG_VCO_480), 120000000);

    /* Initialize serial console */
    ConfigureUART(systemClock);


    //UART_LY();

    //GPIO和ADC0使能部分

    //使能GPIOE3进行AD采样
    /* Enable the clock to GPIO Port E and wait for it to be ready */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while(!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)))
    {
    }

    /* Configure PE3 as ADC input channel */
    MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

    /* Enable the clock to ADC-0 and wait for it to be ready */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while(!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)))
    {
    }

    //将PE3和ADC0通道AIN0连接
    /* Configure Sequencer 2 to sample the analog channel : AIN0. The end of
     * conversion and interrupt generation is set for AIN0 */
    MAP_ADCSequenceStepConfigure(ADC0_BASE, 2, 0, ADC_CTL_CH0 | ADC_CTL_IE |
                                 ADC_CTL_END);

    /* Enable sample sequence 2 with a timer signal trigger.  Sequencer 2
     * will do a single sample when the timer generates a trigger on timeout*/
    MAP_ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_TIMER, 2);           //定时器触发

    /* Clear the interrupt status flag before enabling. This is done to make
     * sure the interrupt flag is cleared before we sample. */
    MAP_ADCIntClearEx(ADC0_BASE, ADC_INT_DMA_SS2);
    MAP_ADCIntEnableEx(ADC0_BASE, ADC_INT_DMA_SS2);             //使能之前清空中断标志位

    /* Enable the DMA request from ADC0 Sequencer 2 */
    MAP_ADCSequenceDMAEnable(ADC0_BASE, 2);

    /* Since sample sequence 2 is now configured, it must be enabled. */
    MAP_ADCSequenceEnable(ADC0_BASE, 2);

    /* Enable the Interrupt generation from the ADC-0 Sequencer */
    MAP_IntEnable(INT_ADC0SS2);



    //DMA使能部分

    /* Enable the DMA and Configure Channel for TIMER0A for Ping Pong mode of
     * transfer */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    while(!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UDMA)))
    {
    }

    MAP_uDMAEnable();

    /* Point at the control table to use for channel control structures. */
    MAP_uDMAControlBaseSet(pui8ControlTable);

    /* Map the ADC0 Sequencer 2 DMA channel */
    MAP_uDMAChannelAssign(UDMA_CH16_ADC0_2);

    /* Put the attributes in a known state for the uDMA ADC0 Sequencer 2
     * channel. These should already be disabled by default. */
    MAP_uDMAChannelAttributeDisable(UDMA_CH16_ADC0_2,
                                    UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
                                    UDMA_ATTR_HIGH_PRIORITY |
                                    UDMA_ATTR_REQMASK);

    /* Configure the control parameters for the primary control structure for
     * the ADC0 Sequencer 2 channel. The primary control structure is used for
     * copying the data from ADC0 Sequencer 2 FIFO to dstBufferA. The transfer
     * data size is 32 bits and the source address is not incremented while
     * the destination address is incremented at 32-bit boundary.
     */
    MAP_uDMAChannelControlSet(UDMA_CH16_ADC0_2 | UDMA_PRI_SELECT,
                              UDMA_SIZE_32 | UDMA_SRC_INC_NONE |
                              UDMA_DST_INC_32 | UDMA_ARB_1);

    /* Set up the transfer parameters for the ADC0 Sequencer 2 primary control
     * structure. The mode is Basic mode so it will run to completion. */
    MAP_uDMAChannelTransferSet(UDMA_CH16_ADC0_2 | UDMA_PRI_SELECT,
                               UDMA_MODE_BASIC,
                               (void *)&ADC0->SSFIFO2, (void *)&dstBufferA,
                               sizeof(dstBufferA)/4);

//    /* Configure the control parameters for the alternate control structure for
//     * the ADC0 Sequencer 2 channel. The alternate control structure is used for
//     * copying the data from ADC0 Sequencer 2 FIFO to dstBufferB. The transfer
//     * data size is 32 bits and the source address is not incremented while
//     * the destination address is incremented at 32-bit boundary.
//     */
//    MAP_uDMAChannelControlSet(UDMA_CH16_ADC0_2 | UDMA_ALT_SELECT,
//                              UDMA_SIZE_32 | UDMA_SRC_INC_NONE |
//                              UDMA_DST_INC_32 | UDMA_ARB_1);
//
//    /* Set up the transfer parameters for the ADC0 Sequencer 2 alternate
//     * control structure. The mode is Basic mode so it will run to
//     * completion */
//    MAP_uDMAChannelTransferSet(UDMA_CH16_ADC0_2 | UDMA_ALT_SELECT,
//                               UDMA_MODE_PINGPONG,
//                               (void *)&ADC0->SSFIFO2, (void *)&dstBufferB,
//                               sizeof(dstBufferB)/4);

    /* The uDMA ADC0 Sequencer 2 channel is primed to start a transfer. As
     * soon as the channel is enabled and the Timer will issue an ADC trigger,
     * the ADC will perform the conversion and send a DMA Request. The data
     * transfers will begin. */
    MAP_uDMAChannelEnable(UDMA_CH16_ADC0_2);



    //定时器TIMER0使能

    /* Enable Timer-0 clock and configure the timer in periodic mode with
     * a frequency of 1 KHz. Enable the ADC trigger generation from the
     * timer-0. */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);                               //1khz定时器   ：  SAMP_FREQ
    while(!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)))
    {
    }

    MAP_TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC);
    MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, (systemClock/SAMP_FREQ));
    MAP_TimerADCEventSet(TIMER0_BASE, TIMER_ADC_TIMEOUT_A);
    MAP_TimerControlTrigger(TIMER0_BASE, TIMER_A, true);
    MAP_TimerEnable(TIMER0_BASE, TIMER_A);


    int i;
    OLED_Init();
    OLED_Refresh();
    UART_LY();
    while(1)
     {
//        OLED_ShowString(0,0,"WAIT!!",16);
//        Thd_Display(THD);         //显示THD，显示百分比
//              UART_LY();
//             display_wave();
//                    Wave_Display(&dstBufferA);         //波形显示，输入128个数，范围0-64
//                    Thd_Display(THDx);         //显示THD，显示百分比

             //////// Amplitude_Display(THDx,(float)Amp[0],(float)Amp[1],(float)Amp[2],(float)Amp[3]);  //显示幅值 基波 & 谐波 *4



        if(S_Flag == 1)
        {
            S_Flag=0;
            MAP_TimerDisable(TIMER0_BASE, TIMER_A);
            MAP_ADCSequenceDisable(ADC0_BASE, 2);
            switch(Fre_Flag)
            {
            case 0:
            {


//                MAP_ADCSequenceDMADisable(ADC0_BASE, 2);
//                MAP_uDMAChannelDisable(UDMA_CH16_ADC0_2);
                for(i=0;i<1024;i++)                       //补零：直流分量
                {
                    mean += dstBufferA[i];
                    if (maxxxx<dstBufferA[i]){maxxxx= dstBufferA[i];}
                    if (minnnn>dstBufferA[i]){minnnn= dstBufferA[i];}
                }
                mean = mean / 1024;
                delta=          maxxxx-minnnn;
                maxxxx=0;minnnn=0;


                 for(i=0;i<1024;i++)
                {
                    if(i < 512)
                        INPUT[i] = dstBufferA[i];
                    else
                        INPUT[i] = (float32_t)mean;
                }
                for(i=0;i<1024;i++)
                {
                    FFT_INPUT[2*i] = INPUT[i];//偶数部分为实部,
                    FFT_INPUT[2*i+1] = 0.00f;//奇数部分为虚部为0
                }
               arm_cfft_radix4_init_f32(&scfft,1024,0,1);//初始化scfft结构体，设定FFT相关参数
               arm_cfft_radix4_f32(&scfft,FFT_INPUT);//进行基四运算,输入值和输出值都存放在fftin中
                arm_cmplx_mag_f32(FFT_INPUT,fftOutput,1024);//计算结果求模得到幅值*/

            //    arm_cfft_radix4_init_f32(&scfft,1024,0,1);//初始化scfft结构体，设定FFT相关参数
             //   arm_cfft_radix4_f32(&scfft,INPUT);//进行基四运算,输入值和输出值都存放在fftin中
              //  arm_cmplx_mag_f32(INPUT,fftOutput,1024);//计算结果求模得到幅值*/


//                for(i=0;i<1024;i++)                                                                          //测试波形和频谱图
//                {
//                    UARTprintf("%d,",dstBufferA[i]);
//                    f_char_printf(fftOutput[i]);
//                    UARTprintf(",\n");
//                }
                for(i = 2 ; i < 512 ;i++)
                        {
                               if(MAX_Fre <fftOutput[i])
                               {
                                     MAX_Fre = fftOutput[i];
                                     heng_Fre = i;
                               }
                        }

                    emo = ((float_t)heng_Fre/1024)*SAMP_FREQ;
                    emo = emo + 200;
                    emo = (uint16_t)(emo/1000);
                    emo = (uint32_t)(emo*1000);
//                     f_char_printf(emo);                                        //基波频率
//                     UARTprintf("\n");
//                     f_char_printf(heng);
//                     UARTprintf("\n");
                if(emo > 50000)
                    SAMP_FREQ = 1000000;            //
                else
                    SAMP_FREQ = (uint32_t)emo * 20;

                if(SAMP_FREQ == 0)
                {
                    SAMP_FREQ = 250000;
                }

                MAP_ADCSequenceEnable(ADC0_BASE, 2);
                MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, (systemClock/SAMP_FREQ));
                MAP_TimerEnable(TIMER0_BASE, TIMER_A);


                dis_freq=emo;



                    emo = 0;
                     emp = 0;
                     heng_Fre = 0;
                     MAX_Fre = 0;

                  Fre_Flag =1;
//                  MAP_ADCSequenceDMAEnable(ADC0_BASE, 2);
//                  MAP_TimerEnable(TIMER0_BASE, TIMER_A);
//                  MAP_uDMAChannelEnable(UDMA_CH16_ADC0_2);
                  break;
            }
            case 1:
                  {
//                      MAP_TimerDisable(TIMER0_BASE, TIMER_A);
//                      MAP_ADCSequenceDisable(ADC0_BASE, 2);

//                                            for(i=0;i<1024;i++)                                                                     //测试波形和频谱图
//                                            {
//                                                UARTprintf("%d,",dstBufferA[i]);
////                                                f_char_printf(fftOutput[i]);
////                                                UARTprintf(",\n");
//                                            }
                      for(i=0;i<1024;i++)                       //补零：直流分量
                      {
                          mean += dstBufferA[i];
                      }
                      mean = mean / 1024;
                       for(i=0;i<1024;i++)
                      {
                          if(i < 512)
                              INPUT[i] = dstBufferA[i];
                          else
                              INPUT[i] = (float32_t)mean;
                      }
                      for(i=0;i<1024;i++)
                      {
                          FFT_INPUT[2*i] = INPUT[i];//偶数部分为实部,
                          FFT_INPUT[2*i+1] = 0.00f;//奇数部分为虚部为0
                      }
                      arm_cfft_radix4_init_f32(&scfft,1024,0,1);//初始化scfft结构体，设定FFT相关参数
                      arm_cfft_radix4_f32(&scfft,FFT_INPUT);//进行基四运算,输入值和输出值都存放在fftin中
                      arm_cmplx_mag_f32(FFT_INPUT,fftOutput,1024);//计算结果求模得到幅值*/







                      for(i=0;i<1024;i++)                                                                     //测试波形和频谱图
                      {
//                          UARTprintf("%d,",dstBufferA[i]);
                          f_char_printf(fftOutput[i]);
                          UARTprintf(",\n\r");
                      }

                    for(i = 2 ; i < 512 ;i++)
                    {
                        if(MAX <fftOutput[i])
                        {
                            MAX = fftOutput[i];
                            heng = i;
                        }
                    }

                    num1 = 2*heng-5;
                    for(num1  ;num1 < 2*heng+5;num1++)
                    {
                        if(MAX1 < fftOutput[num1])
                        {
                            MAX1 = fftOutput[num1];
                        }
//                        else
//                            MAX1 = MAX1;
                    }
                    num2 = 3*heng-5;
                    for(num2;num2<(3*heng+5);num2++)
                    {
                        if(MAX2 < fftOutput[num2])
                        {
                            MAX2 = fftOutput[num2];
                        }
//                        else
//                            MAX2 = MAX2;
                    }
                    num3 = 4*heng-5;
                    for(num3;num3<4*heng+5;num3++)
                    {
                        if(MAX3 < fftOutput[num3])
                        {
                            MAX3 = fftOutput[num3];
                        }
//                        else
//                            MAX3 = MAX3;
                    }
                    num4 = 5*heng-5;
                    for(num4;num4<5*heng+5;num4++)
                    {
                        if(MAX4 < fftOutput[num4])
                        {
                            MAX4 = fftOutput[num4];
                        }
//                        else
//                            MAX4 = MAX4;
                    }
                    THDx = sqrt(( (float)pow(MAX1,2)+(float)pow(MAX2,2)+(float)pow(MAX3,2)+(float)pow(MAX4,2) ))/(float)MAX;



                    Amp[0] = (float)MAX1/(float)MAX;
                    Amp[1] = (float)MAX2/(float)MAX;
                    Amp[2] = (float)MAX3/(float)MAX;
                    Amp[3] = (float)MAX4/(float)MAX;

                    int u;

                                for(u = 1;u<=127;u++)
                                {
                                   Re_Appear[u] =  (u8)(27*(( sin(6.28  / 127 * u))+
                                                  Amp[0]  * (sin(12.56 / 127 * u))+
                                                   Amp[1]  *( sin(18.84 / 127 * u))+
                                                   Amp[2] *( sin(25.12 / 127 * u))+
                                                    Amp[3]  *( sin(31.4  / 127 * u)))+34);
                                 //   Re_Appear[i]=dstBufferA[300+i]+delta-mean;

                                }


//                    Thd_Display(THDx);         //显示THD，显示百分比
//                    int j=8000000;
//                    while(j--);
//                    j=8000000;
//                    while(j--);



                    //                    Wave_Display(&dstBufferA);         //波形显示，输入128个数，范围0-64
//                    Thd_Display(THDx);         //显示THD，显示百分比

                    MAP_ADCSequenceDMADisable(ADC0_BASE, 2);
                       MAP_TimerDisable(TIMER0_BASE, TIMER_A);
                       MAP_uDMAChannelDisable(UDMA_CH16_ADC0_2);

//rms
                    for(i=20;i<40;i=i+1)
                    sum=sum+INPUT[i]*INPUT[i];


                    sum=sum/20;
                    rms=sqrt(sum)*3.3/4096;

                    Amplitude_Display((float)THDx*100,(float)Amp[0],(float)Amp[1],(float)Amp[2],(float)Amp[3],(float)dis_freq,(float)rms);
                    sum=0;

                    uart_output();

                      int j=8000000;
                      while(j--);
                      j=8000000;
                      while(j--);
                      j=8000000;
                       while(j--);

                      Wave_Display(Re_Appear);
//                              j=8000000;
                        //      while(j--);
                       // display_wave();
                       // j=8000000;
                        //while(j--);

//                       display_wave();
//                       UART_LY();

                      //显示幅值 基波 & 谐波 *4
//                    UART_LY();


                    THDx = 0;
                    Amp[0] = 0;
                    Amp[1] = 0;
                    Amp[2] = 0;
                    Amp[3] = 0;
                    MAX = 0 ;
                    MAX1 = 0;
                    MAX2 = 0;
                    MAX3 = 0;
                    MAX4 = 0;



//                         for(i=0;i<4;i++)
//                         {
//                             f_char_printf(Amp[i]);
//                             UARTprintf("归一化\n");
//                         }
//                      for(i=0;i<1024;i++)                                                                     //测试波形和频谱图
//                      {
////                          UARTprintf("%d,",dstBufferA[i]);
//                          f_char_printf(fftOutput[i]);
//                          UARTprintf(",\n");
//                      }
//                    UARTprintf("横坐标%d",heng);
//                    UARTprintf("\n");

                    Fre_Flag = 0;


                    MAP_ADCSequenceEnable(ADC0_BASE, 2);
//                    MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, (systemClock/SAMP_FREQ));
                    MAP_TimerEnable(TIMER0_BASE, TIMER_A);
                    Fre_Flag =1;

                    break;
             }
            default: break;


        }
            MAP_ADCSequenceDMAEnable(ADC0_BASE, 2);
            MAP_TimerEnable(TIMER0_BASE, TIMER_A);
            MAP_uDMAChannelEnable(UDMA_CH16_ADC0_2);
            qwe++;
         }


     }


 }



