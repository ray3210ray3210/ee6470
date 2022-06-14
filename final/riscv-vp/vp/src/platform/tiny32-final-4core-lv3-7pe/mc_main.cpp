#include <cstdlib>
#include <ctime>

#include "basic_timer.h"
#include "core/common/clint.h"
#include "display.hpp"
#include "dma.h"
#include "elf_loader.h"
#include "ethernet.h"
#include "fe310_plic.h"
#include "flash.h"
#include "debug_memory.h"
#include "iss.h"
#include "mem.h"
#include "memory.h"
#include "mram.h"
#include "sensor.h"
#include "sensor2.h"
#include "syscall.h"
#include "terminal.h"
#include "MergeLV1_p0.h"
#include "MergeLV2_p0.h"
#include "MergeLV3_p0.h"
#include "util/options.h"
#include "platform/common/options.h"

#include "gdb-mc/gdb_server.h"
#include "gdb-mc/gdb_runner.h"

#include <boost/io/ios_state.hpp>
#include <boost/program_options.hpp>
#include <iomanip>
#include <iostream>

using namespace rv32;
namespace po = boost::program_options;

class BasicOptions : public Options {
public:
	typedef unsigned int addr_t;

	std::string mram_image;
	std::string flash_device;
	std::string network_device;
	std::string test_signature;

	addr_t mem_size = 1024 * 1024 * 32;  // 32 MB ram, to place it before the CLINT and run the base examples (assume
	                                     // memory start at zero) without modifications
	addr_t mem_start_addr = 0x00000000;
	addr_t mem_end_addr = mem_start_addr + mem_size - 1;
	addr_t clint_start_addr = 0x02000000;
	addr_t clint_end_addr = 0x0200ffff;
	addr_t sys_start_addr = 0x02010000;
	addr_t sys_end_addr = 0x020103ff;
	addr_t term_start_addr = 0x20000000;
	addr_t term_end_addr = term_start_addr + 16;
	addr_t ethernet_start_addr = 0x30000000;
	addr_t ethernet_end_addr = ethernet_start_addr + 1500;
	addr_t plic_start_addr = 0x40000000;
	addr_t plic_end_addr = 0x41000000;
	addr_t sensor_start_addr = 0x50000000;
	addr_t sensor_end_addr = 0x50001000;
	addr_t sensor2_start_addr = 0x50002000;
	addr_t sensor2_end_addr = 0x50004000;
	addr_t mram_start_addr = 0x60000000;
	addr_t mram_size = 0x10000000;
	addr_t mram_end_addr = mram_start_addr + mram_size - 1;
	addr_t dma_start_addr = 0x70000000;
	addr_t dma_end_addr = 0x70001000;
	addr_t flash_start_addr = 0x71000000;
	addr_t flash_end_addr = flash_start_addr + Flashcontroller::ADDR_SPACE;  // Usually 528 Byte
	// addr_t display_start_addr = 0x72000000;
	// addr_t display_end_addr = display_start_addr + Display::addressRange;
	//===LV1===
	addr_t MergeLV1_p0_start_addr = 0x73000000;
 	addr_t MergeLV1_p0_size = 0x01000000;
 	addr_t MergeLV1_p0_end_addr = MergeLV1_p0_start_addr + MergeLV1_p0_size - 1;
	addr_t MergeLV1_p1_start_addr = 0x74000000;
 	addr_t MergeLV1_p1_size = 0x01000000;
 	addr_t MergeLV1_p1_end_addr = MergeLV1_p1_start_addr + MergeLV1_p1_size - 1;
	addr_t MergeLV1_p2_start_addr = 0x75000000;
 	addr_t MergeLV1_p2_size = 0x01000000;
 	addr_t MergeLV1_p2_end_addr = MergeLV1_p2_start_addr + MergeLV1_p2_size - 1;
	addr_t MergeLV1_p3_start_addr = 0x76000000;
 	addr_t MergeLV1_p3_size = 0x01000000;
 	addr_t MergeLV1_p3_end_addr = MergeLV1_p3_start_addr + MergeLV1_p3_size - 1;
	//===LV2===
	addr_t MergeLV2_p0_start_addr = 0x77000000;
 	addr_t MergeLV2_p0_size = 0x01000000;
 	addr_t MergeLV2_p0_end_addr = MergeLV2_p0_start_addr + MergeLV2_p0_size - 1;
	addr_t MergeLV2_p1_start_addr = 0x78000000;
 	addr_t MergeLV2_p1_size = 0x01000000;
 	addr_t MergeLV2_p1_end_addr = MergeLV2_p1_start_addr + MergeLV2_p1_size - 1;

	addr_t MergeLV3_p0_start_addr = 0x79000000;
 	addr_t MergeLV3_p0_size = 0x01000000;
 	addr_t MergeLV3_p0_end_addr = MergeLV3_p0_start_addr + MergeLV3_p0_size - 1;
	bool use_E_base_isa = false;

	OptionValue<unsigned long> entry_point;

	BasicOptions(void) {
        	// clang-format off
		add_options()
			("memory-start", po::value<unsigned int>(&mem_start_addr),"set memory start address")
			("memory-size", po::value<unsigned int>(&mem_size), "set memory size")
			("use-E-base-isa", po::bool_switch(&use_E_base_isa), "use the E instead of the I integer base ISA")
			("entry-point", po::value<std::string>(&entry_point.option),"set entry point address (ISS program counter)")
			("mram-image", po::value<std::string>(&mram_image)->default_value(""),"MRAM image file for persistency")
			("mram-image-size", po::value<unsigned int>(&mram_size), "MRAM image size")
			("flash-device", po::value<std::string>(&flash_device)->default_value(""),"blockdevice for flash emulation")
			("network-device", po::value<std::string>(&network_device)->default_value(""),"name of the tap network adapter, e.g. /dev/tap6")
			("signature", po::value<std::string>(&test_signature)->default_value(""),"output filename for the test execution signature");
        	// clang-format on
	}

	void parse(int argc, char **argv) override {
		Options::parse(argc, argv);

		entry_point.finalize(parse_ulong_option);
		mem_end_addr = mem_start_addr + mem_size - 1;
		assert((mem_end_addr < clint_start_addr || mem_start_addr > MergeLV2_p0_end_addr) &&
		       "RAM too big, would overlap memory");
		mram_end_addr = mram_start_addr + mram_size - 1;
		assert(mram_end_addr < dma_start_addr && "MRAM too big, would overlap memory");
	}
};

std::ostream& operator<<(std::ostream& os, const BasicOptions& o) {
	os << std::hex;
	os << "mem_start_addr:\t" << +o.mem_start_addr << std::endl;
	os << "mem_end_addr:\t"   << +o.mem_end_addr   << std::endl;
	os << static_cast <const Options&>( o );
	return os;
}

int sc_main(int argc, char **argv) {
	BasicOptions opt;
	opt.parse(argc, argv);

	std::srand(std::time(nullptr));  // use current time as seed for random generator

	tlm::tlm_global_quantum::instance().set(sc_core::sc_time(opt.tlm_global_quantum, sc_core::SC_NS));

	ISS core0(0);
	ISS core1(1);
	ISS core2(2);
	ISS core3(3);

	SimpleMemory mem("SimpleMemory", opt.mem_size);
	SimpleTerminal term("SimpleTerminal");
	ELFLoader loader(opt.input_program.c_str());
	SimpleBus<6, 17> bus("SimpleBus");

	CombinedMemoryInterface core0_mem_if("MemoryInterface0", core0);
	CombinedMemoryInterface core1_mem_if("MemoryInterface1", core1);
	CombinedMemoryInterface core2_mem_if("MemoryInterface2", core2);
	CombinedMemoryInterface core3_mem_if("MemoryInterface3", core3);

	SyscallHandler sys("SyscallHandler");
	FE310_PLIC<1, 64, 96, 32> plic("PLIC");
	CLINT<4> clint("CLINT");
	SimpleSensor sensor("SimpleSensor", 2);
	SimpleSensor2 sensor2("SimpleSensor2", 5);
	BasicTimer timer("BasicTimer", 3);
	SimpleMRAM mram("SimpleMRAM", opt.mram_image, opt.mram_size);
	SimpleDMA dma("SimpleDMA", 4);
	Flashcontroller flashController("Flashcontroller", opt.flash_device);
	// EthernetDevice ethernet("EthernetDevice", 7, mem.data, opt.network_device);
	// Display display("Display");
	DebugMemoryInterface dbg_if("DebugMemoryInterface");
	//===LV1===
	MergeLV1_p0 mergeLV1_p0("mergeLV1_p0");
	MergeLV1_p0 mergeLV1_p1("mergeLV1_p1");
	MergeLV1_p0 mergeLV1_p2("mergeLV1_p2");
	MergeLV1_p0 mergeLV1_p3("mergeLV1_p3");
	//===LV2===
	MergeLV2_p0 mergeLV2_p0("mergeLV2_p0");
	MergeLV2_p0 mergeLV2_p1("mergeLV2_p1");
	//===LV3===
	MergeLV3_p0 mergeLV3_p0("mergeLV3_p0");

	MemoryDMI dmi = MemoryDMI::create_start_size_mapping(mem.data, opt.mem_start_addr, mem.size);
	InstrMemoryProxy core0_instr_mem(dmi, core0);
	InstrMemoryProxy core1_instr_mem(dmi, core1);
	InstrMemoryProxy core2_instr_mem(dmi, core2);
	InstrMemoryProxy core3_instr_mem(dmi, core3);

	std::shared_ptr<BusLock> bus_lock = std::make_shared<BusLock>();
	core0_mem_if.bus_lock = bus_lock;
	core1_mem_if.bus_lock = bus_lock;
	core2_mem_if.bus_lock = bus_lock;
	core3_mem_if.bus_lock = bus_lock;

	instr_memory_if *core0_instr_mem_if = &core0_mem_if;
	instr_memory_if *core1_instr_mem_if = &core1_mem_if;
	instr_memory_if *core2_instr_mem_if = &core2_mem_if;
	instr_memory_if *core3_instr_mem_if = &core3_mem_if;
	data_memory_if *core0_data_mem_if = &core0_mem_if;
	data_memory_if *core1_data_mem_if = &core1_mem_if;
	data_memory_if *core2_data_mem_if = &core2_mem_if;
	data_memory_if *core3_data_mem_if = &core3_mem_if;

	if (opt.use_instr_dmi) {
		core0_instr_mem_if = &core0_instr_mem;
		core1_instr_mem_if = &core1_instr_mem;
		core2_instr_mem_if = &core2_instr_mem;
		core3_instr_mem_if = &core3_instr_mem;
	}
	if (opt.use_data_dmi) {
		core0_mem_if.dmi_ranges.emplace_back(dmi);
		core1_mem_if.dmi_ranges.emplace_back(dmi);
		core2_mem_if.dmi_ranges.emplace_back(dmi);
		core3_mem_if.dmi_ranges.emplace_back(dmi);
	}

	uint64_t entry_point = loader.get_entrypoint();
	if (opt.entry_point.available)
		entry_point = opt.entry_point.value;
	try {
		loader.load_executable_image(mem, mem.size, opt.mem_start_addr);
	} catch(ELFLoader::load_executable_exception& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "Memory map: " << std::endl;
		std::cerr << opt << std::endl;
		return -1;
	}
	// core0.init(instr_mem_if, data_mem_if, &clint, entry_point, rv32_align_address(opt.mem_end_addr));
	core0.init(&core0_mem_if, &core0_mem_if, &clint, loader.get_entrypoint(),
	           opt.mem_end_addr - 3);  // -3 to not overlap with the next region and stay 32 bit aligned
	core1.init(&core1_mem_if, &core1_mem_if, &clint, loader.get_entrypoint(), opt.mem_end_addr - 32767);
	core2.init(&core2_mem_if, &core2_mem_if, &clint, loader.get_entrypoint(), opt.mem_end_addr - 65535);
	core3.init(&core3_mem_if, &core3_mem_if, &clint, loader.get_entrypoint(), opt.mem_end_addr - 131071);


	sys.init(mem.data, opt.mem_start_addr, loader.get_heap_addr());
	sys.register_core(&core0);
	sys.register_core(&core1);
	sys.register_core(&core2);
	sys.register_core(&core3);
	if (opt.intercept_syscalls){
		core0.sys = &sys;
		core1.sys = &sys;
		core2.sys = &sys;
		core3.sys = &sys;
	}
	// address mapping
	bus.ports[0] = new PortMapping(opt.mem_start_addr, opt.mem_end_addr);
	bus.ports[1] = new PortMapping(opt.clint_start_addr, opt.clint_end_addr);
	bus.ports[2] = new PortMapping(opt.plic_start_addr, opt.plic_end_addr);
	bus.ports[3] = new PortMapping(opt.term_start_addr, opt.term_end_addr);
	bus.ports[4] = new PortMapping(opt.sensor_start_addr, opt.sensor_end_addr);
	bus.ports[5] = new PortMapping(opt.dma_start_addr, opt.dma_end_addr);
	bus.ports[6] = new PortMapping(opt.sensor2_start_addr, opt.sensor2_end_addr);
	bus.ports[7] = new PortMapping(opt.mram_start_addr, opt.mram_end_addr);
	bus.ports[8] = new PortMapping(opt.flash_start_addr, opt.flash_end_addr);
	// bus.ports[9] = new PortMapping(opt.ethernet_start_addr, opt.ethernet_end_addr);
	// bus.ports[10] = new PortMapping(opt.display_start_addr, opt.display_end_addr);
	bus.ports[9] = new PortMapping(opt.sys_start_addr, opt.sys_end_addr);
	bus.ports[10] = new PortMapping(opt.MergeLV1_p0_start_addr, opt.MergeLV1_p0_end_addr);
	bus.ports[11] = new PortMapping(opt.MergeLV1_p1_start_addr, opt.MergeLV1_p1_end_addr);
	bus.ports[12] = new PortMapping(opt.MergeLV1_p2_start_addr, opt.MergeLV1_p2_end_addr);
	bus.ports[13] = new PortMapping(opt.MergeLV1_p3_start_addr, opt.MergeLV1_p3_end_addr);
	bus.ports[14] = new PortMapping(opt.MergeLV2_p0_start_addr, opt.MergeLV2_p0_end_addr);
	bus.ports[15] = new PortMapping(opt.MergeLV2_p1_start_addr, opt.MergeLV2_p1_end_addr);
	bus.ports[16] = new PortMapping(opt.MergeLV3_p0_start_addr, opt.MergeLV3_p0_end_addr);

	// connect TLM sockets
	core0_mem_if.isock.bind(bus.tsocks[0]);
	core1_mem_if.isock.bind(bus.tsocks[1]);
	core2_mem_if.isock.bind(bus.tsocks[2]);
	core3_mem_if.isock.bind(bus.tsocks[3]);
	dbg_if.isock.bind(bus.tsocks[5]);//core

	PeripheralWriteConnector dma_connector("SimpleDMA-Connector");  // to respect ISS bus locking
	dma_connector.isock.bind(bus.tsocks[4]);//core
	dma.isock.bind(dma_connector.tsock);
	dma_connector.bus_lock = bus_lock;

	bus.isocks[0].bind(mem.tsock);
	bus.isocks[1].bind(clint.tsock);
	bus.isocks[2].bind(plic.tsock);
	bus.isocks[3].bind(term.tsock);
	bus.isocks[4].bind(sensor.tsock);
	bus.isocks[5].bind(dma.tsock);
	bus.isocks[6].bind(sensor2.tsock);
	bus.isocks[7].bind(mram.tsock);
	bus.isocks[8].bind(flashController.tsock);
	// bus.isocks[9].bind(ethernet.tsock);
	// bus.isocks[10].bind(display.tsock);
	bus.isocks[9].bind(sys.tsock);
	bus.isocks[10].bind(mergeLV1_p0.tsock);
	bus.isocks[11].bind(mergeLV1_p1.tsock);
	bus.isocks[12].bind(mergeLV1_p2.tsock);
	bus.isocks[13].bind(mergeLV1_p3.tsock);
	bus.isocks[14].bind(mergeLV2_p0.tsock);
	bus.isocks[15].bind(mergeLV2_p1.tsock);
	bus.isocks[16].bind(mergeLV3_p0.tsock);

	// connect interrupt signals/communication
	plic.target_harts[0] = &core0;
	plic.target_harts[1] = &core1;
	plic.target_harts[2] = &core2;
	plic.target_harts[3] = &core3;

	clint.target_harts[0] = &core0;
	clint.target_harts[1] = &core1;
	clint.target_harts[2] = &core2;
	clint.target_harts[3] = &core3;
	sensor.plic = &plic;
	dma.plic = &plic;
	timer.plic = &plic;
	sensor2.plic = &plic;
	// ethernet.plic = &plic;

	std::vector<debug_target_if *> threads;
	threads.push_back(&core0);
	threads.push_back(&core1);
	threads.push_back(&core2);
	threads.push_back(&core3);

	core0.trace = opt.trace_mode;  // switch for printing instructions
	core1.trace = opt.trace_mode;  // switch for printing instructions
	core2.trace = opt.trace_mode;  // switch for printing instructions
	core3.trace = opt.trace_mode;  // switch for printing instructions

	if (opt.use_debug_runner) {
		auto server = new GDBServer("GDBServer", threads, &dbg_if, opt.debug_port);
		new GDBServerRunner("GDBRunner", server, &core0);
		new GDBServerRunner("GDBRunner", server, &core1);
		new GDBServerRunner("GDBRunner", server, &core2);
		new GDBServerRunner("GDBRunner", server, &core3);
	} else {
		new DirectCoreRunner(core0);
		new DirectCoreRunner(core1);
		new DirectCoreRunner(core2);
		new DirectCoreRunner(core3);
	}

	sc_core::sc_start();

	core0.show();
	core1.show();
	core2.show();
	core3.show();

	if (opt.test_signature != "") {
		auto begin_sig = loader.get_begin_signature_address();
		auto end_sig = loader.get_end_signature_address();

		{
			boost::io::ios_flags_saver ifs(cout);
			std::cout << std::hex;
			std::cout << "begin_signature: " << begin_sig << std::endl;
			std::cout << "end_signature: " << end_sig << std::endl;
			std::cout << "signature output file: " << opt.test_signature << std::endl;
		}

		assert(end_sig >= begin_sig);
		assert(begin_sig >= opt.mem_start_addr);

		auto begin = begin_sig - opt.mem_start_addr;
		auto end = end_sig - opt.mem_start_addr;

		ofstream sigfile(opt.test_signature, ios::out);

		auto n = begin;
		while (n < end) {
			sigfile << std::hex << std::setw(2) << std::setfill('0') << (unsigned)mem.data[n];
			++n;
		}
	}

	return 0;
}
