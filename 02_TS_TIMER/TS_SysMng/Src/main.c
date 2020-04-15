/***********************************************************************************
 * File                     :main.c
 *
 * Title                    :
 *
 * Author                   :Tarik SEMRADE
 *
 * Description              :Timer0 interrupt project and frequency management.
 *                           Timer0 interrupt = 50us.
 *
 * Version                  :
 *
 * Copyright (c) 2020 Tarik SEMRADE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *********************************************************************************/

/**********************************************************************************
 *  Included Files
 *
 *********************************************************************************/
#include "F28x_Project.h"
#include "gpio.h"
#include "device.h"
#include "main.h"
/**********************************************************************************
 *  Defines
 *
 *********************************************************************************/
#define BLUE_LED        DEVICE_GPIO_PIN_LED1
#define RED_LED         DEVICE_GPIO_PIN_LED2

/**********************************************************************************
 *  Globals
 *
 *********************************************************************************/
Uint32 InteCounter = 0UL;

/**********************************************************************************
 * \function:       main
 * \brief           main `0` numbers
 * \param[in]       void
 * \return          void
 **********************************************************************************/
void
main (void)
{

    /* Set up system flash and turn peripheral clocks */
    InitSysCtrl();

    /* GPIO Init */
    InitGpio();

    /* Globally disable maskable CPU interrupts */
    DINT;

    /* Clear and disable all PIE interrupts */
    InitPieCtrl();

    /* Individually disable maskable CPU interrupts */
    IER = 0x0000;

    /* Clear all CPU interrupt flags */
    IFR = 0x0000;

    /* Populate the PIE interrupt vector table with */
    InitPieVectTable();

    /***********************Interrupt linking functions*****************************/

    /* Timer0 ISR function linking */
    EALLOW;

    PieVectTable.TIMER0_INT = &Timer0_ISR;

    EDIS;

    /************************Peripheral Initialization*****************************/
    /* Initialize the Device Peripheral. This function can be */
    InitCpuTimers();

    /* 200MHz CPU Freq, 50 uSeconds Period (in uSeconds) */
    ConfigCpuTimer(&CpuTimer0, 200, 50);

    /* Enable TINT0 in the PIE: Group 1 __interrupt 7 */
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

    /* Enable group 1 interrupts */
    IER |= M_INT1;

    /* Enable Global interrupt INTM */
    EINT;

    /* GPIO init for leds */
    GpioLedInit();

    /* Start Timer0 */
    StartCpuTimer0();

    /* Infinite led loop */
    while (1)
    {
        if ( 5000U == CpuTimer0.InterruptCount )
        {
            /* Toggle Red Led with 250ms = 5 000 * 50us*/
            GPIO_togglePin(RED_LED);

        }
        if ( 10000U == CpuTimer0.InterruptCount  )
        {
            /* Toggle Blue Led with 500ms = 10000 * 50us*/
            GPIO_togglePin(BLUE_LED);

            CpuTimer0.InterruptCount = 0u;
        }
        /* ... */
        /* Max CpuTimer0.InterruptCount is 4,294,967,295 */
    }

}
/**********************************************************************************
 * \function:       Timer0_ISR
 * \brief           `0` Param
 * \param[in]       void
 * \return          void
 **********************************************************************************/
interrupt void
Timer0_ISR (void)
{

    /* Clear Timer interrupt flag */
    CpuTimer0Regs.TCR.bit.TIF = 0;

    /* Toggle led and increment the timer0 */
    CpuTimer0.InterruptCount++;

    /* Acknowledge this interrupt to receive more interrupts from group 1 */
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}
/**********************************************************************************
 * \function:       GpioLedInit
 * \brief           `0` Param
 * \param[in]       void
 * \return          void
 **********************************************************************************/
void
GpioLedInit (void)
{

    /* GPIO bleuLed configuration using TI function */
    GPIO_SetupPinMux(BLUE_LED,GPIO_MUX_CPU1,0);
    GPIO_SetupPinOptions(BLUE_LED, GPIO_OUTPUT, GPIO_ASYNC);

    /* GPIO RedLed configuration using TI function */
    GPIO_SetupPinMux(RED_LED,GPIO_MUX_CPU1,0);
    GPIO_SetupPinOptions(RED_LED, GPIO_OUTPUT, GPIO_ASYNC);

}
