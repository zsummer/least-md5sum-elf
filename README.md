# least-md5sum-elf  

计算自身md5的最小elf实现


## how to use   
### the least version  
```
sh least.sh
```
这里有两个实现 一个是使用Linux Kernel Crypto API 实现, 大约最终的ELF可执行文件约500字节大小 
但是这个实现会使用到较多的int 80指令来完成和内核的数据交换     

### the second version   
```
least-no-kernel.sh
```
这个是普通模式的实现 嵌入了一个md5计算的子程序 因此除了屏幕输出外没有额外的syscall指令.   
大小约为1300字节  


### depends tools   
通过编译和链接的优化, 普通strip在这里基本上没什么用, 这里用了一个ELFKicker的sstrip工具对elf文件进一步精简,  为了减少elf的目标大小并且不 为了进一步优化elf大小  
可以剔除30%的体积    
[sstrip github url](https://github.com/BR903/ELFkickers)

### example  
gcc-8编译下  529字节  
```
zsummer@debian:~/hub/least-md5sum-elf$ sh least.sh 
as: total time in assembly: 0.004000
frag chains:

        0x55b793877e80 .text                   258 frags

        0x55b793877f18 .data                     2 frags

        0x55b793877fb0 .bss                      2 frags
fixups: 13
5 mini local symbols created, 1 converted
744 md5
744 md5
529 md5
zsummer@debian:~/hub/least-md5sum-elf$ ./md5
6bb0b34c73fc825e2b0be0314d361a4dskyler@debian:~/hub/least-md5sum-elf$ 
zsummer@debian:~/hub/least-md5sum-elf$ md5sum md5
6bb0b34c73fc825e2b0be0314d361a4d  md5
zsummer@debian:~/hub/least-md5sum-elf$ wc -c md5
529 md5
```

## 基本原理       
* 使用内核已有的md5计算来完成操作   
* 去除libc的依赖 包括main函数以及常用的api 直接使用syscall来完成   
* 根据int 80的寄存器使用手册手刻syscall汇编 (syscall也是libc提供的接口)  
* 最小化生成汇编码, 并完成_start函数替换, 剔除多余汇编内容   
* 最小化链接为目标elf文件   
* 使用sstrip进一步精简elf文件    

上述操作均为工具或者脚本自动化完成    

## 优化空间  
* 汇编重排 减少汇编指令   
* 执行无关的可利用空间进行指令填充并跳转执行  
* 删除不影响计算结果的汇编指令来减少存储体积  

  
### About The Author  
**Author**: YaweiZhang  
**mail**: yawei.zhang@foxmail.com  
**github**: https://github.com/zsummer/least-md5sum-elf  

