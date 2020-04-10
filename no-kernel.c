
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

#define BYTE unsigned char

/****************************** MACROS ******************************/
#define ROTLEFT(a, b)  ((a << b) | (a >> (32 - b)))

#define F(x, y, z)  ((x & y) | (~x & z))

#define G(x, y, z)  ((x & z) | (y & ~z))

#define H(x, y, z) (x ^ y ^ z)

#define I(x, y, z)  (y ^ (x | ~z))

static long double fsin_my(long double a) 
{
    long double res;
    // prof wiht register
    asm __volatile__("fsin\n\t"
    :"=t"(res)
    :"0"(a)
    :"memory");

    return (res) > 0 ? res : -res;
}

/*********************** FUNCTION DEFINITIONS ***********************/
void md5_compress(unsigned int *input, unsigned int *m) 
{

    unsigned int tmp[4];

    tmp[0] = input[0];
    tmp[1] = input[1];
    tmp[2] = input[2];
    tmp[3] = input[3];

    const char p1[] = {0, 3, 2, 1};
    const char mmstart[] = {0, 1, 5, 0};
    const char mmstep[] = {1, 5, 3, 7};
    const char ss[] = {7, 12, 17, 22, 5, 9, 14, 20, 4, 11, 16, 23, 6, 10, 15, 21};
    int i;
    for (i = 0; i < 64; ++i) 
    {
        unsigned int inc;
        unsigned int b = tmp[p1[(i + 3) % 4]];
        unsigned int c = tmp[p1[(i + 2) % 4]];
        unsigned int d = tmp[p1[(i + 1) % 4]];
        switch (i / 16) {
            case 0:
                inc = F(b, c, d);
                break;
            case 1:
                inc = G(b, c, d);
                break;
            case 2:
                inc = H(b, c, d);
                break;
            case 3:
                inc = I(b, c, d);
                break;
        }
        unsigned int mm = m[(mmstart[i / 16] + (i % 16) * mmstep[i / 16]) % 16];
        unsigned int s = ss[(i / 16) * 4 + (i % 4)];
        unsigned int t = (unsigned int) ((unsigned long long) 4294967296 * fsin_my(i + 1));

        tmp[p1[i % 4]] += inc + mm + t;
        tmp[p1[i % 4]] = b + ROTLEFT(tmp[p1[i % 4]], s);
    }

    input[0] += tmp[0];
    input[1] += tmp[1];
    input[2] += tmp[2];
    input[3] += tmp[3];
}

#define BLOCK_LEN 64  // In bytes
#define STATE_LEN 4  // In words
#define LENGTH_SIZE 8

void main() 
{
    char text[10*1024];
    int fself = syscall(SYS_open, "./no-kernel", O_RDONLY, 400);
    int fsize = syscall(SYS_read, fself, text, 10*1024);
    unsigned int hash[STATE_LEN];

    hash[0] = (unsigned int) (0x67452301);
    hash[1] = (unsigned int) (0xEFCDAB89);
    hash[2] = (unsigned int) (0x98BADCFE);
    hash[3] = (unsigned int) (0x10325476);
    int new_len = ((((fsize + 8) / 64) + 1) * 64) - 8;
    text[fsize] = 0x80;
    *(unsigned long long *) (text + new_len) = fsize << 3;
    int i;
    for (i = 0; i < new_len; i += BLOCK_LEN) 
    {
        md5_compress(hash, &text[i]);
    }

    const char * hex_table = "0123456789abcdef";
    char hex[2];
    const char * chash = (unsigned char*) hash;
    
    for (i = 0; i < 16; i++) 
    {
        hex[0] = hex_table[(chash[i] >> 4) & 0xf];
        hex[1] = hex_table[(chash[i]) & 0xf];
        syscall(SYS_write, 1, hex, 2);
    }
    syscall(SYS_exit, 0);
}
