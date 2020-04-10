
void _start() {

    /* main body of program: call main(), etc */

    /* exit system call */
    asm("movl $1,%eax;"
        "xorl %ebx,%ebx;"
        "int  $0x80"
    );
}
