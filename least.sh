#/bin/bash
m32="-m elf_i386"

gcc-8 -S -o md5.s md5.c -fno-asynchronous-unwind-tables -Os 
sed -i -e "s/main/_start/g" md5.s
sed -i -e "s/\.size.*//g" md5.s
sed -i -e "s/\.ident.*//g" md5.s
sed -i -e "s/\.section.*//g" md5.s
as --64 -ac -ad -an  --statistics -o md5.o md5.s syscall.s 
ld -N --no-demangle  -x -s -O --cref  -o md5 md5.o 
#gcc-8  -Os -fdata-sections -ffunction-sections -flto  md5.s syscall.s -o md5 -Wl,--gc-sections -Wl,--strip-all -nostdlib -nostdinc
wc -c md5
strip md5
wc -c md5
./sstrip  md5
wc -c md5


