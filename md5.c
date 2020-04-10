/*
 * least-md5sum-elf License
 * -----------
 * 
 * least-md5sum-elf is licensed under the terms of the MIT license reproduced below.
 * This means that least-md5sum-elf is free software and can be used for both academic
 * and commercial purposes at absolutely no cost.
 * 
 * 
 * ===============================================================================
 * 
 * Copyright (C) 2010-2015 YaweiZhang <yawei.zhang@foxmail.com>.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 * ===============================================================================
 * 
 * (end of COPYRIGHT)
 */



#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <linux/if_alg.h>
#include <fcntl.h>

void main() 
{
    struct sockaddr_alg sa = 
    {
        .salg_family = AF_ALG,
        .salg_type = "hash",
        .salg_name = "md5"
    };
    char text[10*1024];
    int alg_fd = syscall(SYS_socket, AF_ALG, SOCK_SEQPACKET, 0);
    syscall(SYS_bind, alg_fd, (struct sockaddr *)&sa, sizeof(sa));
    int inst_fd = syscall(SYS_accept, alg_fd, NULL, 0);
    int fself = syscall(SYS_open, "./md5", O_RDONLY, 400);
    int fsize = syscall(SYS_read, fself, text, 10*1024);
    syscall(SYS_write, inst_fd, text, fsize);
    syscall(SYS_read, inst_fd, text, 20);
    const char * hex_table = "0123456789abcdef";
    char hex[2];
    int i;
    for (i = 0; i < 16; i++) 
    {
        hex[0] = hex_table[(text[i] >> 4) & 0xf];
        hex[1] = hex_table[(text[i]) & 0xf];
        syscall(SYS_write, 1, hex, 2);
    }
    syscall(SYS_exit, 0);
}
