----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 05/02/2016 03:25:20 PM
-- Design Name: 
-- Module Name: test_module - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity test_module is
    Port ( clk_i : in STD_LOGIC;
           reset_i : in STD_LOGIC;
           num_channels_i : in STD_LOGIC_VECTOR (31 downto 0);
           period_i : in STD_LOGIC_VECTOR (31 downto 0);
           channel_spacing_i : in STD_LOGIC_VECTOR (31 downto 0);
           channel_sel_i : in STD_LOGIC_VECTOR (31 downto 0);
           clear_channel_i : in STD_LOGIC;
           event_irq_o : out STD_LOGIC;
           event_mask_o : out STD_LOGIC_VECTOR (31 downto 0);
           error_irq_o : out STD_LOGIC;
           error_mask_o : out STD_LOGIC_VECTOR (31 downto 0));
end test_module;

architecture Behavioral of test_module is
 -----------------------------------------------------------------------------
 --! @name Internal Registers
 -----------------------------------------------------------------------------
 --! @{

 signal num_channels : unsigned (31 downto 0);
 signal period : unsigned (31 downto 0);
 signal channel_spacing : unsigned (31 downto 0);
 
 signal period_count : unsigned (31 downto 0);
 signal channel_count : unsigned (31 downto 0);
 signal cur_channel : unsigned (31 downto 0);
  
 signal event_irq : STD_LOGIC;
 signal event_mask : STD_LOGIC_VECTOR (31 downto 0);
 signal error_irq : STD_LOGIC;
 signal error_mask : STD_LOGIC_VECTOR (31 downto 0);


 --! @}
begin
    -----------------------------------------------------------------------------
    -- Outputs
    -----------------------------------------------------------------------------
    
    event_irq_o <= event_irq;
    event_mask_o  <= event_mask;
    error_irq_o <= error_irq;
    error_mask_o  <= error_mask;
    
     process (clk_i) is
     begin
        if  rising_edge(clk_i) then
            if(reset_i = '1') then
                period_count <= to_unsigned(1, 32);
                channel_count <= to_unsigned(1, 32);
                cur_channel <= to_unsigned(0, 32);
                
                event_irq <= '0';
                event_mask <= std_logic_vector(to_unsigned(0, 32));
                error_irq <= '0';
                error_mask <=  std_logic_vector(to_unsigned(0, 32));
                
                if(unsigned(num_channels_i) < 32) then
                    num_channels <= unsigned(num_channels_i);
                else
                    num_channels <= to_unsigned(32, 32);
                end if;
                period <= unsigned(period_i);
                channel_spacing <= unsigned(channel_spacing_i);
            else
                event_irq <= '0';
                error_irq <= '0';
                period_count <= period_count + 1;
                channel_count <= channel_count + 1;
                if(period_count = period) then
                    channel_count <= to_unsigned(1, 32);
                    period_count <= to_unsigned(1, 32);
                    cur_channel <= to_unsigned(0, 32);
                else
                    if(cur_channel < num_channels and channel_count = channel_spacing) then
                        channel_count <= to_unsigned(1, 32);
                        cur_channel <= cur_channel + 1;
                        event_mask(to_integer(cur_channel)) <= '1';
                        if(event_mask(to_integer(cur_channel)) = '1') then
                            error_mask(to_integer(cur_channel)) <= '1';
                            error_irq <= '1';
                        end if;
                        event_irq <= '1';
                    end if;
                end if;
                if(clear_channel_i = '1') then
                    event_mask(to_integer(unsigned(channel_sel_i))) <= '0';
                end if;
            end if;
        end if;
     end process;
end Behavioral;
