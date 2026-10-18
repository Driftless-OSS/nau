CFLAGS := -I./arch/include -I./kernel/include -mgeneral-regs-only
QEMUFLAGS := -m 1G

all:

pre_setup:
	mkdir -p build
	
sgexy: pre_setup
	arm-none-eabi-as -march=armv7-a -mcpu=cortex-a9 arch/arm32/crt0.S -o build/crt0.o
	arm-none-eabi-as -march=armv7-a -mcpu=cortex-a9 arch/arm32/vectors.S -o build/bvec.o
	arm-none-eabi-gcc -ffreestanding -Wall -Wextra -Werror $(CFLAGS) -mcpu=cortex-a9 -marm -c arch/arm32/exception.c -o build/bex.o
	
	arm-none-eabi-gcc -ffreestanding -Wall -Wextra -Werror $(CFLAGS) -c platform/sgexy/serial.c -o build/pserial.o
	arm-none-eabi-gcc -ffreestanding -Wall -Wextra -Werror $(CFLAGS) -DFALLBACK_RAM_BASE=0x40000000ULL -DFALLBACK_RAM_SIZE=0x40000000ULL -c kernel/init.c -o build/kinit.o
	arm-none-eabi-gcc -ffreestanding -Wall -Wextra -Werror $(CFLAGS) -c kernel/debug.c -o build/kdbg.o
	arm-none-eabi-gcc -ffreestanding -Wall -Wextra -Werror $(CFLAGS) -c kernel/fdt.c -o build/kfdt.o
	arm-none-eabi-ld -T platform/sgexy/link.ld build/crt0.o build/bvec.o build/bex.o build/pserial.o build/kinit.o \
	build/kdbg.o build/kfdt.o -o build/kernel.elf
	qemu-system-arm -M smdkc210 -cpu cortex-a9 -kernel build/kernel.elf -nographic $(QEMUFLAGS)
	# qemu-system-arm -M smdkc210 -cpu cortex-a9 -kernel build/kernel.elf -display none -serial file:out.txt -monitor none -m 1G

qvirt: pre_setup
	aarch64-linux-gnu-as arch/arm64/crt0.S -o build/crt0.o
	aarch64-linux-gnu-as arch/arm64/vectors.S -o build/bvec.o
	aarch64-linux-gnu-gcc -ffreestanding -nostdlib -nostartfiles -Wall -Wextra -Werror -I./arch/include -I./kernel/include \
	-mgeneral-regs-only -c arch/arm64/exception.c -o build/bex.o
	
	aarch64-linux-gnu-gcc -ffreestanding -nostdlib -nostartfiles -Wall -Wextra -Werror $(CFLAGS) -c platform/qvirt/serial.c -o build/pserial.o
	aarch64-linux-gnu-gcc -ffreestanding -nostdlib -nostartfiles -Wall -Wextra -Werror $(CFLAGS) -DDTB_FALLBACK=0x40000000UL -c kernel/init.c -o build/kinit.o
	aarch64-linux-gnu-gcc -ffreestanding -nostdlib -nostartfiles -Wall -Wextra -Werror $(CFLAGS) -c kernel/debug.c -o build/kdbg.o
	aarch64-linux-gnu-gcc -ffreestanding -nostdlib -nostartfiles -Wall -Wextra -Werror $(CFLAGS) -c kernel/fdt.c -o build/kfdt.o
	aarch64-linux-gnu-ld -nostdlib -T platform/qvirt/link.ld build/crt0.o build/bvec.o build/bex.o build/pserial.o build/kinit.o build/kdbg.o \
	build/kfdt.o -o build/kernel.elf
	qemu-system-aarch64 -machine virt -cpu cortex-a57 -kernel build/kernel.elf -nographic $(QEMUFLAGS)

ipod-classic-6g: pre_setup
	@echo "WARN: Support for the iPod Classic 6G is currently limited."

	arm-none-eabi-as -mcpu=arm926ej-s arch/arm5/crt0.S -o build/crt0.o
	arm-none-eabi-as -mcpu=arm926ej-s arch/arm5/vectors.S -o build/bvec.o
	arm-none-eabi-gcc -ffreestanding -Wall -Wextra -Werror $(CFLAGS) -mcpu=arm926ej-s -marm -c arch/arm5/exception.c -o build/bex.o
	
	arm-none-eabi-gcc -ffreestanding -Wall -Wextra -Werror $(CFLAGS) -c platform/ipc-6g/serial.c -o build/pserial.o
	arm-none-eabi-gcc -ffreestanding -Wall -Wextra -Werror $(CFLAGS) -DFALLBACK_RAM_BASE=0x08000000ULL -DFALLBACK_RAM_SIZE=0x4000000ULL \
	-c kernel/init.c -o build/kinit.o
	arm-none-eabi-gcc -ffreestanding -Wall -Wextra -Werror $(CFLAGS) -c kernel/debug.c -o build/kdbg.o
	arm-none-eabi-gcc -ffreestanding -Wall -Wextra -Werror $(CFLAGS) -c kernel/fdt.c -o build/kfdt.o
	arm-none-eabi-ld -T platform/ipc-6g/link.ld build/crt0.o build/bvec.o build/bex.o build/pserial.o build/kinit.o \
	build/kdbg.o build/kfdt.o -o build/kernel.elf
	arm-none-eabi-objcopy -O binary build/kernel.elf build/kernel.bin
	
	../qemu-ipod-classic/build/qemu-system-arm -machine iPod-Classic -kernel build/kernel.bin -serial null

ipod-touch-4g:
	@echo "NAU support for the iPod Touch 4G is currently not available."

ipod-touch-2g:
	@echo "NAU support for the iPod Touch 2G is currently not available."

clean:
	rm -rf build
