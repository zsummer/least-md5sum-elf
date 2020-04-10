#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <linux/if_alg.h>
#include <fcntl.h>


#define WORD unsigned int
#define BYTE unsigned char

/****************************** MACROS ******************************/
WORD ROTLEFT(WORD a, WORD b) { return ((a << b) | (a >> (32 - b))); }

WORD F(WORD x, WORD y, WORD z) { return ((x & y) | (~x & z)); }

WORD G(WORD x, WORD y, WORD z) { return ((x & z) | (y & ~z)); }

WORD H(WORD x, WORD y, WORD z) { return (x ^ y ^ z); }

WORD I(WORD x, WORD y, WORD z) { return (y ^ (x | ~z)); }

static void FF(WORD *a, WORD b, WORD c, WORD d, WORD m, WORD s, WORD t) {
    *a += F(b, c, d) + m + t;
    *a = b + ROTLEFT(*a, s);
}

static void GG(WORD *a, WORD b, WORD c, WORD d, WORD m, WORD s, WORD t) {
    *a += G(b, c, d) + m + t;
    *a = b + ROTLEFT(*a, s);
}

static void HH(WORD *a, WORD b, WORD c, WORD d, WORD m, WORD s, WORD t) {
    *a += H(b, c, d) + m + t;
    *a = b + ROTLEFT(*a, s);
}

static void II(WORD *a, WORD b, WORD c, WORD d, WORD m, WORD s, WORD t) {
    *a += I(b, c, d) + m + t;
    *a = b + ROTLEFT(*a, s);
}

/*********************** FUNCTION DEFINITIONS ***********************/
static void md5_compress(WORD *input, WORD *m) {

    WORD tmp[4];

    tmp[0] = input[0];
    tmp[1] = input[1];
    tmp[2] = input[2];
    tmp[3] = input[3];

    typedef void (*FuckFunc)(WORD *a, WORD b, WORD c, WORD d, WORD m, WORD s, WORD t);
    FuckFunc ff[] = {&FF, &GG, &HH, &II};

    const char p1[] = {0, 3, 2, 1};
    const char p2[] = {1, 0, 3, 2};
    const char p3[] = {2, 1, 0, 3};
    const char p4[] = {3, 2, 1, 0};
    const char mmstart[] = {0, 1, 5, 0};
    const char mmstep[] = {1, 5, 3, 7};
    const char ss[] = {7, 12, 17, 22, 5, 9, 14, 20, 4, 11, 16, 23, 6, 10, 15, 21};
    const WORD tt[] = {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                       0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
                       0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                       0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
                       0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                       0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
                       0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                       0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};
    for (int i = 0; i < 64; ++i) {
        FuckFunc f = ff[i / 16];
        f(&tmp[p1[i % 4]], tmp[p2[i % 4]], tmp[p3[i % 4]], tmp[p4[i % 4]],
          m[(mmstart[i / 16] + (i % 16) * mmstep[i / 16]) % 16], ss[(i / 16) * 4 + (i % 4)], tt[i]);
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

    int off;
    for (off = 0; fsize - off >= BLOCK_LEN; off += BLOCK_LEN)
    {
        md5_compress(hash, &text[off]);
    }

    unsigned char block[BLOCK_LEN] = {0};
    int rem = fsize - off;
    int i;
    for (i = 0; i < rem; i++) 
    {
        block[i] = text[off + i];
    }

    block[rem] = 0x80;
    rem++;
    if (BLOCK_LEN - rem < LENGTH_SIZE) 
    {
        md5_compress(hash, block);
        for (i = 0; i < sizeof(block); i++) 
        {
            block[i] = 0;
        }
    }

    block[BLOCK_LEN - LENGTH_SIZE] = (unsigned char) ((fsize & 0x1FU) << 3);
    fsize >>= 5;
    for (i = 1; i < LENGTH_SIZE; i++, fsize >>= 8)
    {
        block[BLOCK_LEN - LENGTH_SIZE + i] = (unsigned char) (fsize & 0xFFU);
    }
    md5_compress(hash, block);

    const char * hex_table = "0123456789abcdef";
    char hex[2];
    const char * chash = (unsigned char*) hash;
    for (int i = 0; i < 16; i++) 
    {
        hex[0] = hex_table[(chash[i] >> 4) & 0xf];
        hex[1] = hex_table[(chash[i]) & 0xf];
        syscall(SYS_write, 1, hex, 2);
    }
    syscall(SYS_exit, 0);
}
