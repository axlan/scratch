
# Create project
create_project timing_test_proj ./timing_test_proj

# Set the directory path for the new project
set proj_dir [get_property directory [current_project]]

# Set project properties
set obj [get_projects timing_test_proj]
set_property "board_part" "em.avnet.com:microzed_7020:part0:1.1" $obj
set_property "default_lib" "xil_defaultlib" $obj
set_property "sim.ip.auto_export_scripts" "1" $obj
set_property "simulator_language" "Mixed" $obj

# Create 'sources_1' fileset (if not found)
if {[string equal [get_filesets -quiet sources_1] ""]} {
  create_fileset -srcset sources_1
}

set_property  ip_repo_paths  ./ip_repo/timetestip_1.0 [current_project]
update_ip_catalog

create_bd_design "design_1"

create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.5 processing_system7_0

apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 -config {make_external "FIXED_IO, DDR" apply_board_preset "1" Master "Disable" Slave "Disable" }  [get_bd_cells processing_system7_0]

set_property -dict [list CONFIG.PCW_USE_M_AXI_GP0 {1} CONFIG.PCW_USE_FABRIC_INTERRUPT {1} CONFIG.PCW_IRQ_F2P_INTR {1} CONFIG.PCW_QSPI_GRP_SINGLE_SS_ENABLE {1} CONFIG.PCW_USB0_PERIPHERAL_ENABLE {0} CONFIG.PCW_TTC0_PERIPHERAL_ENABLE {0}] [get_bd_cells processing_system7_0]

create_bd_cell -type ip -vlnv xilinx.com:user:timetestip:1.0 timetestip_0

apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Master "/processing_system7_0/M_AXI_GP0" Clk "Auto" }  [get_bd_intf_pins timetestip_0/S00_AXI]

create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_0

connect_bd_net [get_bd_pins timetestip_0/event_irq] [get_bd_pins xlconcat_0/In0]
connect_bd_net [get_bd_pins timetestip_0/error_irq] [get_bd_pins xlconcat_0/In1]
connect_bd_net [get_bd_pins xlconcat_0/dout] [get_bd_pins processing_system7_0/IRQ_F2P]

create_bd_port -dir O STATUS_GOOD
connect_bd_net [get_bd_ports STATUS_GOOD] [get_bd_pins timetestip_0/status_good]

save_bd_design

make_wrapper -files [get_files ./timing_test_proj/timing_test_proj.srcs/sources_1/bd/design_1/design_1.bd] -top
add_files -norecurse ./timing_test_proj/timing_test_proj.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v
update_compile_order -fileset sources_1
update_compile_order -fileset sim_1

add_files -fileset constrs_1 ./constraints/constraints.xdc
set_property target_constrs_file ./constraints/constraints.xdc [current_fileset -constrset]

puts "INFO: Project created:timing_test_proj"
