.PHONY: clean install

all: os.bin

terminal.o: terminal.c terminal.h
	i686-elf-gcc -c terminal.c -o terminal.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

kernel.o: kernel.c terminal.h terminal.c
	i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

boot.o: boot.s
	i686-elf-gcc -std=gnu99 -ffreestanding -g -c boot.s -o boot.o

os.bin: kernel.o boot.o linker.ld terminal.o
	i686-elf-gcc -T linker.ld -o os.bin -ffreestanding -O2 -nostdlib boot.o kernel.o terminal.o -lgcc

clean:
	rm *.o os.bin

install:
	@sudo apt install -y build-essential
	@sudo apt install -y bison
	@sudo apt install -y flex
	@sudo apt install -y libgmp3-dev
	@sudo apt install -y libmpc-dev
	@sudo apt install -y libmpfr-dev
	@sudo apt install -y texinfo
	@sudo apt install -y libisl-dev
	@sudo apt install -y qemu-system-i386
	@sudo apt install -y wget
	@sudo apt-get -y update
	@sudo apt -y dist-upgrade
	@sudo apt -y autoremove
	@sudo ./setup.sh
	@echo " "
