# COMP 40 hw06: Universal Virtual Machine

Eli Intriligator and Max Behrendt

Date: Nov 23 2021

TA help acknowledgements:
    Alex, Ann-Marie, Liana, Alan, Tina, Igor, Gustavo, Mary-Joy

In this assignment, we wrote the software implementation of a
virtual machine, complete with 14 machine instructions that
result in a Turing-complete system.

## Known problems/limitations

We believe we have implemented all features correctly.

## Significant departures from design

In our design, we planned to store each segment in a sequence. However,
we switched to using UArrays to store each segment since this approach
involves less pointer-following, making it faster and simpler to
program. In addition, using UArrays means that we did not have to
manage memory as much.

## Architecture

The program is composed of two modules and a file um-main.c, which
contains main. The first module, segment.h, implements the segmented
memory abstraction, allowing the user to map and unmap segments of
memory and receive 32-bit identifiers for the allocated segments. This
module knows the secrets of where memory is stored and how segment
identifiers are managed. To create the initial segment, segment.h
receives a UArray of the input instructions from main. The second
module, instructions.h, implements the UM's instruction set
abstraction. It keeps the registers, instruction codes, and information
about the bit representation of instructions hidden from the other
modules. This module depends on um-main.c to feed it instruction words,
and it depends on segment.h to handle segmented functions. Lastly,
um-main.c reads instructions from the .um file and contains the main
execution loop for UM programs. um-main.c keeps the file I/O hidden
from the other modules. It interacts with segment.h to initialize
the UM and to free all segments at the end of execution, and it
interacts with instruction.h to execute UM instructions.

**How long does it take our program to execute 50 million instructions?**
We know that midmark.um executes 85070522 instructions (we counted the
instructions and printed the result), and we also know that it took our
program 8.305 seconds to run midmark.um. That means, on average, our
program takes 9.76248859e-8 seconds to execute one instruction. Based
on this, it would take our program 4881.244 seconds to execute 50
million instructions.

## UM Tests:

`add`: Tests add by loading the ascii value B into one register and
the value 1 into another, then calling add on them to get an
expected output of 'C'.

`conditional move`: Tests first by putting the values 80 and 90 into
two registers and calling conditional move with the condition 0. We
then print the first register to see that it is still 80. The same then
occurs with the condition 1, in which case we print the first register
and see the value 80 has changed to 90.

`divide`: Loads the values 640 and 10 into two registers, then calls
the divide instruction with the first being divided by the second.
We use output to print the result, which is the 64th ascii char.

`halt`: Calls halt immediately; the program should stop.

`halt verbose`: Similarly to halt, the program should stop, but if it
does not the word 'bad' will be printed.

`load_prog_full`: This tests whether load program successfully switches
to a new segment. It checks this by first mapping a new segment that
will be switched to, then loading 80 into register 7 using
multiplication and addition to construct two new instructions – an
output instruction that prints the contents of register 7 and a halt
instruction. These two new instructions are loaded into the new segment
using segmented store, then the test calls load program and switches to
the new segment. The test will then print 80 (i.e. 'P') if load program
is implemented correctly.

`load_prog_pointer`: Loads the values A, B, C, and D into 4 registers,
prints the first 2, then calls load_program with m[0], such that
it just moves the program pointer to the 10th instruction, skipping
the print of C. Thus the correct output is ABD.

`map_segment`: Uses map segment to make a new segment of size 7, which
places the index of the new segment into a register. We then take that
register and add 80 to it, then output it. If correct, the test should
output the 81st ascii char.

`multiply`: Multiplys 8 by 8 and then prints the result. Should print the
64th ascii char.

`nand`: Calls nand on the values 5 and 10, then adds 80 to the result
so that we can output a printable ascii. Then does the same with the
values 5 and 5. The correct output is 'OJ'.

`output`: Loads and then outputs every ascii char.

`seg_store_load`: Tests both seg_store and seg_load by mapping a new
segment, using seg store to store the value 80 in that segment,
using seg load to load that value into a register, then using
output to print the value.

`unmap_segment`: Maps a segment of size 8, then immidiately unmaps it.
We then map a new segment, which should be at the same index as the
first segment, and place values in it. We then print those values
to make sure that the segment was properly mapped to the index
that the original was unmapped from.

---

Time spent analyzing problems posed in the assignment:
    5 hours

Time spent preparing our design:
    7 hours

Time spent solving the problems after our analysis:
    18 hours
