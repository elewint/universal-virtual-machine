/**************************************************************
 *
 *                         instruction.c
 *
 *     Assignment: UM
 *     Authors:  Eli Intriligator (eintri01), Max Behrendt (mbehre01)
 *     Date:     Nov 23, 2021
 *
 *     Summary
 *     Implementation of the instruction class.
 *     
 **************************************************************/
#include "instruction.h"

typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;

uint32_t registers[8] = {0, 0, 0, 0, 0, 0, 0, 0};

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
                                               int *prog_counter)
{
    assert(continue_execution != NULL);
    assert(prog_counter != NULL);

    Um_opcode op = Bitpack_getu(instruction, 4, 28);

    if (op > 13) {
        exit(1);
    }

    switch(op) {
        case CMOV:
            cmov(Bitpack_getu(instruction, 3, 6), 
                Bitpack_getu(instruction, 3, 3),
                Bitpack_getu(instruction, 3, 0));
            return;
        case SLOAD:
            seg_load(Bitpack_getu(instruction, 3, 6), 
                Bitpack_getu(instruction, 3, 3),
                Bitpack_getu(instruction, 3, 0));
            return;
        case SSTORE:
            seg_store(Bitpack_getu(instruction, 3, 6), 
                Bitpack_getu(instruction, 3, 3),
                Bitpack_getu(instruction, 3, 0));
            return;
        case ADD:
            add(Bitpack_getu(instruction, 3, 6), 
                Bitpack_getu(instruction, 3, 3),
                Bitpack_getu(instruction, 3, 0));
            return;
        case MUL:
            multiply(Bitpack_getu(instruction, 3, 6), 
                Bitpack_getu(instruction, 3, 3),
                Bitpack_getu(instruction, 3, 0));
            return;
        case DIV:
            divide(Bitpack_getu(instruction, 3, 6), 
                Bitpack_getu(instruction, 3, 3),
                Bitpack_getu(instruction, 3, 0));
            return;
        case NAND:
            nand(Bitpack_getu(instruction, 3, 6), 
                Bitpack_getu(instruction, 3, 3),
                Bitpack_getu(instruction, 3, 0));
            return;
        case HALT:
            *continue_execution = false;
            return;
        case ACTIVATE:
            map_seg(Bitpack_getu(instruction, 3, 3),
                Bitpack_getu(instruction, 3, 0));
            return;
        case INACTIVATE:
            unmap_seg(Bitpack_getu(instruction, 3, 0));
            return;
        case OUT:
            output(Bitpack_getu(instruction, 3, 0));
            return;
        case IN:
            input(Bitpack_getu(instruction, 3, 0));
            return;
        case LOADP:
            *prog_counter = registers[Bitpack_getu(instruction, 3, 0)];
            loadprog(Bitpack_getu(instruction, 3, 3));

            if (*prog_counter >= seg_zero_length()) {
                exit(1);
            }
            return;
        case LV:
            loadval(Bitpack_getu(instruction, 3, 25),
                    Bitpack_getu(instruction, 25, 0));
            return;
    }
}

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
void cmov(Um_register a, Um_register b, Um_register c)
{
    if (registers[c] != 0) {
        registers[a] = registers[b];
    }
}

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
void seg_load(Um_register a, Um_register b, Um_register c)
{
    registers[a] = get_word(registers[b], registers[c]);
}

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
void seg_store(Um_register a, Um_register b, Um_register c)
{
    set_word(registers[a], registers[b], registers[c]);
}

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
void add(Um_register a, Um_register b, Um_register c)
{
    uint64_t mod_divisor = (uint64_t)1 << 32;
    registers[a] = (registers[b] + registers[c]) % mod_divisor;
}

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
void multiply(Um_register a, Um_register b, Um_register c)
{
    uint64_t mod_divisor = (uint64_t)1 << 32;
    registers[a] = (registers[b] * registers[c]) % mod_divisor;
}

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
void divide(Um_register a, Um_register b, Um_register c)
{
    if (registers[c] == 0) {
        exit(1);
    }

    registers[a] = (registers[b] / registers[c]);
}

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
void nand(Um_register a, Um_register b, Um_register c)
{
    registers[a] = ~(registers[b] & registers[c]);
}

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
void map_seg(Um_register b, Um_register c)
{
    registers[b] = new_segment(registers[c]);
}

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
void unmap_seg(Um_register c)
{
    if (registers[c] == 0) {
        exit(1);
    } else {
        free_segment(registers[c]);
    }
}

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
void output(Um_register c)
{
    char output_char = registers[c];

    assert(registers[c] < 256);

    putchar(output_char);
}

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
void input(Um_register c)
{
    int character = getchar();

    if (character == EOF) {
        registers[c] = ~0U;
    } else {
        registers[c] = character;
    }
}

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
void loadval(Um_register a, uint32_t value)
{
    registers[a] = value;
}

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
void loadprog(Um_register b)
{
    replace_segment_zero(registers[b]);
}