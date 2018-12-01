#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <seq.h>
#include <uarray.h>
#include <except.h>
#include <bitpack.h>

#include "io_dev.h"
#include "mem_interface.h"
#include "ops_interface.h"

void run_prog(Seq_T mem, Seq_T unmapped_seq, UArray_T registers, 
              uint32_t *prog_count);

int main(int argc, char *argv[]) {
        if (argc == 1) {
                fprintf(stdout, "Error: A UM file not provided\n");
                exit(EXIT_FAILURE);
        }

        FILE *fp;
        fp = fopen(argv[1], "r");

        /* Checks if the file is read */
        if (fp == NULL) {
                fprintf(stderr, "%s: %s %s %s\n",
                        argv[0], "Could not open file ",
                        argv[1], "for reading");
                exit(EXIT_FAILURE);
        }

        /* Main UM components */
        Seq_T mem;
        Seq_T unmapped_seq;
        UArray_T registers;
        uint32_t prog_count = 0;

        /* Initializes main UM components */
        registers = initialize_regs();
        unmapped_seq = init_unmapped_seq();
        mem = init_mem();
        init_prog(mem, fp);

        /* Runs the UM */
        run_prog(mem, unmapped_seq, registers, &prog_count);

        /* Frees memory */
        fclose(fp);
        free_mem(mem);
        free_unmapped_seq(unmapped_seq);
        free_regs(registers);

        exit(EXIT_SUCCESS);
}

/* Function: run_program
 * Does: Runs all instructions
 * Paramters: Seq_T, Seq_T, UArray_T, uint32_t*
 * Returns: none
 */
void run_prog(Seq_T mem, Seq_T unmapped_seq, UArray_T registers, 
              uint32_t *prog_count) {
        bool exit_condition = false;

        /* Keeps running until the program counter points to the last 
         * instruction 
         */
        while (!exit_condition) {
                uint32_t instruction = get_word(mem, 0, *prog_count);
                uint32_t opcode;
                unsigned a, b, c, lvalue;
                decode_word(instruction, &opcode, &a, &b, &c, &lvalue);

                *prog_count = *prog_count + 1; 

                /* Executes the specified instruction */
                switch (opcode) {
                        case 0 :
                                conditional_move(registers, a, b, c);
                                break;
                        case 1 :
                                segmented_load(registers, mem, a, b, c);
                                break;
                        case 2 :
                                segmented_store(registers, mem, a, b, c);
                                break;
                        case 3 :
                                addition(registers, a, b, c);
                                break;
                        case 4 :
                                multiplication(registers, a, b, c);
                                break;
                        case 5 :
                                division(registers, a, b, c);
                                break;
                        case 6 :
                                bitwise_NAND(registers, a, b, c);
                                break;
                        case 7 :
                                halt(mem, prog_count);
                                break;
                        case 8 :
                                map_segment(registers, mem, unmapped_seq, b, c);
                                break;
                        case 9 :
                                unmap_segment(registers, mem, unmapped_seq, c);
                                break;
                        case 10 :
                                output(registers, c);
                                break;
                        case 11 :
                                input(registers, c);
                                break;
                        case 12 :
                                load_program(mem, registers, prog_count, b, c);
                                break;
                        case 13 :
                                load_value(registers, a, lvalue);
                                break;
                        default:
                                fprintf(stderr, "Error: Invalid Instruction\n");
                                exit(EXIT_FAILURE);

                }

                mem_seg prog_seg = (mem_seg)Seq_get(mem, 0);
                uint32_t curr_length = (uint32_t)UArray_length(prog_seg->words);

                /* Check if the last instruction has been executed*/
                if (*prog_count == curr_length) {
                        exit_condition = true;
                } 

        }
}