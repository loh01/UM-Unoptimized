#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <seq.h>
#include <uarray.h>

#include "except.h"

uint32_t io_input()
{
        char value = fgetc(stdin);
        
        return value;
}

void io_output(uint32_t word)
{
        fputc(word, stdout);
}