/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */
#include <unistd.h>
#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xscugic.h"

#define EVENT_IRQ 61
#define ERROR_IRQ 62


/*
	3'h0   : reg_data_out <= reset_reg;
	3'h1   : reg_data_out <= num_ch_reg;
	3'h2   : reg_data_out <= period_reg;
	3'h3   : reg_data_out <= ch_spacing_reg;
	3'h4   : reg_data_out <= clear_ch_reg;
	3'h5   : reg_data_out <= ch_sel_reg;
	3'h6   : reg_data_out <= event_mask_ro_reg;
	3'h7   : reg_data_out <= error_mask_ro_reg;
 */

#define CLK_FREQ 100e6

#define ONE_MS_COUNTS 1e-3*CLK_FREQ

#define NUM_CH 16
#define MAX_NUM_CH 32

#define CH_SPACING ONE_MS_COUNTS/100

#define NUM_TESTS 10
#define FAIL_CHANNEL 8

volatile int eventCounts[MAX_NUM_CH];
volatile int triggerError[MAX_NUM_CH];
volatile int errorDetected;

void clearChannel(int ch);

XScuGic InterruptController; /* Instance of the Interrupt Controller */
static XScuGic_Config *GicConfig;/* The configuration parameters of the controller */
void event_irq_Handler(void *baseaddr_p){
	int i;
	u32 event_mask = Xil_In32(XPAR_TIMETESTIP_0_0+4*6);
	for(i=0;i<MAX_NUM_CH;i++)
	{
		if(event_mask&(1<<i))
		{
			if(!triggerError[i])
			{
				clearChannel(i);
			}
			eventCounts[i]++;
		}
	}
}

void error_irq_Handler(void *baseaddr_p){
	errorDetected = 1;
}



int ScuGicInterrupt_Init()
{
 int Status;
 /*
  * Initialize the interrupt controller driver so that it is ready to
  * use.
  * */
 Xil_ExceptionInit();
 GicConfig = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
 if (NULL == GicConfig) {
  return XST_FAILURE;
 }
 Status = XScuGic_CfgInitialize(&InterruptController, GicConfig,
   GicConfig->CpuBaseAddress);
 if (Status != XST_SUCCESS) {
  return XST_FAILURE;
 }
 /*
  * Setup the Interrupt System
  * */
 /*
  * Connect the interrupt controller interrupt handler to the hardware
  * interrupt handling logic in the ARM processor.
  */
 Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
   (Xil_ExceptionHandler) XScuGic_InterruptHandler,
   (void *) &InterruptController);

 /*
  * Connect a device driver handler that will be called when an
  * interrupt for the device occurs, the device driver handler performs
  * the specific interrupt processing for the device
  */
 Status = XScuGic_Connect(&InterruptController,EVENT_IRQ,
   (Xil_ExceptionHandler)event_irq_Handler,
   (void *)&InterruptController);

 XScuGic_Enable(&InterruptController, EVENT_IRQ);

 Status = XScuGic_Connect(&InterruptController,ERROR_IRQ,
   (Xil_ExceptionHandler)error_irq_Handler,
   (void *)&InterruptController);

 XScuGic_Enable(&InterruptController, ERROR_IRQ);

 /*
  * Enable interrupts in the ARM
  */
 Xil_ExceptionEnable();
 //Only used for edge sensitive Interrupts
 XScuGic_SetPriorityTriggerType(&InterruptController, EVENT_IRQ,
      0x0, 3);
 XScuGic_SetPriorityTriggerType(&InterruptController, ERROR_IRQ,
       0x0, 3);


 if (Status != XST_SUCCESS) {
  return XST_FAILURE;
 }
 return XST_SUCCESS;
}




void initTest()
{
	Xil_Out32(XPAR_TIMETESTIP_0_0+4*1,NUM_CH);
	Xil_Out32(XPAR_TIMETESTIP_0_0+4*2,ONE_MS_COUNTS);
	Xil_Out32(XPAR_TIMETESTIP_0_0+4*3,CH_SPACING);
	Xil_Out32(XPAR_TIMETESTIP_0_0+4*4,0);
	Xil_Out32(XPAR_TIMETESTIP_0_0+4*5,0);
}


void resetTest()
{
	initTest();
	Xil_Out32(XPAR_TIMETESTIP_0_0,1);
	Xil_Out32(XPAR_TIMETESTIP_0_0,0);
}

void clearChannel(int ch)
{
	Xil_Out32(XPAR_TIMETESTIP_0_0+4*5,ch);
	Xil_Out32(XPAR_TIMETESTIP_0_0+4*4,1);
	Xil_Out32(XPAR_TIMETESTIP_0_0+4*4,0);
}

int main()
{
	int i=0;
	int testCount=0;
	int xstatus;

    init_platform();

    printf("Hello World\n\r");

    for(i=0;i<MAX_NUM_CH;i++)
    {
    	eventCounts[i] = 0;
    	triggerError[i] = 0;
    }

    errorDetected = 0;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//SCUGIC interrupt controller Intialization
	//Registration of the Timer ISR
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    xstatus = ScuGicInterrupt_Init();
	if(XST_SUCCESS != xstatus)
	{
		print(" :( SCUGIC INIT FAILED \n\r");
	}

	resetTest();

    while(1)
    {
    	printf("Counts:");
    	 for(i=0;i<MAX_NUM_CH;i++)
		{
    		 printf("%d,",eventCounts[i]);
		}
    	 printf("\n\r");



		if(errorDetected)
		{
			u32 error_mask = Xil_In32(XPAR_TIMETESTIP_0_0+4*7);
			if(triggerError[FAIL_CHANNEL])
			{
				printf("Error: %X\n\r",(unsigned int)error_mask);
				break;
			}
			printf("Unexpected Error: %X\n\r",(unsigned int)error_mask);
			return 0;
		}

		if(testCount>=NUM_TESTS)
		{
			 printf("trigger error:%d\r\n",FAIL_CHANNEL);
			triggerError[FAIL_CHANNEL]=1;
		}

    	sleep(1);
    	testCount++;
    }
    printf("Test Done\n\r");

    cleanup_platform();
    return 0;
}
