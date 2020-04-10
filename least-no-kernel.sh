#/bin/bash
gcc -S  no-kernel.c -fno-asynchronous-unwind-tables -Os -ffast-math -fomit-frame-pointer -fsingle-precision-constant -fno-verbose-asm  -nodefaultlibs
sed -i -e "s/main/_start/g" no-kernel.s
sed -i -e "s/\.size.*//g" no-kernel.s
sed -i -e "s/\.ident.*//g" no-kernel.s
sed -i -e "s/\.section.*//g" no-kernel.s
as --64 -ac -ad -an  --statistics -o no-kernel.o no-kernel.s syscall.s 
ld -N --no-demangle -x -s -Os --cref  -o no-kernel no-kernel.o 
gcc -Os -fdata-sections -ffunction-sections -flto  no-kernel.s syscall.s -o no-kernel2 -Wl,--gc-sections -Wl,--strip-all -Wl,-N -Wl,-x -Wl,-s  -nostdlib -nostdinc 
wc -c no-kernel
strip no-kernel
wc -c no-kernel
./sstrip  no-kernel
wc -c no-kernel


