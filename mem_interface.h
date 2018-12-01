#ifndef MEM_INTERFACE_INCLUDED
#define MEM_INTERFACE_INCLUDED
#include <stdbool.h>
#include <stdint.h>
#include <seq.h>
#include <uarray.h>
#include "except.h"

typedef struct mem_seg {
        unsigned mapped;
        UArray_T words;
} *mem_seg;

Seq_T init_mem();
void init_prog(Seq_T mem, FILE *fp);
Seq_T init_unmapped_seq();
uint32_t mem_map_segment(Seq_T mem, Seq_T unmapped_seq, unsigned num_words);
void mem_unmap_segment(Seq_T mem, Seq_T unmapped_seq, unsigned index);
uint32_t get_word(Seq_T mem, unsigned seg_num, unsigned offset);
void put_word(Seq_T mem, unsigned seg_num, unsigned offset, uint32_t val);
void free_mem(Seq_T mem);
void free_unmapped_seq(Seq_T unmapped_seq);

#endif