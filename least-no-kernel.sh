#/bin/bash
gcc-8 -S -o no-kernel.s no-kernel.c -fno-asynchronous-unwind-tables -Os 
sed -i -e "s/main/_start/g" no-kernel.s
sed -i -e "s/\.size.*//g" no-kernel.s
sed -i -e "s/\.ident.*//g" no-kernel.s
sed -i -e "s/\.section.*//g" no-kernel.s
as --64 -ac -ad -an  --statistics -o no-kernel.o no-kernel.s syscall.s 
ld -N --no-demangle -x -s -Os --cref  -o no-kernel no-kernel.o 
wc -c no-kernel
strip no-kernel
wc -c no-kernel
./sstrip  no-kernel
wc -c no-kernel


