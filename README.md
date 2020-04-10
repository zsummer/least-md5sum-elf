# least-md5sum-elf  

计算自身md5的最小elf实现(未充分优化)   


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
gcc-8编译下  463字节  
```
zsummer@debian:~/hub/least-md5sum-elf$ sh least.sh 
as: total time in assembly: 0.004000
frag chains:

        0x560d91b21e80 .text                   231 frags

        0x560d91b21f18 .data                     2 frags

        0x560d91b21fb0 .bss                      2 frags
fixups: 12
8 mini local symbols created, 1 converted
672 md5
672 md5
463 md5
zsummer@debian:~/hub/least-md5sum-elf$ wc -c md5
463 md5
zsummer@debian:~/hub/least-md5sum-elf$ ./md5
164ee61efc0382188ac0532b215b2a72skyler@debian:~/hub/least-md5sum-elf$ 
zsummer@debian:~/hub/least-md5sum-elf$ md5sum ./md5
164ee61efc0382188ac0532b215b2a72  ./md5
zsummer@debian:~/hub/least-md5sum-elf$ ldd md5
        不是动态可执行文件
zsummer@debian:~/hub/least-md5sum-elf$ 
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

