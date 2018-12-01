#include <stdint.h>
#include <stdio.h>

#include <assert.h>
#include <seq.h>
#include "bitpack.c"


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;

/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc) 
{
        Um_instruction instruction = 0;

        instruction = Bitpack_newu((uint64_t)instruction, 3, 0, (uint64_t)rc);
        instruction = Bitpack_newu((uint64_t)instruction, 3, 3, (uint64_t)rb);
        instruction = Bitpack_newu((uint64_t)instruction, 3, 6, (uint64_t)ra);
        instruction = Bitpack_newu((uint64_t)instruction, 4, 28, (uint64_t)op);

        return instruction;
}

/* Wrapper functions for each of the instructions */
typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction conditional_move(Um_register a, Um_register b, 
        Um_register c) 
{
        return three_register(CMOV, a, b, c);
}

static inline Um_instruction segment_load(Um_register a, Um_register b, 
        Um_register c) 
{
        return three_register(SLOAD, a, b, c);
}

static inline Um_instruction segment_store(Um_register a, Um_register b, 
        Um_register c) 
{
        return three_register(SSTORE, a, b, c);
}

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction multiply(Um_register a, Um_register b, 
        Um_register c) 
{
        return three_register(MUL, a, b, c);
}

static inline Um_instruction divide(Um_register a, Um_register b, 
        Um_register c) 
{
        return three_register(DIV, a, b, c);
}

static inline Um_instruction bit_nand(Um_register a, Um_register b, 
        Um_register c) 
{
        return three_register(NAND, a, b, c);
}

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

static inline Um_instruction map_seg(Um_register b, Um_register c) 
{
        return three_register(ACTIVATE, 0, b, c);
}

static inline Um_instruction unmap_seg(Um_register c) 
{
        return three_register(INACTIVATE, 0, 0, c);
}

static inline Um_instruction output(Um_register c) 
{
        return three_register(OUT, 0, 0, c);
}

static inline Um_instruction input(Um_register c) 
{
        return three_register(IN, 0, 0, c);
}

static inline Um_instruction load_pro(Um_register b, Um_register c) 
{
        return three_register(LOADP, 0, b, c);
}


Um_instruction loadval(unsigned ra, unsigned val) 
{
        Um_instruction instruction = 0;

        instruction = Bitpack_newu((uint64_t)instruction, 25, 0, (uint64_t)val);
        instruction = Bitpack_newu((uint64_t)instruction, 3, 25, (uint64_t)ra);
        instruction = Bitpack_newu((uint64_t)instruction, 4, 28, (uint64_t)13);

        return instruction;
}

/* Functions for working with streams */
static inline void emit(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

extern void Um_write_sequence(FILE *output, Seq_T stream) 
{
        int length = Seq_length(stream);
        for (int i = 0; i < length; i++) {
                uint32_t word = (uintptr_t)Seq_get(stream, i);

                for (int j = 3; j >= 0; j--) {
                        uint32_t val = (uint32_t)Bitpack_getu(word, 8, j * 8);
                        putc(val, output);
                } 
        }
}

/* Unit tests for the UM */

void emit_halt_test(Seq_T stream)
{
        emit(stream, halt());
}

void emit_add_test(Seq_T stream) 
{
        emit(stream, loadval(r2, 1));
        emit(stream, loadval(r3, 2));
        emit(stream, add(r1, r2, r3));
        emit(stream, output(r1));
        emit(stream, halt());
}

void emit_verbose_halt_test(Seq_T stream)
{
        emit(stream, halt());
        emit(stream, loadval(r1, 'B'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, 'a'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, 'd'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, '!'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, '\n'));
        emit(stream, output(r1));
}

void emit_digit_test(Seq_T stream) 
{
        emit(stream, loadval(r1, 48));
        emit(stream, loadval(r2, 6));
        emit(stream, add(r3, r1, r2));
        emit(stream, output(r3));
        emit(stream, halt());
}

void emit_conditional_move_test(Seq_T stream) 
{
        emit(stream, loadval(r1, 2));
        emit(stream, loadval(r2, 3));
        emit(stream, loadval(r3, 0));
        emit(stream, output(r1));
        emit(stream, output(r2));
        emit(stream, conditional_move(r1, r2, r3));
        emit(stream, output(r1));
        emit(stream, output(r2));
        emit(stream, loadval(r3, 1));
        emit(stream, conditional_move(r1, r2, r3));
        emit(stream, output(r1));
        emit(stream, output(r2));
        emit(stream, halt());
}

void emit_io_test(Seq_T stream) 
{
        emit(stream, input(r3));
        emit(stream, output(r3));
        emit(stream, halt());
}

void emit_advanced_test(Seq_T stream) 
{
        emit(stream, input(r3));
        emit(stream, loadval(r2, 15));
        emit(stream, bit_nand(r1, r2, r3));
        emit(stream, bit_nand(r1, r1, r1));
        emit(stream, output(r1));
        emit(stream, loadval(r4, 48));
        emit(stream, add(r5, r1, r4));
        emit(stream, output(r5));
        emit(stream, halt());
}

void emit_multiplication_test(Seq_T stream) 
{
        emit(stream, loadval(r3, 5));
        emit(stream, loadval(r2, 20));
        emit(stream, multiply(r1, r2, r3));
        emit(stream, output(r1));
        emit(stream, halt());
}

void emit_division_test(Seq_T stream) 
{
        emit(stream, loadval(r3, 2));
        emit(stream, loadval(r2, 200));
        emit(stream, divide(r1, r2, r3));
        emit(stream, output(r1));
        emit(stream, halt());
}

void emit_unmap_test(Seq_T stream) 
{
        emit(stream, loadval(r2, 4));
        emit(stream, loadval(r3, 5));
        emit(stream, map_seg(r2, r3));
        emit(stream, loadval(r2, 2));
        emit(stream, map_seg(r2, r3));
        emit(stream, output(r2));
        emit(stream, unmap_seg(r2));
        emit(stream, map_seg(r2, r3));
        emit(stream, output(r2));
        emit(stream, halt());
}

void emit_segments_test(Seq_T stream) 
{
        for (int i = 1; i < 100; i++) {
                emit(stream, loadval(r0, 50 * i));
                emit(stream, loadval(r1, 10));
                emit(stream, loadval(r3, 40));
                emit(stream, map_seg(r2, r0));
                emit(stream, segment_load(r1, r2, r3));
        }

        for (int i = 2; i < 30; i+=2) {
                emit(stream, loadval(r2, i * 2 / 3));
                emit(stream, unmap_seg(r2));
        }

        for (int i = 1; i < 20; i++) {
                emit(stream, loadval(r0, 50 * i));
                emit(stream, map_seg(r2, r0));
        }

        emit(stream, halt());
}

void emit_load_pro_test(Seq_T stream) 
{
        emit(stream, loadval(r0, 50));
        emit(stream, map_seg(r1, r0));
        emit(stream, loadval(r2, 0));
        emit(stream, load_pro(r1, r2));
        emit(stream, halt());
}

void emit_five_hundred_k_test(Seq_T stream) 
{
        for (int i = 0; i < 500000; i++) {
                emit(stream, loadval(r0, 100));
                emit(stream, map_seg(r1, r0));
        }
}