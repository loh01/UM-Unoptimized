#ifndef OPS_INTERFACE_INCLUDED
#define OPS_INTERFACE_INCLUDED
#include <stdbool.h>
#include <stdint.h>
#include <seq.h>
#include <uarray.h>

#include "except.h"

UArray_T initialize_regs();
void free_regs(UArray_T registers);
uint32_t at_reg(UArray_T registers, unsigned index);
void update_reg(UArray_T registers, unsigned index, uint32_t word);

void decode_word(uint32_t word, uint32_t *opcode, unsigned *a, unsigned *b, 
	             unsigned *c, unsigned *lvalue);
void conditional_move(UArray_T registers, unsigned a, unsigned b, unsigned c);
void segmented_load(UArray_T registers, Seq_T mem, unsigned a, unsigned b, 
	                unsigned c);
void segmented_store(UArray_T registers, Seq_T mem, unsigned a, unsigned b, 
	                 unsigned c);
void addition(UArray_T registers, unsigned a, unsigned b, unsigned c);
void multiplication (UArray_T registers, unsigned a, unsigned b, unsigned c);
void division(UArray_T registers, unsigned a, unsigned b, unsigned c);
void bitwise_NAND(UArray_T registers, unsigned a, unsigned b, unsigned c);
void halt(Seq_T mem, uint32_t *prog_count);
void map_segment(UArray_T registers, Seq_T mem, Seq_T unmapped_seq, unsigned b, 
	             unsigned c);
void unmap_segment(UArray_T registers, Seq_T mem, Seq_T unmapped_seq, 
	               unsigned c);
void output(UArray_T registers, unsigned c);
void input(UArray_T registers, unsigned c);
void load_program(Seq_T mem, UArray_T registers, uint32_t *prog_count, 
	              unsigned b, unsigned c);
void load_value(UArray_T registers, unsigned a, unsigned lvalue);

#endif