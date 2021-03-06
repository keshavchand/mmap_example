#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#define defaultSize 1024
#define asm_code {0x48,0x89,0xf8,0x48,0x83,0xc0,0x00,0xc3}


typedef int (*rFunc)(int);
rFunc func;

void * get_virtual_memory(size_t size){
    if (size < 1) size = defaultSize;
    int protection = PROT_READ | PROT_WRITE | PROT_EXEC; //We get a readable, writeable and executable memory
    int visibility = MAP_ANONYMOUS | MAP_SHARED; // Memory can be accessed by other process but mapping aren't backed by any file
    return mmap(NULL, size, protection , visibility, 0 , 0);
}

int inject_code(unsigned char code[], unsigned char * mem, int len){
    memcpy(mem , code, len );
    return 0;
}

int allocate_RWX(char * instruction, int j){
    unsigned char code[] = asm_code;
    if(strncmp("add", instruction, 3) == 0) code[5] = 0xc0;
    else if (strncmp("sub", instruction, 3) == 0) code[5] = 0xe8 ;
    else if (strncmp("mul", instruction, 3) == 0) code[4] = 0x6b ;
    else if (strncmp("and", instruction, 3) == 0) code[5] = 0xc0 ;
    else if (strncmp("xor", instruction, 3) == 0) code[5] = 0xf0 ;
    else if (strncmp("or", instruction, 2) == 0) code[5] = 0xc8 ;
    else if (strncmp("shl", instruction, 3) == 0){ code[4] = 0xc1 ; code[5] = 0xe0 ; }
    else if (strncmp("shr", instruction, 3) == 0){ code[4] = 0xc1 ; code[5] = 0xe8 ; }
    code[6] = j % 255;
    void * mem = get_virtual_memory(16);
    inject_code(code, mem , sizeof(code));
    func = mem;
    return 1;
}

int main(int argc, char * argv[]){
    printf("Instruction>> ");
    char* ins = malloc(8);
    scanf(" %3s", ins);

    int j = 0 , i = 0;
    printf("First and second value>> ");
    scanf(" %d %d", &j, &i);
    allocate_RWX(ins,j);

    printf("%d\n", func(i));

    munmap(func, 16);
    return 0;
}
