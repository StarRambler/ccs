/*
 * UART.c
 *
 *  Created on: 2021年11月10日
 *      Author: Administrator
 */

#include <ti/devices/msp432e4/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <UART.h>
#include <stdarg.h>

#if defined(__ICCARM__)
#pragma data_alignment=1024
uint8_t pui8ControlTable[1024];
#elif defined(__TI_ARM__)
#pragma DATA_ALIGN(pui8ControlTable, 1024)
uint8_t pui8ControlTable[1024];
#else
uint8_t pui8ControlTable[1024] __attribute__ ((aligned(1024)));
#endif


void UART7_IRQHandler(void)
{
    uint32_t getIntStatus;

    /* Read the interrupt status from the UART */
    getIntStatus = MAP_UARTIntStatus(UART7_BASE, true);

    /* If the UARTRX DMA Done interrupt is asserted then re-enable the DMA
     * control structure and clear the interrupt condition */
    if((getIntStatus & UART_INT_DMARX) == UART_INT_DMARX)
    {
        MAP_UARTIntClear(UART7_BASE, getIntStatus);

        MAP_uDMAChannelTransferSet(UDMA_CH20_UART7RX | UDMA_PRI_SELECT,
                                   UDMA_MODE_BASIC,
                                   (void *)&UART7->DR, (void *)&UART7->DR,
                                   100);
        MAP_uDMAChannelEnable(UDMA_CH20_UART7RX);
    }
}

uint8_t  USART3_TX_BUF[USART3_MAX_SEND_LEN];           //发送缓冲,最大USART3_MAX_SEND_LEN字节
int printf(const char *str, ...)
{
    uint16_t v,w;
    va_list ap;
    va_start(ap,str);
    vsprintf((char*)USART3_TX_BUF,str,ap);
    va_end(ap);
    v=strlen((const char*)USART3_TX_BUF);       //此次发送数据的长度
    for(w=0;w<v;w++)                            //循环发送数据
    {
      //while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
        UART9BitAddrSend(UART7_BASE, USART3_TX_BUF[w]);
    }
    return 0;
}

//int UART_LY(void)
//{
//    uint32_t systemClock;
//    int k ;
//    char a='b';
//    char b='c';
//    double lzd[8]={0.48,0.51,0.01,0.48,86.26,0,0,0};
//
//    /* Configure the system clock for 120 MHz */
//    systemClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
//                                          SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
//                                          120000000);
//
//    /* Enable the clock to the GPIO Port A and wait for it to be ready */
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
//    while(!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC)))
//    {
//
//    }
//
//    /* Configure the GPIO PA0-1 as U0RX-U0TX */
//    MAP_GPIOPinConfigure(GPIO_PC4_U7RX);
//    MAP_GPIOPinConfigure(GPIO_PC5_U7TX);
//    MAP_GPIOPinTypeUART(GPIO_PORTC_BASE, (GPIO_PIN_4 | GPIO_PIN_5));
//
//    /* Enable the clock to the UART-0 and wait for it to be ready */
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
//    while(!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART7)))
//    {
//    }
//
//    /* Configure UART for 115200 bps, 8-N-1 format. Enable the DMA Request
//     * generation and interrupt on DMARX Done. Also make sure that the FIFO is
//     * disabled. */
//    MAP_UARTConfigSetExpClk(UART7_BASE, systemClock, 9600,
//                            (UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE |
//                             UART_CONFIG_STOP_ONE));
//    MAP_UARTDMAEnable(UART7_BASE, UART_DMA_RX);
//    MAP_UARTIntEnable(UART7_BASE, UART_INT_DMARX);
//    MAP_UARTEnable(UART7_BASE);
//    MAP_UARTFIFODisable(UART7_BASE);
//
//    MAP_IntEnable(INT_UART3);
//
//    /* Enable the DMA and Configure Channel for UART0 RX for Basic mode of
//     * transfer */
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
//    while(!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UDMA)))
//    {
//    }
//
//    MAP_uDMAEnable();
//
//    /* Point at the control table to use for channel control structures. */
//    MAP_uDMAControlBaseSet(pui8ControlTable);
//
//    /* Map the UART0 RX DMA channel */
//    MAP_uDMAChannelAssign(UDMA_CH20_UART7RX);
//
//    /* Put the attributes in a known state for the uDMA UART0 RX channel.
//     * These should already be disabled by default. */
//    MAP_uDMAChannelAttributeDisable(UDMA_CH20_UART7RX,
//                                    UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
//                                    UDMA_ATTR_HIGH_PRIORITY |
//                                    UDMA_ATTR_REQMASK);
//
//    /* Configure the control parameters for the primary control structure for
//     * the UART0 RX channel. The primary control structure is used for copying
//     * the data from srcBuffer to UART0 Data register. The transfer data size
//     * is 32 bits and the source & destination address are not incremented. */
//    MAP_uDMAChannelControlSet(UDMA_CH20_UART7RX | UDMA_PRI_SELECT,
//                              UDMA_SIZE_32 | UDMA_SRC_INC_NONE | UDMA_DST_INC_NONE |
//                              UDMA_ARB_1);
//
//    /* Set up the transfer parameters for the UART0 RX primary control
//     * structure. The mode is Ping-Pong mode so it will run to completion. */
//    MAP_uDMAChannelTransferSet(UDMA_CH20_UART7RX | UDMA_PRI_SELECT,
//                               UDMA_MODE_BASIC,
//                               (void *)&UART7->DR, (void *)&UART7->DR,
//                               100);
//
//    /* The uDMA UART0 RX channel is primed to start a transfer. As soon as
//     * the channel is enabled and the UART0 receives a byte of data it will
//     * issue a DMA Request, and data transfers will begin. */
//    MAP_uDMAChannelEnable(UDMA_CH20_UART7RX);
//
//
//
//
//
//        printf ("%c ",a);
//
//        for (k=0;k<8;k++)
//        {
//          printf("%f ",lzd[k]);
//        }
//
//        printf ("%c ",b);
//}



