#
# Libraries
#
set LIB_PATH    "[get_install_path]/share/stratus/techlibs/GPDK045/gsclib045_svt_v4.4/gsclib045/timing"
set LIB_NAME	"slow_vdd1v2_basicCells.lib"
use_tech_lib	"$LIB_PATH/$LIB_NAME"

#
# C++ compiler options
#
set CLOCK_PERIOD "5.0"
set_attr cc_options			" -g -DCLOCK_PERIOD=$CLOCK_PERIOD"

#
# stratus_hls options
#
set_attr clock_period			$CLOCK_PERIOD	;# Clock period is required
#set_attr cycle_slack			1.75			;# Cycle slack adds margin to help logic synthesis close timing
set_attr balance_expr			off				;# Control expression balancing: off, width, delay  default=off
set_attr rtl_annotation			op,stack
set_attr default_input_delay	0.1				;# Prevents registering of inputs
set_attr dpopt_auto				off				;# Set automatic DPOPT part creation: off, array, op, expr
set_attr dpopt_with_enable		off				;# Control if enable pins are on DPOPT parts (default=off)
set_attr flatten_arrays			off			;# Control array flattening: none, all_const, lhs_const, all default=none--------------------
set_attr global_state_encoding	binary			;# Control FSM state encoding: binary, one_hot  default=binary
#set_attr ignore_cells			"XYZ* ABC*"		;# Define list of cells to ignore
set_attr inline_partial_constants	on			;# Remove constant portions of variables
set_attr lsb_trimming			on				;# Remove unused LSBs: default=off
set_attr message_detail			1				;# Detail level in log file: 0, 1, or 2
set_attr method_processing		synthesize		;# Control SC_METHOD synthesis: default=translate, recommended=synthesize
#set_attr path_delay_limit		120				;# Prevents long paths through resource sharing muxes (in % of clock period)
set_attr sched_asap				off				;# Create shortest possible schedule: default=off, recommended=off
set_attr unroll_loops			off				;# Control loop unrolling: default=off---------------------------------------------
set_attr wireload				none			;# Control wireload model used: default=none

#
# Simulation Options
#
use_verilog_simulator	incisive				;# 'mti', 'incisive', 'vcs', 'vcsi'
enable_waveform_logging	-vcd					;# Generate vcd or fsdb waveform file with each simulation
set_attr end_of_sim_command "make cmp_result"	;# Make rule to run at end of each simulation

#
# System Module Configurations
#
define_system_module	main	main.cc		;# SystemC module for simulation only
define_system_module	System	system.cc	;# SystemC module for simulation only
define_system_module	tb	tb.cc			;# SystemC module for simulation only

#
# Synthesis Module 
#
define_hls_module	Partition	Partition.cc		;# SystemC module to be synthesized
define_hls_module	MergeLV1_p0	MergeLV1_p0.cc		;# SystemC module to be synthesized
define_hls_module	MergeLV1_p1	MergeLV1_p1.cc		;# SystemC module to be synthesized
define_hls_module	MergeLV2_p0	MergeLV2_p0.cc		;# SystemC module to be synthesized
#define_hls_module	MergeSort	MergeSort.cc		;# SystemC module to be synthesized

#
# Synthesis Configurations
#
define_hls_config	Partition	BASIC		;# A synthesis configuration of the hls_module 
define_hls_config	Partition	UNROLL
define_hls_config	Partition	FLAT        --flatten_arrays=all

define_hls_config	MergeLV1_p0	BASIC
define_hls_config	MergeLV1_p0	UNROLL		--unroll_loops=on
define_hls_config	MergeLV1_p0	FLAT        --flatten_arrays=all

define_hls_config	MergeLV1_p1	BASIC
define_hls_config	MergeLV1_p1	UNROLL		--unroll_loops=on
define_hls_config	MergeLV1_p1	FLAT        --flatten_arrays=all

define_hls_config	MergeLV2_p0	BASIC
define_hls_config	MergeLV2_p0	UNROLL		--unroll_loops=on
define_hls_config	MergeLV2_p0	FLAT        --flatten_arrays=all


#define_hls_config	MergeSort	UNROLL		--unroll_loops=on;# A synthesis configuration of the hls_module 
#define_hls_config   MergeSort   FLAT        --flatten_arrays=all
#define_hls_config   MergeSort   REG			--unroll_loops=on		
#define_hls_config   MergeSort   MEM			--unroll_loops=on
#define_hls_config   MergeSort   FLAT_UN        --flatten_arrays=all	--unroll_loops=on
#define_hls_config   MergeSort   FLAT_UN_DP        --flatten_arrays=all	--unroll_loops=on --dpopt_auto=all

#define_hls_config   MergeSort   PIPELINE_1_FILE    -DMP=1	--flatten_arrays=all --unroll_loops=on
#define_hls_config   MergeSort   PIPELINE_1_FL    -DII=1	--flatten_arrays=all	
#define_hls_config   MergeSort   PIPELINE_2_FL    -DII=2	--flatten_arrays=all
#define_hls_config   MergeSort   PIPELINE_3_FL    -DII=3	--flatten_arrays=all
#define_hls_config   MergeSort   PIPELINE_1_FLDP    -DII=1	--flatten_arrays=all	--dpopt_auto=all
#define_hls_config   MergeSort   PIPELINE_2_FLDP    -DII=2	--flatten_arrays=all	--dpopt_auto=all
#define_hls_config   MergeSort   PIPELINE_3_FLDP    -DII=3	--flatten_arrays=all	--dpopt_auto=all



#
# Simulation Configurations
#
define_sim_config B			;#{MergeSort BEH}			;# A behavioral PIN-level configuration

# The following rules are TCL code to create a simulation configuration
# for RTL_V for each hls_config defined.
foreach cfg { BASIC UNROLL FLAT} {
	define_sim_config V_${cfg} \
	"Partition RTL_V ${cfg}" \
	"MergeLV1_p0 RTL_V ${cfg}" \
	"MergeLV1_p1 RTL_V ${cfg}" \
	"MergeLV2_p0 RTL_V ${cfg}" \
}

