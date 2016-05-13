
# Create project
create_project dma_test_proj ./dma_test_proj

# Set the directory path for the new project
set proj_dir [get_property directory [current_project]]

# Set project properties
set obj [get_projects dma_test_proj]
set_property "board_part" "em.avnet.com:microzed_7020:part0:1.1" $obj
set_property "default_lib" "xil_defaultlib" $obj
set_property "sim.ip.auto_export_scripts" "1" $obj
set_property "simulator_language" "Mixed" $obj

# Create 'sources_1' fileset (if not found)
if {[string equal [get_filesets -quiet sources_1] ""]} {
  create_fileset -srcset sources_1
}


create_bd_design "design_1"

create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.5 processing_system7_0

apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 -config {make_external "FIXED_IO, DDR" apply_board_preset "1" Master "Disable" Slave "Disable" }  [get_bd_cells processing_system7_0]

set_property -dict [list CONFIG.PCW_USE_M_AXI_GP0 {1} CONFIG.PCW_QSPI_GRP_SINGLE_SS_ENABLE {1} CONFIG.PCW_USB0_PERIPHERAL_ENABLE {0} CONFIG.PCW_USE_S_AXI_HP0 {1} CONFIG.PCW_USE_FABRIC_INTERRUPT {1} CONFIG.PCW_IRQ_F2P_INTR {1} CONFIG.PCW_TTC0_PERIPHERAL_ENABLE {0}] [get_bd_cells processing_system7_0]

create_bd_cell -type ip -vlnv xilinx.com:ip:axi_dma:7.1 axi_dma_0

set_property -dict [list CONFIG.c_sg_include_stscntrl_strm {0}] [get_bd_cells axi_dma_0]

apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Master "/processing_system7_0/M_AXI_GP0" Clk "Auto" }  [get_bd_intf_pins axi_dma_0/S_AXI_LITE]

apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Master "/axi_dma_0/M_AXI_SG" Clk "Auto" }  [get_bd_intf_pins processing_system7_0/S_AXI_HP0]

apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Slave "/processing_system7_0/S_AXI_HP0" Clk "Auto" }  [get_bd_intf_pins axi_dma_0/M_AXI_MM2S]

apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Slave "/processing_system7_0/S_AXI_HP0" Clk "Auto" }  [get_bd_intf_pins axi_dma_0/M_AXI_S2MM]

create_bd_cell -type ip -vlnv xilinx.com:ip:axis_data_fifo:1.1 axis_data_fifo_0

connect_bd_intf_net [get_bd_intf_pins axis_data_fifo_0/S_AXIS] [get_bd_intf_pins axi_dma_0/M_AXIS_MM2S]

connect_bd_intf_net [get_bd_intf_pins axi_dma_0/S_AXIS_S2MM] [get_bd_intf_pins axis_data_fifo_0/M_AXIS]

connect_bd_net [get_bd_pins axis_data_fifo_0/s_axis_aresetn] [get_bd_pins axi_dma_0/axi_resetn]

connect_bd_net [get_bd_pins axis_data_fifo_0/s_axis_aclk] [get_bd_pins axi_dma_0/s_axi_lite_aclk]


create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_0

connect_bd_net [get_bd_pins xlconcat_0/In0] [get_bd_pins axi_dma_0/mm2s_introut]
connect_bd_net [get_bd_pins xlconcat_0/In1] [get_bd_pins axi_dma_0/s2mm_introut]


connect_bd_net [get_bd_pins xlconcat_0/dout] [get_bd_pins processing_system7_0/IRQ_F2P]

save_bd_design

make_wrapper -files [get_files ./dma_test_proj/dma_test_proj.srcs/sources_1/bd/design_1/design_1.bd] -top
add_files -norecurse ./dma_test_proj/dma_test_proj.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v
update_compile_order -fileset sources_1
update_compile_order -fileset sim_1


puts "INFO: Project created:dma_test_proj"
