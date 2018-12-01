#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <seq.h>
#include <uarray.h>

#include "mem_interface.h"
#include "ops_interface.h"
#include "io_dev.h"
#include "bitpack.h"
#include "except.h"

/* Function: initialize_regs
 * Does: Initializes registers
 * Paramters: None
 * Returns: UArray_T
 */
UArray_T initialize_regs()
{
        UArray_T registers = UArray_new(8, sizeof(uint32_t));

        /* Sets the values of all registers to 0 */
        for (int i = 0; i < 8; i++) {
            uint32_t* value = (uint32_t*)UArray_at(registers, i);
            *value = 0;
        }

        return registers;
}

/* Function: free_regs
 * Does: Frees registers
 * Paramters: UArray_T
 * Returns: None
 */
void free_regs(UArray_T registers)
{
        if (registers == NULL) {
                fprintf(stdout, "Error: Reigsters not initialized");  
                exit(EXIT_FAILURE);
        }  
        UArray_free(&registers);
}

/* Function: free_regs
 * Does: Frees registers
 * Paramters: UArray_T
 * Returns: None
 */
uint32_t at_reg(UArray_T registers, unsigned index)
{
        if (registers == NULL) {
                fprintf(stdout, "Error: Reigsters not initialized");
                exit(EXIT_FAILURE);  
        }       
        return *((uint32_t *)UArray_at(registers, index));
}

/* Function: update_reg
 * Does: Updates a specified register to a specified value
 * Paramters: UArray_T, unsigned, uint32_t
 * Returns: None
 */
void update_reg(UArray_T registers, unsigned index, uint32_t word)
{ 
        if (registers == NULL) {
                fprintf(stdout, "Error: Reigsters not initialized");  
                exit(EXIT_FAILURE);
        }

        uint32_t *curr_reg = (uint32_t *)UArray_at(registers, index);
        *curr_reg = word;
}

/* Function: decode_word
 * Does: Decodes a given word and puts the appropriate bits into
         appropriate parameters
 * Paramters: uint32_t, uint32_t*, unsigned*, unsigned*, unsigned*, unsigned*
 * Returns: None
 */
void decode_word(uint32_t word, uint32_t *opcode, unsigned *a, unsigned *b, 
                 unsigned *c, unsigned *lvalue)
{
        *opcode = (uint32_t)Bitpack_getu(word, 4, 28);

        if (*opcode == 13) {
                *a = (unsigned)Bitpack_getu(word, 3, 25);
                *b = 0;
                *c = 0;
                *lvalue = (unsigned)Bitpack_getu(word, 25, 0);
        } else {
                *a = (unsigned)Bitpack_getu(word, 3, 6);
                *b = (unsigned)Bitpack_getu(word, 3, 3);
                *c = (unsigned)Bitpack_getu(word, 3, 0);
                *lvalue = 0;
        }
}

/* Function: conditional move
 * Does: Performs a conditional move
 * Paramters: UArray_T, unsigned, unsigned, unsigned
 * Returns: None
 */
void conditional_move(UArray_T registers, unsigned a, unsigned b, unsigned c)
{
        if (a > 7 || b > 7 || c > 7) {
                fprintf(stdout, "Error: Invalid register index provided");
                exit(EXIT_FAILURE);
        }

        if (registers == NULL) {
                fprintf(stdout, "Error: Memory not initialized");  
                exit(EXIT_FAILURE);
        }

        if (at_reg(registers, c) == 0) {
                return;
        } else {
                uint32_t temp = at_reg(registers, b);
                update_reg(registers, a, temp);
        }
}

/* Function: segmented_load
 * Does: Performs a segmented load
 * Paramters: UArray_T, Seq_T, unsigned, unsigned
 * Returns: None
 */
void segmented_load(UArray_T registers, Seq_T mem, unsigned a, unsigned b, 
                    unsigned c)
{
        if (a > 7 || b > 7 || c > 7) {
                fprintf(stdout, "Error: Invalid register index provided");
                exit(EXIT_FAILURE);
        }

        if (registers == NULL || mem == NULL) {
                fprintf(stdout, "Error: Memory not initialized");  
                exit(EXIT_FAILURE);
        }
        
        unsigned val_b = at_reg(registers, b);
        unsigned val_c = at_reg(registers, c);

        uint32_t new_word = get_word(mem, val_b, val_c);
        update_reg(registers, a, new_word); 
}

/* Function: segmented_store
 * Does: Performs a segmented store
 * Paramters: UArray_T, Seq_T, unsigned, unsigned
 * Returns: None
 */
void segmented_store(UArray_T registers, Seq_T mem, unsigned a, unsigned b, 
                     unsigned c)
{
        if (a > 7 || b > 7 || c > 7) {
                fprintf(stdout, "Error: Invalid register index provided");
                exit(EXIT_FAILURE);
        }

        if (registers == NULL || mem == NULL) {
                fprintf(stdout, "Error: Memory not initialized");  
                exit(EXIT_FAILURE);
        }
        
        unsigned val_a = at_reg(registers, a);
        unsigned val_b = at_reg(registers, b);
        unsigned val_c = at_reg(registers, c);

        put_word(mem, val_a, val_b, val_c);
}

/* Function: addition
 * Does: Performs an addition
 * Paramters: UArray_T, unsigned, unsigned, unsigned
 * Returns: None
 */
void addition(UArray_T registers, unsigned a, unsigned b, unsigned c)
{
        if (a > 7 || b > 7 || c > 7) {
                fprintf(stderr, "Error: Invalid register index provided");
                exit(EXIT_FAILURE);
        }

        if (registers == NULL) {
                fprintf(stdout, "Error: Memory not initialized");  
                exit(EXIT_FAILURE);
        }

        uint32_t first = at_reg(registers, b);
        uint32_t second = at_reg(registers, c);
        uint32_t sum = (first + second) % 4294967296;

        update_reg(registers, a, sum);
}

/* Function: multiplication
 * Does: Performs a multiplication
 * Paramters: UArray_T, unsigned, unsigned, unsigned
 * Returns: None
 */
void multiplication (UArray_T registers, unsigned a, unsigned b, unsigned c)
{
        if (a > 7 || b > 7 || c > 7) {
                fprintf(stdout, "Error: Invalid register index provided");
                exit(EXIT_FAILURE);
        }

        if (registers == NULL) {
                fprintf(stdout, "Error: Memory not initialized");  
                exit(EXIT_FAILURE);
        }

        uint32_t first = at_reg(registers, b);
        uint32_t second = at_reg(registers, c);
        uint32_t product = (first * second) % 4294967296;

        update_reg(registers, a, product);
}

/* Function: division
 * Does: Performs a division
 * Paramters: UArray_T, unsigned, unsigned, unsigned
 * Returns: None
 */
void division(UArray_T registers, unsigned a, unsigned b, unsigned c)
{
        if (a > 7 || b > 7 || c > 7) {
                fprintf(stdout, "Error: Invalid register index provided");
                exit(EXIT_FAILURE);
        }

        if (registers == NULL) {
                fprintf(stdout, "Error: Memory not initialized");
                exit(EXIT_FAILURE);  
        }

        uint32_t first = at_reg(registers, b);
        uint32_t second = at_reg(registers, c);
        uint32_t quotient = (first / second) % 4294967296;

        update_reg(registers, a, quotient);
}

/* Function: bitwise_NAND
 * Does: Performs a bitwise NAND
 * Paramters: UArray_T, unsigned, unsigned, unsigned
 * Returns: None
 */
void bitwise_NAND(UArray_T registers, unsigned a, unsigned b, unsigned c)
{
        if (a > 7 || b > 7 || c > 7) {
                fprintf(stdout, "Error: Invalid register index provided");
                exit(EXIT_FAILURE);
        }

        if (registers == NULL) {
                fprintf(stdout, "Error: Memory not initialized");  
                exit(EXIT_FAILURE);
        }

        uint32_t first = at_reg(registers, b);
        uint32_t second = at_reg(registers, c);
        uint32_t result = first & second;
        result = ~result;
        update_reg(registers, a, result);
}

/* Function: halt
 * Does: Halts the program
 * Paramters: Seq_T, uint32_t*
 * Returns: None
 */
void halt(Seq_T mem, uint32_t *prog_count)
{
        if (mem == NULL) {
                fprintf(stdout, "Error: Memory not initialized"); 
                exit(EXIT_FAILURE); 
        }
        
        mem_seg prog_seg = (mem_seg)Seq_get(mem, 0);
        *prog_count = ((uint32_t)UArray_length(prog_seg->words));
}

/* Function: map_segment
 * Does: Maps a segment with a specified number of words
 * Paramters: UArray_T, Seq_T, Seq_T, unsigned, unsigned
 * Returns: None
 */
void map_segment(UArray_T registers, Seq_T mem, Seq_T unmapped_seq, unsigned b, 
                 unsigned c)
{
        if (b > 7 || c > 7) {
                fprintf(stdout, "Error: Invalid register index provided");
                exit(EXIT_FAILURE);
        }

        if (registers == NULL || mem == NULL || unmapped_seq == NULL) {
                fprintf(stdout, "Error: Memory not initialized");  
                exit(EXIT_FAILURE);
        }
        
        unsigned val_c = at_reg(registers, c);

        uint32_t index = mem_map_segment(mem, unmapped_seq, val_c);
        update_reg(registers, b, index);
}

/* Function: unmap_segment
 * Does: Maps the segment at a specified index
 * Paramters: UArray_T, Seq_T, Seq_T, unsigned
 * Returns: None
 */
void unmap_segment(UArray_T registers, Seq_T mem, Seq_T unmapped_seq, 
                   unsigned c)
{
        if (c > 7) {
                fprintf(stdout, "Error: Invalid register index provided");
                exit(EXIT_FAILURE);
        }

        if (registers == NULL || mem == NULL || unmapped_seq == NULL) {
                fprintf(stdout, "Error: Memory not initialized");  
                exit(EXIT_FAILURE);
        }
        
        unsigned index = (unsigned)at_reg(registers, c);

        if (index == 0) {
                fprintf(stdout, "Error: Cannot unmap segment 0");  
                exit(EXIT_FAILURE);
        }

        mem_unmap_segment(mem, unmapped_seq, index);
}

/* Function: output
 * Does: Outputs the value at the given register
 * Paramters: UArray_T, unsigned
 * Returns: None
 */
void output(UArray_T registers, unsigned c)
{
        if (c > 7) {
                fprintf(stdout, "Error: Invalid register index provided");
                exit(EXIT_FAILURE);
        }

        if (registers == NULL) {
                fprintf(stdout, "Error: Memory not initialized");  
                exit(EXIT_FAILURE);
        }

        io_output(at_reg(registers, c));
}

/* Function: input
 * Does: Takes input and stores it in the given register
 * Paramters: UArray_T, unsigned
 * Returns: None
 */
void input(UArray_T registers, unsigned c)
{
        if (c > 7) {
                fprintf(stdout, "Error: Invalid register index provided");
                exit(EXIT_FAILURE);
        }

        if (registers == NULL) {
                fprintf(stdout, "Error: Memory not initialized");  
                exit(EXIT_FAILURE);
        }

        uint32_t userinput = io_input();
        if (userinput == (unsigned)EOF) {
                userinput = ~0;      
        } 

        update_reg(registers, c, userinput);
}

/* Function: load_program
 * Does: Loads the sgment at the spcified index into the program
 * Paramters: Seq_T, UArray_T, uint32_t*, unsigned, unsigned
 * Returns: None
 */
void load_program(Seq_T mem, UArray_T registers, uint32_t *prog_count, 
                  unsigned b, unsigned c)
{
        if (b > 7 || c > 7) {
                fprintf(stdout, "Error: Invalid register index provided");
                exit(EXIT_FAILURE);
        }

        if (registers == NULL || mem == NULL) {
                fprintf(stdout, "Error: Memory not initialized");  
                exit(EXIT_FAILURE);
        }
        
        uint32_t seg_num = at_reg(registers, b);

        if (seg_num == 0) {
                *prog_count = at_reg(registers, c);
                return;
        }

        /* Makes a deep copy of the segment to be duplicated*/
        mem_seg to_duplicate = (mem_seg)Seq_get(mem, seg_num);
        mem_seg duplicate = malloc(sizeof(*duplicate));
        duplicate->mapped = to_duplicate->mapped;

        int length = UArray_length(to_duplicate->words);
        duplicate->words = UArray_new(length, sizeof(uint32_t));

        /* Copies each word */
        for (int i = 0; i < length; i++) {
                uint32_t *temp_duplicate = UArray_at(duplicate->words, i);
                uint32_t *temp_to_duplicate = UArray_at(to_duplicate->words, i);
                *temp_duplicate = *temp_to_duplicate;
        }
        /* Abandons the original program segment */
        mem_seg abandoned = Seq_remlo(mem);
        UArray_free(&(abandoned->words));
        free(abandoned);

        Seq_addlo(mem, duplicate);

        *prog_count = at_reg(registers, c);
}

/* Function: load_value
 * Does: Loadsa the given value to the given register
 * Paramters: UArray_T, unsigned, unsigned
 * Returns: None
 */
void load_value(UArray_T registers, unsigned a, unsigned lvalue)
{
        if (a > 7) {
                fprintf(stderr, "Error: Invalid register index provided");
                exit(EXIT_FAILURE);
        }

        if (registers == NULL) {
                fprintf(stderr, "Error: Memory not initialized");  
        }

        update_reg(registers, a, lvalue);
}