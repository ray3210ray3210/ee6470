all : main_sw_4core.cpp bootstrap.S
	riscv32-unknown-elf-g++ -std=c++14 main_sw_4core.cpp bootstrap.S -o main -lm -nostartfiles -march=rv32ima -mabi=ilp32
	
sim: all
	$(EE6470)/riscv-vp/vp/build/bin/tiny32-final-4core-lv3-7pe --intercept-syscalls main
		
dump-elf: all
	riscv32-unknown-elf-readelf -a main
	
dump-code: all
	riscv32-unknown-elf-objdump -D main
	
dump-comment: all
	objdump -s --section .comment main
	
clean:
	rm -f main
