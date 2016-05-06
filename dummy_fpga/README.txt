Timing Test FPGA IP

Included Files:
  projgen.tcl - tcl script for generating example project file
  ip_repo/ - countains the IP block for testing
  unecrypted/ - countains unencrypted versions of the source files for reference
  timing_test.c - example software written to be compiled with the Xilinx SDK and run on the processor bare metal

Project Generation:
  Running "vivado -mode batch -nojournal -nolog -source projgen.tcl" generates a project file targetting the Microzed Zynq developement board.
  The projgen.tcl script file countaints TCL commands to add the IP block, instantiate and configure a Zynq processor instance, and connect things together. This script can be modified or used as an example for configuring a project for a different platform.

Running the Test:
  With the project created a bitfile can be generated through the GUI or tcl commands. Once the bitfile is generated the hardware design can be exported to the SDK where an application project can be created for the test C code. The procedure for generating this was similar to the one presented in the tutorials here for generating a boot image for an SDK card http://zedboard.org/support/design/1519/10

IP Block Description:
  The IP block is meant to simulate the behavior of tracking channels raising interrupts to indicate they need to be serviced. The inteface is an AXI4-Lite interface which maps 8 4byte registers. These registers have the following function:
  0 reset - while this is nonzero reset the test modules state
  1 number of channels - sets the number of simulated channels (value is used at reset). Must be less then 32
  2 channel period - sets the number of clock cycles between interrupts on each channel (value is used at reset)
  3 channel spacing - sets the number of clock cycles between the start of each channel (value is used at reset). The first channel starts after this many clock cycles followed by the second after another channel spacing. (channel spacing) * (number of channels + 1) must be less then the channel period
  4 clear selected channel - clear the inturrupt on from the selected channel 
  5 selected channel - specify the selected channel
  6 channel inturrupt mask - each bit indicates whether a channel has an interrupt that needs to be cleared
  7 channel error mask - each bit indicates if a channel had an interrupt that was not serviced within a channel period of being raised. These can only be cleared with a reset 
  In addition the IP block has an output meant to control an LED. It is high as long as the channel error mask is zero and goes low when an error occurs
  
Included Test Description:
  The purpose of this test is to show that an IP block with encrypted source can succesfully be included in a Vivado build and that a processor environment can communicate with the IP block to meet real time deadlines. timing_test.c configures 16 channels to go off every 1ms services them for 10 seconds then lets an error occur to excercise all of the blocks functionality.

Further Testing:
  The intent is that the provided HDL should be tested in the target hardware and build environment. It will likely need to be adapted to test on a different hardware or OS environment. There the test should be modified to test the long term ability to meet timing deadlines while the processor is under heavy load.
