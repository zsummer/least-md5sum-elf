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
    for (int i = 0; i < 16; i++) 
    {
        hex[0] = hex_table[(text[i] >> 4) & 0xf];
        hex[1] = hex_table[(text[i]) & 0xf];
        syscall(SYS_write, 1, hex, 2);
    }
    syscall(SYS_exit, 0);
}
