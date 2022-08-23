/**************************************************************
 *
 *                         um-main.c
 *
 *     Assignment: UM
 *     Authors:  Eli Intriligator (eintri01), Max Behrendt (mbehre01)
 *     Date:     Nov 23, 2021
 *
 *     Summary
 *     This file takes in a UM file from the command line and executes
 *     its instructions, calling functions from the instructions.h
 *     and segment.h modules where necessary.
 *     
 *     Note
 *     A UM file must be supplied.
 *     
 **************************************************************/
#include "bitpack.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "segment.h"
#include "instruction.h"

UArray_T read_words(FILE *fp, int num_words);
void print_words(UArray_T segment_zero);
void execute_program();

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Incorrect usage!\n");
        exit(EXIT_FAILURE);
    }

    struct stat buf;
    
    FILE *fp = fopen(argv[1], "r");
    assert(fp != NULL);

    stat(argv[1], &buf);
    int num_words = buf.st_size / 4;

    UArray_T segment_zero = read_words(fp, num_words);

    init_segment(segment_zero);

    execute_program(num_words);

    free_all_segments();

    fclose(fp);
    
    return 0;
}

/* read_words
 * Purpose: Reads the instructions from a file into what will become
            segment 0
 * Parameters: a file pointer and an integer
 * Returns: A UArray
 *
 * Expected input: A file pointer pointing to a file that is filled with
                    valid um instructions, and the number of uint32_t words
                    in that fiile
 * Success output: A UArray_T that contains all of the instructions in the
                    supplied file in the proper order
 * Failure output: none
 */
UArray_T read_words(FILE *fp, int num_words)
{
    UArray_T segment_zero = UArray_new(num_words, sizeof(uint32_t));

    for (int i = 0; i < num_words; i++) {
        uint32_t curr_word = 0;

        for (int j = 24; j >= 0; j -= 8) {
            uint32_t c = fgetc(fp);
            curr_word = Bitpack_newu(curr_word, 8, j, c);
        }

        *(uint32_t *)UArray_at(segment_zero, i) = curr_word;
    }

    return segment_zero;
}

/* execute_program
 * Purpose: loops through all of the words in m[0] and calls opcode_reader
            on them, updating the program pointer as needed
 * Parameters: none
 * Returns: Nothing
 *
 * Expected input: none
 * Success output: none
 * Failure output: none
 */
void execute_program()
{
    bool continue_execution = true;
    int prog_counter = 0;

    while (continue_execution == true) {
        Um_instruction word = get_word(0, prog_counter);
        prog_counter++;
        opcode_reader(word, &continue_execution, &prog_counter);
    }
}
