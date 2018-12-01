#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <seq.h>
#include <uarray.h>

#include "mem_interface.h"
#include "bitpack.h"
#include "except.h"

Seq_T init_mem() 
{
        Seq_T mem = Seq_new(0);
        mem_seg prog_seg = malloc(sizeof(*prog_seg));
        prog_seg->mapped = 1;
        prog_seg->words = NULL;
        Seq_addlo(mem, prog_seg);

        return mem;
}

void init_prog(Seq_T mem, FILE *fp)
{
        int counter = 0;

        int num_words = 0;
        int reader = 1;
        int index = 0;
        uint32_t temp_ch;
        uint32_t word = 0;
        bool end = false;

        if (mem == NULL || fp == NULL) {
                fprintf(stdout, "Error: Memory/File pointer is uninitialized");
                exit(EXIT_FAILURE);
        }

        while (!end) {
                temp_ch = fgetc(fp);
                if ((int)temp_ch == EOF) {
                        end = true;
                } else {
                        counter++;
                }
        }

        num_words = counter / 4;
        mem_seg prog_seg = (mem_seg)Seq_get(mem, 0);
        prog_seg->words = UArray_new(num_words, sizeof(uint32_t));
        end = false;

        rewind(fp);

        while (!end) {
                temp_ch = fgetc(fp);

                if ((int)temp_ch == EOF) {
                        end = true;
                } else {
                        word = (uint32_t)Bitpack_newu((uint64_t)word, 8, 
                        (32 - (8 * reader)), temp_ch);

                        if (reader < 4) {
                                reader++;
                        } else {
                                uint32_t *curr_word = 
                                UArray_at(prog_seg->words, index); 
                                *curr_word = word;

                                reader = 1;
                                index++;
                        }
                }
        }
}

Seq_T init_unmapped_seq()
{
        Seq_T unmapped_seq = Seq_new(0);

        return unmapped_seq;
}

uint32_t mem_map_segment(Seq_T mem, Seq_T unmapped_seq, unsigned num_words)
{
        if (mem == NULL) {
                fprintf(stdout, "Error: Memory is uninitialized");
                exit(EXIT_FAILURE);
        }
        uint32_t new_index;

        mem_seg new_seg;

        /* Checks if there are any unmapped segments */
        if (Seq_length(unmapped_seq) != 0) {
                /* Gets the segment number of an unmapped segment */
                uint32_t* new_index_ptr = 
                ((void *)(uintptr_t)Seq_remhi(unmapped_seq));
                new_index = *new_index_ptr;
                free(new_index_ptr);
                new_seg = (mem_seg)Seq_get(mem, new_index);
                new_seg->mapped = 1;
                new_seg->words = UArray_new(num_words, sizeof(uint32_t));

        } else {
                /* Creates a new struct */
                new_seg = malloc(sizeof(*new_seg));
                new_seg->mapped = 1;
                new_seg->words = UArray_new(num_words, sizeof(uint32_t));

                Seq_addhi(mem, new_seg);
                new_index = Seq_length(mem) - 1;
        }

        /* Sets all words to 0 */
        for (unsigned i = 0; i < num_words; i++) {
                uint32_t *temp = UArray_at(new_seg->words, i);
                *temp = 0;
        }

        return new_index;
}

void mem_unmap_segment(Seq_T mem, Seq_T unmapped_seq, unsigned index)
{
        if (mem == NULL) {
                fprintf(stdout, "Error: Memory is uninitialized");
                exit(EXIT_FAILURE);
        }

        mem_seg old_seg = (mem_seg)Seq_get(mem, index);

        /* Checks if the segment is already unmapped*/
        if (old_seg->mapped != 0) {
                old_seg->mapped = 0;
                UArray_free(&(old_seg->words));
                old_seg->words = NULL;
        } else {
                fprintf(stdout, "Error: Unmapping an unmapped segment");
                exit(EXIT_FAILURE);
        }

        uint32_t *new_index = malloc(sizeof(*new_index));
        *new_index = index;
        Seq_addhi(unmapped_seq, (void *)(uintptr_t)new_index);
}

uint32_t get_word(Seq_T mem, unsigned seg_num, unsigned offset)
{
        if (mem == NULL) {
                fprintf(stdout, "Error: Memory is uninitialized");
                exit(EXIT_FAILURE);
        }

        mem_seg curr_seg = (mem_seg)Seq_get(mem, seg_num);
        uint32_t word = *(uint32_t *)UArray_at(curr_seg->words, offset);

        return word;
}

void put_word(Seq_T mem, unsigned seg_num, unsigned offset, uint32_t val)
{
        if (mem == NULL) {
                fprintf(stdout, "Error: Memory is uninitialized");
                exit(EXIT_FAILURE);
        }

        mem_seg curr_seg = (mem_seg)Seq_get(mem, seg_num);
        uint32_t *word = (uint32_t *)UArray_at(curr_seg->words, offset);
        *word = val;
}

void free_mem(Seq_T mem)
{
        if (mem == NULL) {
                fprintf(stdout, "Error: Memory is uninitialized");
                exit(EXIT_FAILURE);
        }

        int length = Seq_length(mem);

        /* Frees each mem_seg struct*/
        for (int i = 0; i < length; i++) {
                mem_seg curr_seg = (mem_seg)Seq_get(mem, i);
                /* Frees each Uarray of words */
                if ((curr_seg)->words != NULL) {
                        UArray_free(&(curr_seg->words));
                } 
                free(curr_seg);
        }
        Seq_free(&mem);
}

void free_unmapped_seq(Seq_T unmapped_seq)
{
        int length = Seq_length(unmapped_seq);

        /* Frees each number in the sequence */
        for (int i = 0; i < length; i++) {
                free((void *)((uintptr_t)Seq_get(unmapped_seq, i)));
        }

        Seq_free(&unmapped_seq);
}