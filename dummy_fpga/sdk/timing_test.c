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

#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xil_io.h"

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

#define CH_SPACING ONE_MS_COUNTS/100

#define NUM_TESTS 100


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
	Xil_Out32(XPAR_TIMETESTIP_0_0+4*4,ch);
	Xil_Out32(XPAR_TIMETESTIP_0_0+4*5,1);
	Xil_Out32(XPAR_TIMETESTIP_0_0+4*5,0);
}

int main()
{
	int testCount=0;
	int i;

    init_platform();

    printf("Hello World\n\r");


    resetTest();



    while(testCount<NUM_TESTS)
    {
    	u32 event_mask = Xil_In32(XPAR_TIMETESTIP_0_0+4*6);
    	u32 error_mask = Xil_In32(XPAR_TIMETESTIP_0_0+4*7);

    	if(event_mask)
    	{
    		for(i=0;i<32;i++)
    		{
    			if(event_mask&(1<<i))
    			{
    				clearChannel(i);
    			}
    		}
    		testCount++;
    	}

    	if(error_mask)
    	{
    		 printf("Unexpected Error\n\r");
    		 return 0;
    	}

    }

    printf("Waiting for error\n\r");

    while(1)
    {
		u32 error_mask = Xil_In32(XPAR_TIMETESTIP_0_0+4*7);
		if(error_mask)
		{
			 break;
		}
    }
    printf("Test done\n\r");

    cleanup_platform();
    return 0;
}
