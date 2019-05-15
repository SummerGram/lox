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
	echo "Y" | sudo apt install build-essential
	echo "Y" | sudo apt install bison
	echo "Y" | sudo apt install flex
	echo "Y" | sudo apt install libgmp3-dev
	echo "Y" | sudo apt install libmpc-dev
	echo "Y" | sudo apt install libmpfr-dev
	echo "Y" | sudo apt install texinfo
	echo "Y" | sudo apt install libcloog-isl-dev
	echo "Y" | sudo apt install libisl-dev
	echo "Y" | sudo apt install qemu-system-i386
	echo "Y" | sudo apt install wget
	echo "Y" | sudo apt-get update
	echo "Y" | sudo apt dist-upgrade
	echo "Y" | sudo apt autoremove
	sudo ./setup.sh
	echo " "