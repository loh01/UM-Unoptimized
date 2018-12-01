#ifndef IO_DEV_INCLUDED
#define IO_DEV_INCLUDED
#include <stdbool.h>
#include <stdint.h>
#include <seq.h>
#include <uarray.h>
#include "except.h"

uint32_t io_input();
void io_output(uint32_t word);

#endif