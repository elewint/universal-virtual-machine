/*
 * umtest.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include "bitpack.h"

typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc);
Um_instruction loadval(unsigned ra, unsigned val);

/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

Um_instruction output(Um_register c)
{
        return three_register(OUT, 0, 0, c);
}

/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
}


/* Unit tests for the UM */

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
        append(stream, halt());
}

void build_addition_test(Seq_T stream)
{
        append(stream, loadval(r1, 'B'));
        append(stream, loadval(r2, 1));
        append(stream, add(r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
}

void build_output_test(Seq_T stream)
{
    for (int i = 0; i < 255; i++) {
        append(stream, loadval(r1, i));
        append(stream, output(r1));
    }
    append(stream, halt());
}

void build_output_edge_case_test(Seq_T stream)
{
        append(stream, loadval(r1, 300));
        append(stream, output(r1));
        append(stream, halt());
}

void build_multiply_test(Seq_T stream)
{
        append(stream, loadval(r1, 8));
        append(stream, loadval(r2, 8));
        append(stream, three_register(MUL, r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
}

void build_divide_test(Seq_T stream)
{
        append(stream, loadval(r1, 640));
        append(stream, loadval(r2, 10));
        append(stream, three_register(DIV, r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
}

void build_nand_test(Seq_T stream)
{
        // Should return 1
        append(stream, loadval(r1, 5));
        append(stream, loadval(r2, 10));
        append(stream, three_register(NAND, r3, r1, r2));

        append(stream, loadval(r1, 80));
        append(stream, add(r3, r1, r3));
        append(stream, output(r3));

        // Should return 0
        append(stream, loadval(r1, 5));
        append(stream, loadval(r2, 5));
        append(stream, three_register(NAND, r3, r1, r2));

        append(stream, loadval(r1, 80));
        append(stream, add(r3, r1, r3));
        append(stream, output(r3));
        append(stream, halt());
}

void build_input_test(Seq_T stream)
{
    for (int i = 0; i < 256; i++) {
        append(stream, three_register(IN, 0, 0, r1));
        append(stream, output(r1));
    }
    append(stream, halt());
}

void build_map_segment_test(Seq_T stream)
{
    append(stream, loadval(r1, 7));
    append(stream, three_register(ACTIVATE, 0, r2, r1));
    append(stream, loadval(r4, 80));
    append(stream, add(r2, r4, r2));
    append(stream, output(r2));
    append(stream, halt());
}

void build_unmap_segment_test(Seq_T stream)
{
        // Map a segment, output its index + 80 (should print Q)
        append(stream, loadval(r1, 7));
        append(stream, three_register(ACTIVATE, 0, r2, r1));

        append(stream, three_register(INACTIVATE, 0, 0, r2));

        append(stream, loadval(r1, 7));
        append(stream, three_register(ACTIVATE, 0, r2, r1));
        append(stream, loadval(r4, 80));
        append(stream, add(r2, r4, r2));
        append(stream, output(r2));
        append(stream, loadval(r5, '!'));
        append(stream, output(r5));
        append(stream, halt());
}

void build_conditional_move(Seq_T stream)
{
    // Should not change anything
    append(stream, loadval(r1, 80));
    append(stream, loadval(r2, 90));
    append(stream, loadval(r3, 0));  
    append(stream, three_register(CMOV, r1, r2, r3));
    append(stream, output(r1));
    
    // Should change r1 to r2
    append(stream, loadval(r3, 1));  
    append(stream, three_register(CMOV, r1, r2, r3));
    append(stream, output(r1));
    append(stream, halt());
}

void build_seg_store(Seq_T stream)
{
       append(stream, loadval(r1, 2));
       append(stream, three_register(ACTIVATE, 0, r2, r1)); 
       append(stream, loadval(r0, 0)); 
       append(stream, loadval(r3, 80));
       append(stream, three_register(SSTORE, r2, r0, r3)); 
       append(stream, three_register(SLOAD, r4, r2, r0)); 
       append(stream, output(r4));
       append(stream, halt());
}

void build_load_prog_pointer(Seq_T stream)
{
       append(stream, loadval(r1, 'A'));
       append(stream, loadval(r2, 'B'));
       append(stream, loadval(r3, 'C'));
       append(stream, loadval(r4, 'D'));
       append(stream, loadval(r5, 10));
       append(stream, output(r1));
       append(stream, output(r2));
       append(stream, loadval(r0, 0)); 
       append(stream, three_register(LOADP, 0, r0, r5)); 
       append(stream, output(r3));
       append(stream, output(r4));
       append(stream, halt());
}

void build_load_prog_full(Seq_T stream)
{
        // Map segment
       append(stream, loadval(r2, 2));
       append(stream, three_register(ACTIVATE, 0, r2, r2)); 

        // Bitpack and store output instruction
       append(stream, loadval(r1, 16777216)); //this is 2^24
       append(stream, loadval(r3, 16));
       append(stream, three_register(MUL, r1, r1, r3)); //mult by 16 to get 2^28
       append(stream, loadval(r3, 10));
       append(stream, three_register(MUL, r0, r1, r3)); //r0 = 10 x 2^28
       append(stream, loadval(r3, 7));
       append(stream, add(r0, r3, r0)); // r0 is now an output instruction, r7
       append(stream, three_register(SSTORE, r2, r6, r0)); //store our output instruction in m[r2][0]
       
        //Bitpack and store halt instruction
       append(stream, loadval(r4, 7));
       append(stream, three_register(MUL, r5, r1, r4)); //r5 = 7 x 2^28
       append(stream, loadval(r6, 1));
       append(stream, three_register(SSTORE, r2, r6, r5)); // store halt

        // r7 stores the value our loaded program will output
       append(stream, loadval(r7, 80));
       append(stream, loadval(r6, 0));
       append(stream, three_register(LOADP, 0, r2, r6)); 
}

void multiply_edge_case(Seq_T stream)
{
        append(stream, loadval(r1, 0));
        append(stream, loadval(r2, 2));
        append(stream, loadval(r3, 2));

        for (int i = 0; i < 30; i++) {
                append(stream, three_register(MUL, r1, r2, r3)); 
                append(stream, three_register(CMOV, r2, r1, r3));
        }
}

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        Um_instruction word = 0;

        word = Bitpack_newu(word, 4, 28, op);
        word = Bitpack_newu(word, 3, 6, ra);
        word = Bitpack_newu(word, 3, 3, rb);
        word = Bitpack_newu(word, 3, 0, rc);
        
        return word;
}

Um_instruction loadval(unsigned ra, unsigned val)
{
        Um_instruction word = 0;
        Um_opcode op = LV;

        word = Bitpack_newu(word, 25, 0, val);
        word = Bitpack_newu(word, 3, 25, ra);
        word = Bitpack_newu(word, 4, 28, op);
        
        return word;
}




