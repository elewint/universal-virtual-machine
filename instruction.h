/**************************************************************
 *
 *                         instruction.h
 *
 *     Assignment: UM
 *     Authors:  Eli Intriligator (eintri01), Max Behrendt (mbehre01)
 *     Date:     Nov 23, 2021
 *
 *     Summary
 *     This class allows the user to execute instructions that are
 *     encoded in the first four bits of a 32-bit instruction word,
 *     performing basic operations on a set of eight registers.
 *     
 **************************************************************/
#ifndef INSTRUCTIONS_INCLUDED
#define INSTRUCTIONS_INCLUDED
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitpack.h"
#include "segment.h"

typedef uint32_t Um_instruction;
typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

/* opcode_reader
 * Purpose: Reads in an instruction and calls the appropriate function
 * Parameters: the instruction as an Um_instruction (uint32_t), a bool
 *             pointer of whether to continue running the program, and
 *             an int pointer representing the program counter
 * Returns: none
 *
 * Expected input: a valid instruction, valid bool and int pointers
 * Success output: no direct output, but instructions are successfully called
 * Failure output: exits the program if a loadprog instruction is called and
                    gives a program index that is out of bounds, or if an
                    invalid opcode is given
 */
void opcode_reader(Um_instruction instruction, bool *continue_execution,
                                               int *prog_counter);

/* cmov
 * Purpose: moves the value in register a into register b if
            register c does not equal 0
 * Parameters: 3 Um_registers named a, b, and c
 * Returns: Nothing
 *
 * Expected input: 3 valid Um_registers, each being a uint32_t
                   between 0 and 7
 * Success output: none (register a is either changed or not)
 * Failure output: none
 */
void cmov(Um_register a, Um_register b, Um_register c);

/* seg_load
 * Purpose: gets the word from segment m(registers[b]) at index registers[c]
 * Parameters: 3 Um_registers named a, b, and c
 * Returns: Nothing
 *
 * Expected input: 3 valid Um_registers, each being a uint32_t
                   between 0 and 7
 * Success output: none (register a is loaded with the value from the given
                    word in the supplied segment)
 * Failure output: none
 */
void seg_load(Um_register a, Um_register b, Um_register c);

/* seg_store
 * Purpose: stores the word in register c in segment m[register b] at
            index of register c
 * Parameters: 3 Um_registers named a, b, and c
 * Returns: Nothing
 *
 * Expected input: 3 valid Um_registers, each being a uint32_t
                   between 0 and 7
 * Success output: none (the word is properly stored in memory)
 * Failure output: none
 */
void seg_store(Um_register a, Um_register b, Um_register c);

/* add
 * Purpose: adds the values in registers b and c, mods the result by 32, and
            places that value in register a
 * Parameters: 3 Um_registers named a, b, and c
 * Returns: Nothing
 *
 * Expected input: 3 valid Um_registers, each being a uint32_t
                   between 0 and 7
 * Success output: none (register a is updated with the sum value)
 * Failure output: none
 */
void add(Um_register a, Um_register b, Um_register c);

/* multiply
 * Purpose: multiplies the values in registers b and c, mods the result by 32, 
            and places that value in register a
 * Parameters: 3 Um_registers named a, b, and c
 * Returns: Nothing
 *
 * Expected input: 3 valid Um_registers, each being a uint32_t
                   between 0 and 7
 * Success output: none (register a is updated with the product)
 * Failure output: none
 */
void multiply(Um_register a, Um_register b, Um_register c);

/* divide
 * Purpose: divides the values in registers b and c, trunkating any decimals,
            and places that value in register a
 * Parameters: 3 Um_registers named a, b, and c
 * Returns: Nothing
 *
 * Expected input: 3 valid Um_registers, each being a uint32_t
                   between 0 and 7
 * Success output: none (register a is updated with the divided value)
 * Failure output: exits the program if given a divisor of 0
 */
void divide(Um_register a, Um_register b, Um_register c);

/* nand
 * Purpose: Executes a bitwise nand operation on the values in registers
            b and c, then places the result in register a
 * Parameters: 3 Um_registers named a, b, and c
 * Returns: Nothing
 *
 * Expected input: 3 valid Um_registers, each being a uint32_t
                   between 0 and 7
 * Success output: none (register a is updated with the new value)
 * Failure output: none
 */
void nand(Um_register a, Um_register b, Um_register c);

/* map_seg
 * Purpose: Creates a new segment with a number of words equal to the
            given size, and places the segment index in register b
 * Parameters: 2 Um_registers named b and c
 * Returns: Nothing
 *
 * Expected input: 2 valid Um_registers, each being a uint32_t
                   between 0 and 7
 * Success output: none (register b is updated with the proper index)
 * Failure output: none
 */
void map_seg(Um_register b, Um_register c);

/* unmap_seg
 * Purpose: unmaps the segment at the given segment rregister
 * Parameters: 1 Um_register named c
 * Returns: Nothing
 *
 * Expected input: 1 valid Um_register, a uint32_t
                   between 0 and 7
 * Success output: none (the given memory segment is unmapped)
 * Failure output: none
 */
void unmap_seg(Um_register c);

/* output
 * Purpose: outputs the value in the given register to the console
 * Parameters: 1 Um_register named c
 * Returns: Nothing
 *
 * Expected input: 1 valid Um_register, a uint32_t
                   between 0 and 7
 * Success output: none (value in register c is printed to console)
 * Failure output: Raises an exception if the value supplied is greater
                    than 255
 */
void output(Um_register c);

/* input
 * Purpose: takes in a char value from I/O and puts it in register c
 * Parameters: 1 Um_register named c
 * Returns: Nothing
 *
 * Expected input: 1 valid Um_register, a uint32_t
                   between 0 and 7
 * Success output: none (register c is updated with the value)
 * Failure output: none
 */
void input(Um_register c);

/* loadval
 * Purpose: loads the supplied value into the given register
 * Parameters: 1 Um_register named a, and a uint32_t value
 * Returns: Nothing
 *
 * Expected input: 1 valid Um_register, a uint32_t
                   between 0 and 7, and a uint32_t value that is not
                   greater than 25 bits
 * Success output: none (value is placed into register a)
 * Failure output: none
 */
void loadval(Um_register a, uint32_t value);

/* loadprog
 * Purpose: m[0] is replaced with the segment at the given index
 * Parameters: 1 Um_register named b
 * Returns: Nothing
 *
 * Expected input: 1 valid Um_register, a uint32_t
                   between 0 and 7,
 * Success output: none (m[0] is replaced (or not) accordingly)
 * Failure output: none
 */
void loadprog(Um_register b);

#endif