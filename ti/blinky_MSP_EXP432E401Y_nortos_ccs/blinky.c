#include <stdint.h>
#include <stdbool.h>
#include "oled.h"

#include "ti/devices/msp432e4/driverlib/driverlib.h"

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

const unsigned char DISPLAY[128] = {32,35,38,41,44,47,49,52,54,56,58,60,61,62,63,63,64,63,63,62,
                                   61,60,58,56,54,52,49,47,44,41,38,35,32,28,25,22,19,16,14,11,
                                   9,7,5,3,2,1,0,0,0,0,0,1,2,3,5,7,9,11,14,16,19,22,25,28,32,35,
                                   38,41,44,47,49,52,54,56,58,60,61,62,63,63,64,63,63,62,
                                   61,60,58,56,54,52,49,47,44,41,38,35,32,28,25,22,19,16,14,11,
                                   9,7,5,3,2,1,0,0,0,0,0,1,2,3,5,7,9,11,14,16,19,22,25,28};

int main(void)
{
    OLED_Init();
    OLED_Refresh();
    while(1)
    {
        Wave_Display(&DISPLAY);         //波形显示，输入128个数，范围0-64
        Thd_Display(0.610323f);         //显示THD，显示百分比
        Amplitude_Display(215.6,115.11,152.1,112.1,112.1);  //显示幅值 基波 & 谐波 *4
    }
}
