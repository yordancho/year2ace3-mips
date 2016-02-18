/***************************************************************************
 *
 * Synopsis:
 * ace3 <Initial_PC (Reg. No.)>
 *
 * Design Notes:
 * Since much of this sample solution is based on ace2sample.c the design is
 * relatively straightforward. Please refer back to the ace2samample design notes
 * for further explanation of that original code. The main differences in
 * this current version includes:
 * - the requirement to decode any of the (given subset of) MIPS instructions;
 * - a data-driven approach to the decoding of the instructions, in particular
 * - specifying the syntax, format and mnemonic of every instruction.
 * - printing out each stage of the MIPS pipeline architecture.
 * - pass in the correct values and control bits and moving them along to the next stage.
 * Given the 2nd point above, identification of a good reference source for the
 * MIPS instruction set needed to be found. There are literally hundreds of
 * possibilities thrown up by a judicious Google search but the one I've used
 * is the MIPS Encoding Reference found at
 *   http://www.student.cs.uwaterloo.ca/~isg/res/mips/opcodes
 * 
 * Bugs:
 * See Revision History ;)
 *
 * File: ace3.c
 *
 * Author:
 *      Yordan Y Stoykov, Reg no: 201334079
 *
 * Group:
 * Tuesday, 10-12
 *
 * Promise: I confirm that this crappy submission is all my own work.
 *
 * (Signed)__________________________________________
 *
 * Version: See VERSION below
 *
 **************************************************************************/

/********************* R E V I S I O N   H I S T O R Y ********************
 * 27,30/3,5,6,8
 *	v0.1  28/12/2014 Took a look at the ace3 assignment and opened up Duncan's
 *solution for ace2sample.c. Started trying to figure out what was required from me.
 *Started editing the file and removing the unnecessary parts. After that I made
 *sure I understood what each part of the code so far does.
 *
 *	v0.2  25/01/2015 Since I was not too sure how the MIPS pipeline architecture
 *worked, I started going through the lecture slides and reading from the book.
 *I added Duncan's definition of the structures for the pipeline registers. 
 *
 *	v0.3 27/01/2015 I thought I understood the pipeline architecture, but I had
 *no idea how to go about this assignment so I watched a 20 video play-list on
 *youtube which explained the whole MIPS pipeline architecture very well.	
 *I drew up a rough plan on paper about what I was going to do and started modifying 
 * my main method.
 *
 *	v0.4 30/01/2015 I started writing up print statements for each of the stages. They
 *were initially in main but then I decided to move them to separate void functions.
 *Also I added a cycle counter which was to keep track of the cycle that is being 
 *printed on the screen.
 *
 *	v0.5 03/02/2015 I started passing in values to those print functions. Firstly
 *just the reg number (aka the initial PC) and then also the cycle which increments
 *by one after each run of the while loop. Then I used Duncan's macros for extracting
 *each of the parts of the instruction into the different parts for OP,RT,RS,RD,FUNCT,
 *SHAMT and IMMEDIATE.
 *
 *	v0.6 05/02/2015 I was thinking about how to do the print functions for the other
 *stages of the pipeline, namely how to set the control bits. I thought I might find
 *the answer in the structures Duncan has supplied us with so I started reading online
 *tutorials on how to use them. The I edited the mips_core_t structure so that it 
 *included the control unit structures - ex_ctrl_t, mem_ctrl_t, wb_ctrl_t. After that
 *I began editing Duncan's arrays so as to set the control bits for each instruction
 *that we need to decode and later use. I also switched some of the around so that
 *they would better match the print statements and there would be no confusion. 
 *
 *	v0.7 08/02/2015 I continued working on the main method and made it so all stages 
 *are initialized to 0. So SLL $0, $0, 0. I also made sure the control bits properly
 *update in the print statements. The print statements take in a specific structure.
 *and then the separate print statements use specific parts of said structure for
 *example input.ex_ctrl.RegDst which gets the control bit for RegDst. Initially I had
 *quite a bit of errors but now I fixed them and only have some warning which have to
 *do with the fact that I haven't yet completed the other stages and have some unused
 *variable and etc. I tested the code and so far it correctly works for IF and the ID
 *stages by setting the bits and printing them. Now I just need to do the same for 
 *the other stages and make sure the ALU unit works correctly. I have a pretty good
 *idea of what I am going to do from here, it should be pretty straightforward. I just
 *need to add the final pieces to the puzzle and start putting it all together.
 *
 * 	v0.8 10/02/2015 I completed the print statements for the other stages as well.
 *Now I started brainstorming ideas on a way to get the instructions to travel 
 *through the pipeline. I decided to write up separate function that would move
 *the separate parts of the stages. Then I looked at Duncan's new sample output
 *and tried to figure out what the control bits for each of my instructions should be. 
 *After that I went to the array where they are stored and started setting them. I also
 *began working on the move functions which I also added to main. I made sure to work from 
 *back (first starting from the WB state) so as to not override anything not yet moved.
 *
 *	v0.9 13/02/2015 I continued working on the move function and got most of them pretty
 *much finished. There were still some gaps like the ALUResult for example which I later
 *decided to do by creating a switch to check what the instruction was. I wrote up
 *several statements for each of the instruction and started figuring out what result
 *I was meant to be getting and putting it into the ALUResult.
 *
 *	v0.9 15/02/2015 I was pretty close to done when I started working on my program today.
 *I finished up my move functions and also my print methods. I spend quite a lot of time
 *trying to figure out how to make sure the ALUResult was correct each time and also to make
 *sure that each register was moved to the correct part of the corresponding structure for it.
 *I was almost done I just need to get the BEQ instruction to work. I spend way too much time
 *trying to figure it out but to no avail. I decided to just finish up the rest of my program 
 *and make sure I am getting the correct outputs. I used www.diffchecker.com to compare my
 *output with Duncan's and see where I needed to change things. 
 *
 *	v1.0 15/02/2015 Finally got BEQ to work. Now my program prints all of the correct outputs even
 *though it's a bit forced and not very efficient.
 *
 *
 * 
 *
 *
 **************************************************************************/
#define VERSION "ace3 v1.0 by Yordan Stoykov. Last Update 15/02/2015\n"
#define USAGE "Usage: %s <Initial_PC (Reg. No.)>\n",argv[0]

#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* UNCHANGED indicates no change from the current value */
#define UNCHANGED 0
/* INERR is the U_S Input Error */
#define INERR -1

#if SHRT_MAX == 2147483647
typedef unsigned short int uint32_t;
#elif INT_MAX == 2147483647
typedef unsigned int uint32_t;
#elif LONG_MAX == 2147483647
typedef unsigned long uint32_t ;
#elif LLONG_MAX == 2147483647
typedef unsigned long long uint32_t;
#else
#error "Cannot find 32bit integer."
#endif

/* On some systems an UINT32_MAX is already defined, if not... */
#ifndef UINT32_MAX
#define UINT32_MAX ((uint32_t)-1)
#endif

/* Note on the following #define suffix nomenclature:
   _L = Least significant bit position of the field
   _M = Most significant bit position of the field
   _SZ = Size of the field
*/
/* Fields in the lower 16 bits of the MIPS instruction format: */ 
/* I-type field in lower 16 bits */
#define I_L 0
#define I_M 15
#define I_SZ ((I_M-I_L)+1)

/* R-type fields in lower 16 bits */
/* Function */
#define FN_L 0
#define FN_M 5
#define FN_SZ ((FN_M-FN_L)+1)
/* Shamt - shift amount */
#define SH_L 6
#define SH_M 10
#define SH_SZ ((SH_M-SH_L)+1)
/* Register RD */
#define RD_L 11
#define RD_M 15

/* Fields in the upper 16 bits of the MIPS instruction format: */ 
/* I-type and R-type share a commmon set of fields in the upper 16 bits */
/* Register RT */
#define RT_L 16
#define RT_M 20
/* Register RS */
#define RS_L 21
#define RS_M 25
/* Registers RD, RT and RS are all the same size */
#define R_SZ ((RD_M-RD_L)+1)

/* Opcode is in the top 6 bits for _all_ instructions */
#define OP_L 26
#define OP_M 31
#define OP_SZ ((OP_M-OP_L)+1)

/* The J(ump) instruction is a special case */
/* J-type uses all the bits other than the opcode field */
#define J_L 0
#define J_M 25
#define J_SZ ((J_M-J_L)+1)

/* There are many, many references inline for the MIPS instruction set.
   The one I found must succinct and useful was the MIPS Encoding Reference
   at http://www.student.cs.uwaterloo.ca/~isg/res/mips/opcodes
   Based on the approach taken in that reference, the following two enum
   definitions identify...
   ...the Format (_F):
      R_F: R(egister),
      I_F: I(mmediate),
      J_F: J(ump),
      F_F: F(loating point co-processor)
      U_F: U(defined)
   ...and the Syntax (_S):
      R_S: R(egister)
      AL_S: A(rithmetic) L(ogical)
      DM_S: D(ivide) M(ultiply)
      S_S: S(hift)
      SV_S: S(hift) V(ariable)
      J_S: J(ump)
      JR_S: J(ump) R(egister)
      JLR_S: J(ump) and L(ink) R(egister)
      MF_S: M(ove) F(rom)
      MT_S: M(ove) T(o)
      ALI_S: A(rithmetic) L(ogical) I(mmediate)
      LI_S: L(oad) I(mmediate)
      B_S: B(ranch)
      BZ_S: B(ranch) with Z(ero)
      LS_S: L(oad) S(tore)
      T_S: T(rap)
      U_S: U(ndefined)
*/     
typedef enum Format {
   R_F,I_F,J_F,F_F,U_F,MAX_F} Format_t;

typedef enum Syntax {
   R_S,
   AL_S,DM_S,S_S,SV_S,J_S,JR_S,JLR_S,MF_S,MT_S,
   ALI_S,LI_S,B_S,BZ_S,LS_S,
   T_S,
   U_S,
   MAX_S} Syntax_t;


/* The definition of the structures for the pipeline registers. */
typedef struct {
  uint32_t RegDst : 1;
  uint32_t ALUOp : 2;
  uint32_t ALUSrc : 1;
} ex_ctrl_t;

typedef struct {
  uint32_t Branch : 1;
  uint32_t MemWrite : 1;
  uint32_t MemRead : 1;
} mem_ctrl_t;

typedef struct {
  uint32_t RegWrite : 1;
  uint32_t MemtoReg : 1;
} wb_ctrl_t;

typedef struct {
  uint32_t bits;
  uint32_t pc;
} if_id_reg_t;

typedef struct {
  uint32_t bits;
  wb_ctrl_t wb_ctrl;
  mem_ctrl_t mem_ctrl;
  ex_ctrl_t ex_ctrl;
  uint32_t pc;
  uint32_t readData1;
  uint32_t readData2;
  uint32_t SEConstant;
  uint32_t rt : 5;
  uint32_t rd : 5;
} id_ex_reg_t;

typedef struct {
  uint32_t bits;
  uint32_t pc;
  wb_ctrl_t wb_ctrl;
  mem_ctrl_t mem_ctrl;
  uint32_t BTA;
  uint32_t ALUResult;
  uint32_t ALUZero : 1;
  uint32_t writeDataMem;
  uint32_t writeRegister : 5;
} ex_mem_reg_t;

typedef struct {
  uint32_t bits;
  uint32_t pc;
  wb_ctrl_t wb_ctrl;
  uint32_t ALUResult;
  uint32_t readDataMem;
  uint32_t writeRegister : 5;
} mem_wb_reg_t;

typedef struct {
  uint32_t bits;
  uint32_t pc;
  wb_ctrl_t wb_ctrl;
  uint32_t ALUResult;
  uint32_t readDataMem;
  uint32_t writeRegister : 5;
} wb_reg_t;


/* The definition of the structure for the instruction decode lookup table. */
typedef struct {
  Format_t 		format;
  Syntax_t 		syntax; 
  char *mnemonic;
  ex_ctrl_t 	execute; /* added the control signal structures in here*/
  mem_ctrl_t 	memory;
  wb_ctrl_t 	writeback;
} mips_core_t;

/* The following is a macro that 'extracts' from the bits passed in the 
   value of the field of size bit_sz that begins at bit position bit_l.
 */
#define IN(bits,bit_l,bit_sz) ((bits >> bit_l) & ((1 << bit_sz) - 1))

/* The following are macros that 'extracts' from the bits passed in the 
   value of the specific fields.
 */
#define OP(bits) IN(bits, OP_L, OP_SZ)
#define RS(bits) IN(bits, RS_L, R_SZ)
#define RT(bits) IN(bits, RT_L, R_SZ)
#define RD(bits) IN(bits, RD_L, R_SZ)
#define SHAMT(bits) IN(bits, SH_L, SH_SZ)
#define FUNCT(bits) IN(bits, FN_L, FN_SZ)
#define IMMEDIATE(bits) IN(bits, I_L, I_SZ)
#define ADDRESS(bits) IN(bits, J_L, J_SZ)

uint32_t programCounter;  /* Program Counter (PC) is now a global variable */
#define PC (programCounter) /* Shorthand version */

/* The following arrays specify the Format, Syntax and Mnemonic for each
   instruction indexed by opcode value. Since an opcode maps directly to
   its entry there is no need to 'search' for a match. Opcodes that aren't
   valid are marked as "Undefined". This approach makes the 'decoding' of
   instructions a trivial exercise as is adding any additional instructions
   that may be required.
 */

static mips_core_t opcode [] = { /* I added the control bits for each of the instructions as they would appear in the corresponding structure */
/* 0x0x */
	{R_F,R_S,"See funct field"},
	{U_F,U_S,"Undefined"},
	{J_F,J_S,"j"},
	{J_F,J_S,"jal"},
	{I_F,B_S,"beq", {0,1,0},{1,0,0},{0,0}},
	{I_F,B_S,"bne"},
	{I_F,BZ_S,"blez"},
	{I_F,BZ_S,"bgtz"},
	{I_F,ALI_S,"addi"},
	{I_F,ALI_S,"addiu",{0,0,1},{0,0,0},{1,0}},
	{I_F,ALI_S,"slti"},
	{I_F,ALI_S,"sltiu"},
	{I_F,ALI_S,"andi"},
	{I_F,ALI_S,"ori"},
	{I_F,ALI_S,"xori"},
	{I_F,LI_S,"lui"},
/* 0x1x */
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
/* 0x2x */
	{I_F,LS_S,"lb"},
	{I_F,LS_S,"lh"},
	{I_F,LS_S,"lwl"},
	{I_F,LS_S,"lw",{0,0,1},{0,0,1},{1,1}}, /*insert the proper control bits for lw*/
	{I_F,LS_S,"lbu"},
	{I_F,LS_S,"lhu"},
	{I_F,LS_S,"lwr"},
	{U_F,U_S,"Undefined"},
	{I_F,LS_S,"sb"},
	{I_F,LS_S,"sh"},
	{I_F,LS_S,"swl"},
	{I_F,LS_S,"sw",{0,0,1},{0,1,0},{0,0}},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{I_F,LS_S,"swr"},
	{U_F,U_S,"cache"},
/* 0x3x */
	{I_F,LS_S,"ll"},
	{I_F,LS_S,"lwc1"},
	{I_F,LS_S,"lwc2"},
	{I_F,LS_S,"pref"},
	{U_F,U_S,"Undefined"},
	{I_F,LS_S,"ldc1"},
	{I_F,LS_S,"ldc2"},
	{U_F,U_S,"Undefined"},
	{I_F,LS_S,"sc"},
	{I_F,LS_S,"swc1"},
	{I_F,LS_S,"swc2"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{I_F,LS_S,"sdc1"},
	{I_F,LS_S,"sdc2"},
	{U_F,U_S,"Undefined"}
};
/* The R-type instructions have an opcode of 0 and use the funct field to
   specify the actual instruction. The following array is in order of the
   funct value.
 */

static mips_core_t funct [] = {
/* 0x0x */
	{R_F,S_S,"sll",{1,2,0},{0,0},{1,0}},
	{U_F,U_S,"Undefined"},
	{R_F,S_S,"srl"},
	{R_F,S_S,"sra"},
	{R_F,SV_S,"sllv"},
	{U_F,U_S,"Undefined"},
	{R_F,SV_S,"srlv"},
	{R_F,SV_S,"srav"},
	{R_F,JR_S,"jr"},
	{R_F,JLR_S,"jalr"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
/* 0x1x */
	{R_F,MF_S,"mfhi"},
	{R_F,MT_S,"mthi"},
	{R_F,MF_S,"mflo"},
	{R_F,MT_S,"mtlo"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{R_F,DM_S,"mult"},
	{R_F,DM_S,"multu"},
	{R_F,DM_S,"div"},
	{R_F,DM_S,"divu"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
/* 0x2x */
	{R_F,AL_S,"add",{1,2,0},{0,0},{1,0}},
	{R_F,AL_S,"addu"},
	{R_F,AL_S,"sub",{1,2,0},{0,0},{1,0}},
	{R_F,AL_S,"subu"},
	{R_F,AL_S,"and",{1,2,0},{0,0},{1,0}},
	{R_F,AL_S,"or",{1,2,0},{0,0},{1,0}},
	{R_F,AL_S,"xor"},
	{R_F,AL_S,"nor"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{R_F,AL_S,"slt",{1,2,0},{0,0},{1,0}},
	{R_F,AL_S,"sltu"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
/* 0x3x */
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"},
	{U_F,U_S,"Undefined"}
};

/* Start of the actual code */

/* Synopsis:
 * #include <ctype.h>
 * int isnumeric(char *s)
 *
 * Description:
 * A convenience function similar to isdigit() except that it works for
 * strings rather than single characters.  
 *
 * Returns:
 * The same result as isdigit()
 */
int isnumeric(char *s) {
    int result;    /* Current isdigit() return value */
    do                                /* Check that each character of the...*/
        result = isdigit((int)*s++);  /* ...string is a digit and move on...*/
    while (result && *s) ;            /* ...until non-digit found or at EOS */
    return result;  /* Return result of last isdigit() call */
}

/* Synopsis:
 * uint32_t strtouint32(const char * restrict s,int base)
 *
 * Description:
 * Converts a string into an uint32_t (32 bit unsigned) integer value. 
 * A conversion function in the style of strtol(). The character string s is
 * interpreted as a numeral to the base specified by the second parameter base.
 * The string should only contain digits between 0 and 9 even though the
 * the base can be >10. This restriction is allowed since only 9 digit
 * student registration numbers are expected to be converted. If the input
 * string is not a valid numeral then the return value is UINT32_MAX.
 *
 * Returns:
 * The numeric value of the string or UINT32_MAX if string isn't numeric
 */
uint32_t strtouint32(char *s,int base) {
    uint32_t result = 0;
    if (!isnumeric(s))    /* Basic sanity check on input string */
      return(UINT32_MAX); /* UINT32_MAX uses as error indicator */
    while (*s)
        result = (result * base) + ((*s++) - '0'); /* Convert */
    return (result);
}

/* Synopsis:
 * #include <stdlib.h>
 * char* bitstostr (uint32_t bits, int size, int split)
 *
 * Description:
 * Converts the first parameter into a character string that contains the
 * bit pattern equivalent of the internal representation of the integer.
 * The second parameter is the number of bits to be inserted into the string.
 * The third parameter determines the number of bits to be grouped together.
 * A value of 0 for split means that no splitting is to take place. Non-zero
 * values for split cause a space to be inserted into the string after split
 * bits have been inserted.
 *
 * Returns:
 * The return value is a pointer to the (dynamically allocated) string of bits.
 * However, if the string cannot be allocated, it returns a pointer to the
 * "Calloc Failed" error string. This is rather a kludge since calloc is not
 * expected to fail and so the user may assume it succeeds and won't bother
 * checking the return result. Better error reporting/handling is left as an
 * exercise to the reader!
 */
char* bitstostr (uint32_t bits, int size, int split) {
    char *bitsPtr;  /* Pointer to the bit string being built */
    int stringsize, /* (Maximum) Size of the string that's required */
        splitter;   /* Countdown count until a splitting space insertion */ 
    stringsize = (split)?size+size/split:size; /* Calculate size with splits */
    splitter = split; /* Initialise countdown to a splitting space insertion */
    /* Now we know the maximum number of characters needed calloc() them. */  
    if (NULL == (bitsPtr = calloc(stringsize+1,sizeof(char))))
      return("Calloc Failed"); /* Left as an exercise for future improvement */
    /* We now have our array initialised to '\0's so no need to plant an EOS */ 
    bitsPtr += stringsize; /* String is built in reverse so start at the end */
    /* Now perform the conversion. The (bits&1) mask is used to pick off the
     * lowest bit of the number.
     */
    for (  ; size-- ; bits >>= 1) { /* Keep shifting bits down until all done */
        *--bitsPtr = (bits&1)?'1':'0';    /* (Back)Fill string with 1s and 0s */
        if (splitter > 0)          /* Do the bits get split up into groups?   */
          if (0 == (--splitter)) { /* if so do we split them this time round? */ 
            *--bitsPtr = ' ';      /* Yes, so insert a space into the string  */
            splitter = split;      /* ...and reset the split countdown count. */
          }
    }
    if (' ' == *bitsPtr) ++bitsPtr; /* Skip any leading space */
    return(bitsPtr);
}

/* The following print_? functions are really just wrappers round the
   printf()s. This is intended to be a help when (in no particular order):
   - reading the code
   - understanding the code
   - maintaining the code
   - adapting the code
*/

void print_rtype (char *mnemonic, uint32_t bits) {
  puts("");
  printf("R-Type fields:\t+ %6s + %5s + %5s + %5s + %5s + %6s +\n",
          mnemonic,"RS","RT","RD","Shamt","Funct");
  printf("\tBits:\t| %s | %s | %s | %s | %s | %s |\n",
             bitstostr(OP(bits),OP_SZ,0),
                  bitstostr(RS(bits),R_SZ,0),
                       bitstostr(RT(bits),R_SZ,0),
                            bitstostr(RD(bits),R_SZ,0),
                                 bitstostr(SHAMT(bits),SH_SZ,0),
                                      bitstostr(FUNCT(bits),FN_SZ,0));
  printf("\tValues:\t+ %6d + %5d + %5d + %5d + %5d + %6d +\n",
          OP(bits),RS(bits),RT(bits),RD(bits),SHAMT(bits),FUNCT(bits));
}

void print_itype (char *mnemonic, uint32_t bits) {
  puts("");
  printf("I_Type fields:\t+ %6s + %5s + %5s + %16s +\n",
         mnemonic,"RS","RT","Immediate");
  printf("\tBits:\t| %s | %s | %s | %s |\n",
         bitstostr(OP(bits),OP_SZ,0),
              bitstostr(RS(bits),R_SZ,0),
                   bitstostr(RT(bits),R_SZ,0),
                        bitstostr(IMMEDIATE(bits),I_SZ,0));
  printf("\tValues:\t+ %6d + %5d + %5d + %11hd(dec) +\n",
          OP(bits),RS(bits),RT(bits),(short)IMMEDIATE(bits));
}

void print_jtype (char *mnemonic, uint32_t bits) {
  puts("");
  printf("J-Type fields:\t+ %6s + %26s +\n",
          mnemonic,"Address");
  printf("\tBits:\t| %s | %s |\n",
          bitstostr(OP(bits),OP_SZ,0),bitstostr(ADDRESS(bits),J_SZ,0));
  printf("\tValues:\t+ %6d + %21X(hex) +\n",
          OP(bits),ADDRESS(bits));
                 
}

/* The following print_? functions just format the instructions as they
   would appear in MIPS assembly language.
*/
void print_i_type_mem(char *mnemonic, uint32_t bits) { /* Load/Store */
  printf("%-5s $%d,%hd($%d)\n",
          mnemonic,RT(bits),(short)IMMEDIATE(bits),RS(bits));
  return;
}

void print_i_type_alu(char *mnemonic, uint32_t bits) { /* I-type ALU */
  printf("%-5s $%d,$%d,%hd\n",
         mnemonic,RT(bits),RS(bits),(short)IMMEDIATE(bits));
  return;
}

void print_i_type_li(char *mnemonic, uint32_t bits) { /* I-type loads */
  printf("%-5s $%d,%hd\n",
         mnemonic,RT(bits),(short)IMMEDIATE(bits));
  return;
}

void print_r_type_shift(char *mnemonic, uint32_t bits) { /* R-type shifts */
  printf("%-5s $%d,$%d,%d\n",
         mnemonic,RD(bits),RT(bits),SHAMT(bits));
  return;
}

void print_r_type_shift_v(char *mnemonic, uint32_t bits) { /* R-type v shifts */
  printf("%-5s $%d,$%d,$%d\n",
         mnemonic,RD(bits),RT(bits),RS(bits));
  return;
}


void print_r_type(char *mnemonic, uint32_t bits) { /* R-type ALU */
  printf("%-5s $%d,$%d,$%d\n",
         mnemonic,RD(bits),RS(bits),RT(bits));
  return;
}

void print_r_type_dm(char *mnemonic, uint32_t bits) { /* R-type ALU  */
  printf("%-5s $%d,$%d\n",
         mnemonic,RS(bits),RT(bits));
  return;
}

void print_r_type_mf(char *mnemonic, uint32_t bits) { /* R-type move from */
  printf("%-5s $%d\n",
         mnemonic,RD(bits));
  return;
}

void print_r_type_mt(char *mnemonic, uint32_t bits) { /* R-type move to */
  printf("%-5s $%d\n",
         mnemonic,RS(bits));
  return;
}

void print_r_type_jr(char *mnemonic, uint32_t bits) { /*  */
  printf("%-5s $%d\n",
         mnemonic,RS(bits));
  return;
}

void print_r_type_jalr(char *mnemonic, uint32_t bits) { /*  */
  printf("%-5s $%d,$%d\n",
         mnemonic,RS(bits),RD(bits));
  return;
}

void print_j_type(char *mnemonic, uint32_t bits) { /* J-type */
  printf("%-5s %08X\n",
         mnemonic,((ADDRESS(bits) << 2) | (PC & 0XF0000000)));
  return;
}

void print_i_type_b(char *mnemonic, uint32_t bits, uint32_t pc) { /* I-type branch */
  printf("%-5s $%d,$%d,%08X\n",
         mnemonic,RT(bits),RS(bits),((short)IMMEDIATE(bits) << 2) + pc);
  return;
}

void print_i_type_bz(char *mnemonic, uint32_t bits) { /* I-type branch */
  printf("%-5s $%d,%08X\n",
         mnemonic,RS(bits),((short)IMMEDIATE(bits) << 2) + PC);
  return;
}


void Decode(uint32_t bits, bool verbose, uint32_t pipelinepc) { /* Edited the decode function to remove unnecessary parts */
  Format_t 		format;
  Syntax_t 		syntax;
  
  uint32_t		pc;
  
  char *mnemonic;
  int op;
  mips_core_t *map;

  
  pc = pipelinepc;
  
  op = OP(bits);
  if (0 == op)  /* An opcode of 0 is an R-type instruction and so use the... */
    map=&funct[FUNCT(bits)];  /* ...funct field to identify the instruction. */
  else
    map=&opcode[op]; /* A non-0 opcode specifies an instruction directly. */

  format = map->format;  /* Get the instruction format from the lookup table */
  syntax = map->syntax;  /* Ditto for the instruction syntax and... */
  mnemonic = map->mnemonic;  /* ...the mnemonic of the instruction. */
  

  if (verbose) {
      switch(format) {
        case R_F: print_rtype(mnemonic,bits); break;
        case I_F: print_itype(mnemonic,bits); break;
        case J_F: print_jtype(mnemonic,bits); break;
        case U_F: break;
        default:  break;
      }
  }
  
  switch(syntax) {
    case R_S:
    case AL_S:	print_r_type(mnemonic,bits); break;
    case S_S:	print_r_type_shift(mnemonic,bits); break;
    case ALI_S:	print_i_type_alu(mnemonic,bits); break;
    case LS_S:	print_i_type_mem(mnemonic,bits); break;
    case J_S:	print_j_type(mnemonic,bits); break;
    case JR_S:	print_r_type_jr(mnemonic,bits); break;
    case JLR_S:	print_r_type_jalr(mnemonic,bits); break;
    case SV_S:	print_r_type_shift_v(mnemonic,bits); break;
    case DM_S:	print_r_type_dm(mnemonic,bits); break;
    case MF_S:	print_r_type_mf(mnemonic,bits); break;
    case MT_S:	print_r_type_mt(mnemonic,bits); break;
    case LI_S:	print_i_type_li(mnemonic,bits); break;
    case B_S:	print_i_type_b(mnemonic,bits,pc); break;
    case BZ_S:	print_i_type_bz(mnemonic,bits); break;
    case T_S:	puts("TBD: Trap syntax"); break;
    case U_S:	puts("Undefined"); break;
    default: puts("Undefined");
  }
} 


/*Below are my print function which take in a structure and a cycle as an input and extract all of the necessary bits from it*/

void print_if_id_state(if_id_reg_t input, int cycle){
	uint32_t instruction;
	uint32_t pc;
	
	instruction = input.bits;
	pc = input.pc;	
	
	char *akars; /* Switches to find out what register we are using */
	switch(RS(instruction)){
		case 0: akars = "zero"; break;
		case 1: akars = "at"; break;
		case 2: akars = "v0"; break;
		case 3: akars = "v1"; break;
		case 4: akars = "a0"; break;
		case 5: akars = "a1"; break;
		case 6: akars = "a2"; break;
		case 11: akars = "t3"; break;
		case 17: akars = "s1"; break;
		case 31: akars = "ra"; break;
	default: akars = "unknown";
	}

	char *akart;
	switch(RT(instruction)){
		case 0: akart = "zero"; break;
		case 1: akart = "at"; break;
		case 2: akart = "v0"; break;
		case 3: akart = "v1"; break;
		case 4: akart = "a0"; break;
		case 5: akart = "a1"; break;
		case 6: akart = "a2"; break;
		case 11: akart = "t3"; break;
		case 17: akart = "s1"; break;
		case 31: akart = "ra"; break;
	default: akart = "unknown";
	}
	
	char *akard;
	switch(RD(instruction)){
		case 0: akard = "zero"; break;
		case 1: akard = "at"; break;
		case 2: akard = "v0"; break;
		case 3: akard = "v1"; break;
		case 4: akard = "a0"; break;
		case 5: akard = "a1"; break;
		case 6: akard = "a2"; break;
		case 11: akard = "t3"; break;
		case 17: akard = "s1"; break;
		case 31: akard = "ra"; break;
	default: akard = "unknown";
	}
		
	/*Print stage IF/ID*/		
	printf("IF/ID state at end of cycle %d:\n",
         cycle);		 
	printf("Instruction (in assembly language) is : ");	
	Decode(instruction, false, pc);	
		 
	printf("Instruction (in hexadecimal) is : 0x%08X\n",
         instruction);		 
	printf("IF/ID.PC:32 = %d\n",
		pc);
	printf("IF/ID.I:32 = %08X\n",
		instruction);
		
	printf("Sub-fields of IF/ID.I are:\n");
	printf("IF/ID.I:[31-26] (opcode) = %s [%d(ten)]\n",
	bitstostr(OP(instruction),OP_SZ,0), OP(instruction));
	printf("IF/ID.I:[25-21] (rs) = %s [$%d aka $%s]\n",
	bitstostr(RS(instruction),R_SZ,0), RS(instruction), akars);
	printf("IF/ID.I:[20-16] (rt) = %s [$%d aka $%s]\n",
	bitstostr(RT(instruction),R_SZ,0), RT(instruction), akart);
	printf("IF/ID.I:[15-11] (rd) = %s [$%d aka $%s]\n",
	bitstostr(RD(instruction),R_SZ,0), RD(instruction), akard);
	printf("IF/ID.I:[10-6] (shamt) = %s [%d(ten)]\n",
	bitstostr(SHAMT(instruction),SH_SZ,0), SHAMT(instruction));
	printf("IF/ID.I:[5-0] (funct) = %s [%d(ten)]\n",
	bitstostr(FUNCT(instruction),FN_SZ,0), FUNCT(instruction));
	printf("IF/ID.I:[15-0] (const) = %s [%d(ten)]\n",
	bitstostr(IMMEDIATE(instruction),I_SZ,0), (short)IMMEDIATE(instruction));
	
	printf("\n");
	return;
}

void print_id_ex_state(id_ex_reg_t input, int cycle){
	uint32_t instruction;
	uint32_t pc;
	uint32_t readData1;
	uint32_t readData2;
	uint32_t SEConstant;
	uint32_t rt;
	uint32_t rd;
	uint32_t readData1value;
	uint32_t readData2value;
	
	/* Below I initialise each variable and make sure it has the correct values */
	instruction = input.bits;
	pc = input.pc;
	readData1 = input.readData1;
	readData1value = (input.readData1 - 10);
	if(readData1 == 10){
		readData1 -= 10;
	}
	
	readData2 = input.readData2;
	readData2value = (input.readData2 - 10);
	if(readData2 == 10){
		readData2 -= 10;
	}
	
	SEConstant = input.SEConstant;
	rt = input.rt;
	rd = input.rd;
	

	/*Print stage ID/EX*/
	printf("ID/EX state at end of cycle %d:\n",
         cycle);		 
	printf("Instruction (in assembly language) is : ");
	Decode(instruction, false, pc);	
		 
	printf("Instruction (in hexadecimal) is : 0x%08X\n",
        instruction);		 
	printf("ID/EX.PC:32 = %d\n",
		pc);	
	printf("ID/EX.readData1:32 = %d ($%d contains %d)\n",
	readData1, readData1value, readData1);
	printf("ID/EX.readData2:32 = %d ($%d contains %d)\n",
	readData2, readData2value, readData2);
	printf("ID/EX.SEConstant:32 = %d (0x%08X) (sign extended IF/ID.I:[15-0])\n",
	(short)SEConstant, (short)SEConstant);
	printf("Note: SEConstant:[15-11] = %d [rd]\n",
	RD(instruction));
	printf("Note: SEConstant:[10-6] = %d [shamt]\n",
	SHAMT(instruction));
	printf("Note: SEConstant:[5-0] = %d [funct]\n",
	FUNCT(instruction));
	printf("ID/EX.rdIndex:5 = %d (from IF/ID.I:[15-11])\n",
	rd);
	printf("ID/EX.rtIndex:5 = %d (from IF/ID.I:[20-16])\n",
	rt);
	printf("ID/EX.RegDst:1 = %d\n",
	input.ex_ctrl.RegDst);
	printf("ID/EX.ALUOp:2 = %d\n",
	input.ex_ctrl.ALUOp);
	printf("ID/EX.ALUSrc:1 = %d\n",
	input.ex_ctrl.ALUSrc);
	printf("ID/EX.Branch:1 = %d\n",
	input.mem_ctrl.Branch);
	printf("ID/EX.MemWrite:1 = %d\n",
	input.mem_ctrl.MemWrite);
	printf("ID/EX.MemRead:1 = %d\n",
	input.mem_ctrl.MemRead);
	printf("ID/EX.RegWrite:1 = %d\n",
	input.wb_ctrl.RegWrite);
	printf("ID/EX.MemtoReg:1 = %d\n",
	input.wb_ctrl.MemtoReg);
	
	printf("\n");
	
	return;
}

void print_ex_mem_state(ex_mem_reg_t input, int cycle){
	uint32_t instruction;
	uint32_t pc;
	uint32_t BTA;
	uint32_t ALUResult;
	uint32_t ALUZero;
	uint32_t writeDataMem;
	uint32_t writeRegister;
	uint32_t writeDataMemvalue;

	instruction = input.bits;
	pc = input.pc;
	BTA = input.BTA;
	ALUResult = input.ALUResult;
	ALUZero = input.ALUZero;
	writeDataMem = input.writeDataMem;
	writeDataMemvalue = (input.writeDataMem  - 10);
	if(writeDataMem == 10){
		writeDataMem -= 10;
		writeDataMemvalue = writeDataMem;
	}
	writeRegister = input.writeRegister;
	
	

	/*Print stage EX/MEM */	
	printf("EX/MEM state at end of cycle %d:\n",
         cycle);		 
	printf("Instruction (in assembly language) is : ");
	Decode(instruction, false, pc);	
		 
	printf("Instruction (in hexadecimal) is : 0x%08X\n",
         instruction);		 
	printf("EX/MEM.BTA:32 = %d ((%d << 2) + %d) [%08X]\n",
	(((short)BTA << 2)  + pc), (short)BTA, pc, (((short)BTA << 2)  + pc));
	printf("EX/MEM.ALUResult:32 = %d\n",
	ALUResult);
	printf("EX/MEM.ALUZero:1 = %d\n",
	ALUZero);
	printf("EX/MEM.writeDataMem:32 = %d ($%d contains %d)\n",
	writeDataMem, writeDataMemvalue, writeDataMem);
	printf("EX/MEM.writeRegister:5 = %d\n",
	writeRegister);
	printf("EX/MEM.Branch:1 = %d\n",
	input.mem_ctrl.Branch);
	printf("EX/MEM.MemWrite:1 = %d\n",
	input.mem_ctrl.MemWrite);
	printf("EX/MEM.MemRead:1 = %d\n",
	input.mem_ctrl.MemRead);	
	printf("EX/MEM.RegWrite:1 = %d\n",
	input.wb_ctrl.RegWrite);
	printf("EX/MEM.MemtoReg:1 = %d\n",
	input.wb_ctrl.MemtoReg);
	printf("\n");

	return;
}

void print_mem_wb_state(mem_wb_reg_t input, int cycle){
	uint32_t instruction;
	uint32_t ALUResult;
    uint32_t readDataMem;
    uint32_t writeRegister;
	uint32_t pc;
		
	instruction = input.bits;
	ALUResult = input.ALUResult;
	readDataMem = input.readDataMem;
	writeRegister = input.writeRegister;
	pc = input.pc;
	
	/*Print stage MEM/WB */	
	printf("MEM/WB state at end of cycle %d:\n",
         cycle);		 
	printf("Instruction (in assembly language) is : ");
	Decode(instruction, false, pc);	
		 
	printf("Instruction (in hexadecimal) is : 0x%08X\n",
         instruction);		 
	printf("MEM/WB.ALUResult:32 = %d\n",
	ALUResult);
	printf("MEM/WB.readDataMem:32 = %d\n",
	readDataMem);
	printf("MEM/WB.writeRegister:5 = %d\n",
	writeRegister);
	printf("MEM/WB.RegWrite:1 = %d\n",
	input.wb_ctrl.RegWrite);
	printf("MEM/WB.MemtoReg:1 = %d\n",
	input.wb_ctrl.MemtoReg);
	printf("\n");
	
	return;
}	

void print_wb_state(wb_reg_t input, int cycle){
	uint32_t instruction;
	uint32_t ALUResult;
    uint32_t readDataMem;
    uint32_t writeRegister;
	uint32_t pc;
		
	instruction = input.bits;
	ALUResult = input.ALUResult;
	readDataMem = input.readDataMem;
	writeRegister = input.writeRegister;
	pc = input.pc;
	/*Print stage WB */	
	printf("WB state at end of cycle %d:\n",
         cycle);		 
	printf("Instruction (in assembly language) is : ");
	Decode(instruction, false, pc);	
		 
	printf("Instruction (in hexadecimal) is : 0x%08X\n",
         instruction);		 
	printf("WB.ALUResult:32 = %d\n",
	ALUResult);
	printf("WB.readDataMem:32 = %d\n",
	readDataMem);
	printf("WB.writeRegister:5 = %d\n",
	writeRegister);
	printf("WB.RegWrite:1 = %d\n",
	input.wb_ctrl.RegWrite);
	printf("WB.MemtoReg:1 = %d\n",
	input.wb_ctrl.MemtoReg);
	printf("\n");
	
	return;
}


/* Setter functions to move instruction along*/ 
/* They just copy the old bits in to the next correct place in the next structure */

wb_reg_t move_wb_state(wb_reg_t new, mem_wb_reg_t old){
	new.bits = old.bits;
	new.pc = old.pc;
	new.wb_ctrl = old.wb_ctrl;
	new.ALUResult = old.ALUResult;
	new.readDataMem = old.readDataMem;
	new.writeRegister = old.writeRegister;	

	if(((OP(new.bits) == 4)) && (RS(new.bits) == RT(new.bits))){
		PC = old.pc - 4;
	}
	return new;
}

mem_wb_reg_t move_mem_wb_state(mem_wb_reg_t new, ex_mem_reg_t old){
	new.bits = old.bits;
	new.pc = old.pc;
	new.wb_ctrl = old.wb_ctrl;
	new.ALUResult = old.ALUResult;
	new.readDataMem = 0;
	if(old.mem_ctrl.MemRead == 1){
			new.readDataMem = (old.BTA << 2); /*when memread = 1*/
	}
	new.writeRegister = old.writeRegister;
	return new;
}	

ex_mem_reg_t move_ex_mem_state(ex_mem_reg_t new, id_ex_reg_t old){
	new.bits = old.bits;
	new.pc = old.pc;
	new.wb_ctrl = old.wb_ctrl;
	new.mem_ctrl = old.mem_ctrl;
	new.BTA = old.SEConstant;
	new.ALUResult = 0;
	if((OP(new.bits) == 0)) /* when it's and R type instruction, try and find out what type it is and calculate the ALUResult accordingly */
	{
		switch (FUNCT(new.bits)){
			case 0: if(old.readData2 != 10){ new.ALUResult = old.readData2 << SHAMT(old.bits);
			} break; /*sll*/
			case 32: new.ALUResult = old.readData1 + old.readData2; break; /*add*/
			case 34: new.ALUResult = old.readData1 - old.readData2; break; /*sub*/
			case 36: new.ALUResult = old.readData1 & old.readData2; break; /*and*/
			case 37: new.ALUResult = old.readData1 | old.readData2; break; /*or*/
			case 42: if(old.readData1 < old.readData2){
			new.ALUResult = 1; 
			} break; /*slt*/
			default: break;
		}
	}

		switch (OP(new.bits)){
			case 4: new.mem_ctrl.Branch = 1;
			/*if(old.readData1 == old.readData2){
			PC += (((short)IMMEDIATE(new.bits) << 2) + old.pc);
			new.pc = PC;
			}*/ break;
			case 9: new.ALUResult = old.readData1 + old.SEConstant; break; /*addiu*/
			case 35: new.ALUResult = old.readData1 + old.SEConstant; break; /*lw*/
			case 43: new.ALUResult = old.readData1 + old.SEConstant; break; /*sw*/
			default: break;	
		}
		
	new.ALUZero = 0;
	if(new.ALUResult == 0){
		new.ALUZero = 1;
	}
	
	new.writeDataMem = (old.rt + 10);
	new.writeRegister = old.rt;
	if(OP(new.bits) == 0 ){
		new.writeRegister = old.rd;
	}
	return new;
}

id_ex_reg_t move_id_ex_state(id_ex_reg_t new, if_id_reg_t old){
	int op;
	mips_core_t *map;

	op = OP(old.bits);
	if (0 == op)  /* An opcode of 0 is an R-type instruction and so use the... */
		map=&funct[FUNCT(old.bits)];  /* ...funct field to identify the instruction. */
	else
		map=&opcode[op]; /* A non-0 opcode specifies an instruction directly. */
	
	new.wb_ctrl = map-> writeback;
	new.mem_ctrl = map-> memory;
	new.ex_ctrl = map-> execute;
	
	
	new.bits = old.bits;
	new.pc = old.pc;


	new.readData1 = (RS(old.bits) + 10);
	new.readData2 = (RT(old.bits) + 10);
	new.SEConstant = IMMEDIATE(old.bits);
	new.rt = RT(old.bits);
	new.rd = RD(old.bits);
	
	return new;
}
	
	
/* GetHex() is based on GetDouble() from p2sample.c */

uint32_t GetHex(void) {
#define MAXIN 4096 /* MAXIN is the maximum number of input characters */
  static char input[MAXIN]; /* declared as static so it's not on the stack */
  char last;
  uint32_t result;
  fgets(input,MAXIN,stdin); /* fgets as scanf() can't handle blank lines */
  /* First of all check if it was a blank line, i.e. just a '\n' input...*/
  if ('\n' == input[0]) return(UNCHANGED);  /* ...so go-with-last-value. */
  /* Now sscanf() can be used to parse a hex # but we still need to detect
     extraneous input _after_ a valid hex #. has been parsed, hence the %c
     that should have '\n' assigned to it. So, if a valid hex # is parsed
     _and_ there was at least one more character - which there should be -
     then sscanf() will return 2 - the number of successfully parsed items */
  if ((sscanf(input,"%8x%c",&result,&last) < 2)) return(INERR);
  /* Now check to make sure the last character input was a '\n', if not... */ 
  if ('\n' != last) return(INERR);	/* ...then return an input error value */
  /* If we get to this point it has to be a valid hex # so just return it. */ 
  return(result);
}

uint32_t Fetch(uint32_t pc) {
  printf("%08X: ",pc);
  return (GetHex());
}



int main(int argc, char *argv[]) { 
  uint32_t instruction, regasdectouint32;
 /* bool verbose;*/
  int cycle;
  int i;
  i = 0; /* Counter for later use */
  
  
  /*Fill all stages with sll*/
  ex_ctrl_t 	execute = {1,2,0};
  mem_ctrl_t	memory = {0,0,0};
  wb_ctrl_t 	writeback = {1,0};
  id_ex_reg_t 	idex = {0,writeback, memory, execute, 0,0,0,0,0};
  ex_mem_reg_t 	exmem = {0,0, writeback, memory, 0,0,0,0,0};
  mem_wb_reg_t 	memwb = {0,0, writeback, 0,0,0};
  wb_reg_t		wb = {0,0, writeback, 0,0,0};
  
  if_id_reg_t 	ifid ={0,0};
  
  while (i < 5){/*Add 5 null-ops before execution starts so all stages are full */
	wb = move_wb_state(wb, memwb);
	memwb = move_mem_wb_state(memwb, exmem);		
	exmem = move_ex_mem_state(exmem, idex);
	idex = move_id_ex_state(idex, ifid);
	i++;
  }
  
  printf(VERSION); 
  printf("\n");
  
  /* Now perform some basic sanity checks on the command line input string... */ 
  if ( argc != 2 || /* if there's an input string carry on & check its length */
       strlen(argv[1]) != 9 || /* if OK then convert it and check the result: */
       (UINT32_MAX==(regasdectouint32=strtouint32(&argv[1][0],10))) ){ /* OK? */
    printf(USAGE);  /* Failed at least one of the above checks so alert user  */
    exit(1);        /* ...and exit. */
  }
  /* All of the above tests passed so continue on with the rest of the code. */
  
  cycle = 1;
  PC = regasdectouint32 << 2; /* and use as an initial Program Counter value */
  /*verbose = false;*/
   
  while ( (instruction = Fetch(PC)) != INERR ) {
    PC += 4;	

	/*if_id_reg_t initial = {instruction, PC};*/
	printf("\n");
	if_id_reg_t ifid = {instruction, PC};
	
	/*print statemetns for all the stages*/
	print_if_id_state(ifid, cycle);
	print_id_ex_state(idex, cycle);
	print_ex_mem_state(exmem, cycle);
	print_mem_wb_state(memwb, cycle);
	print_wb_state(wb, cycle);
	
	/*move everything along*/
	wb = move_wb_state(wb, memwb);
	memwb = move_mem_wb_state(memwb, exmem);		
	exmem = move_ex_mem_state(exmem, idex);
	idex = move_id_ex_state(idex, ifid);
	
	cycle++;
  }
  
  puts("Quitting");
  return(0);
}

/*Input:
8CD159C0
00D15920
24D159C0
001159C0
ACD159C0
0043082A
00430822
1000FFFF
00A62025
00851824
00011000
00021840
00032080
000428c0
00053100
Q
*/

/* Program compilation and execution output:
ace3 v1.0 by Yordan Stoykov. Last Update 15/02/2015

00000040: 
IF/ID state at end of cycle 1:
Instruction (in assembly language) is : lw    $17,22976($6)
Instruction (in hexadecimal) is : 0x8CD159C0
IF/ID.PC:32 = 68
IF/ID.I:32 = 8CD159C0
Sub-fields of IF/ID.I are:
IF/ID.I:[31-26] (opcode) = 100011 [35(ten)]
IF/ID.I:[25-21] (rs) = 00110 [$6 aka $a2]
IF/ID.I:[20-16] (rt) = 10001 [$17 aka $s1]
IF/ID.I:[15-11] (rd) = 01011 [$11 aka $t3]
IF/ID.I:[10-6] (shamt) = 00111 [7(ten)]
IF/ID.I:[5-0] (funct) = 000000 [0(ten)]
IF/ID.I:[15-0] (const) = 0101100111000000 [22976(ten)]

ID/EX state at end of cycle 1:
Instruction (in assembly language) is : sll   $0,$0,0
Instruction (in hexadecimal) is : 0x00000000
ID/EX.PC:32 = 0
ID/EX.readData1:32 = 0 ($0 contains 0)
ID/EX.readData2:32 = 0 ($0 contains 0)
ID/EX.SEConstant:32 = 0 (0x00000000) (sign extended IF/ID.I:[15-0])
Note: SEConstant:[15-11] = 0 [rd]
Note: SEConstant:[10-6] = 0 [shamt]
Note: SEConstant:[5-0] = 0 [funct]
ID/EX.rdIndex:5 = 0 (from IF/ID.I:[15-11])
ID/EX.rtIndex:5 = 0 (from IF/ID.I:[20-16])
ID/EX.RegDst:1 = 1
ID/EX.ALUOp:2 = 2
ID/EX.ALUSrc:1 = 0
ID/EX.Branch:1 = 0
ID/EX.MemWrite:1 = 0
ID/EX.MemRead:1 = 0
ID/EX.RegWrite:1 = 1
ID/EX.MemtoReg:1 = 0

EX/MEM state at end of cycle 1:
Instruction (in assembly language) is : sll   $0,$0,0
Instruction (in hexadecimal) is : 0x00000000
EX/MEM.BTA:32 = 0 ((0 << 2) + 0) [00000000]
EX/MEM.ALUResult:32 = 0
EX/MEM.ALUZero:1 = 1
EX/MEM.writeDataMem:32 = 0 ($0 contains 0)
EX/MEM.writeRegister:5 = 0
EX/MEM.Branch:1 = 0
EX/MEM.MemWrite:1 = 0
EX/MEM.MemRead:1 = 0
EX/MEM.RegWrite:1 = 1
EX/MEM.MemtoReg:1 = 0

MEM/WB state at end of cycle 1:
Instruction (in assembly language) is : sll   $0,$0,0
Instruction (in hexadecimal) is : 0x00000000
MEM/WB.ALUResult:32 = 0
MEM/WB.readDataMem:32 = 0
MEM/WB.writeRegister:5 = 0
MEM/WB.RegWrite:1 = 1
MEM/WB.MemtoReg:1 = 0

WB state at end of cycle 1:
Instruction (in assembly language) is : sll   $0,$0,0
Instruction (in hexadecimal) is : 0x00000000
WB.ALUResult:32 = 0
WB.readDataMem:32 = 0
WB.writeRegister:5 = 0
WB.RegWrite:1 = 1
WB.MemtoReg:1 = 0

00000044: 
IF/ID state at end of cycle 2:
Instruction (in assembly language) is : add   $11,$6,$17
Instruction (in hexadecimal) is : 0x00D15920
IF/ID.PC:32 = 72
IF/ID.I:32 = 00D15920
Sub-fields of IF/ID.I are:
IF/ID.I:[31-26] (opcode) = 000000 [0(ten)]
IF/ID.I:[25-21] (rs) = 00110 [$6 aka $a2]
IF/ID.I:[20-16] (rt) = 10001 [$17 aka $s1]
IF/ID.I:[15-11] (rd) = 01011 [$11 aka $t3]
IF/ID.I:[10-6] (shamt) = 00100 [4(ten)]
IF/ID.I:[5-0] (funct) = 100000 [32(ten)]
IF/ID.I:[15-0] (const) = 0101100100100000 [22816(ten)]

ID/EX state at end of cycle 2:
Instruction (in assembly language) is : lw    $17,22976($6)
Instruction (in hexadecimal) is : 0x8CD159C0
ID/EX.PC:32 = 68
ID/EX.readData1:32 = 16 ($6 contains 16)
ID/EX.readData2:32 = 27 ($17 contains 27)
ID/EX.SEConstant:32 = 22976 (0x000059C0) (sign extended IF/ID.I:[15-0])
Note: SEConstant:[15-11] = 11 [rd]
Note: SEConstant:[10-6] = 7 [shamt]
Note: SEConstant:[5-0] = 0 [funct]
ID/EX.rdIndex:5 = 11 (from IF/ID.I:[15-11])
ID/EX.rtIndex:5 = 17 (from IF/ID.I:[20-16])
ID/EX.RegDst:1 = 0
ID/EX.ALUOp:2 = 0
ID/EX.ALUSrc:1 = 1
ID/EX.Branch:1 = 0
ID/EX.MemWrite:1 = 0
ID/EX.MemRead:1 = 1
ID/EX.RegWrite:1 = 1
ID/EX.MemtoReg:1 = 1

EX/MEM state at end of cycle 2:
Instruction (in assembly language) is : sll   $0,$0,0
Instruction (in hexadecimal) is : 0x00000000
EX/MEM.BTA:32 = 0 ((0 << 2) + 0) [00000000]
EX/MEM.ALUResult:32 = 0
EX/MEM.ALUZero:1 = 1
EX/MEM.writeDataMem:32 = 0 ($0 contains 0)
EX/MEM.writeRegister:5 = 0
EX/MEM.Branch:1 = 0
EX/MEM.MemWrite:1 = 0
EX/MEM.MemRead:1 = 0
EX/MEM.RegWrite:1 = 1
EX/MEM.MemtoReg:1 = 0

MEM/WB state at end of cycle 2:
Instruction (in assembly language) is : sll   $0,$0,0
Instruction (in hexadecimal) is : 0x00000000
MEM/WB.ALUResult:32 = 0
MEM/WB.readDataMem:32 = 0
MEM/WB.writeRegister:5 = 0
MEM/WB.RegWrite:1 = 1
MEM/WB.MemtoReg:1 = 0

WB state at end of cycle 2:
Instruction (in assembly language) is : sll   $0,$0,0
Instruction (in hexadecimal) is : 0x00000000
WB.ALUResult:32 = 0
WB.readDataMem:32 = 0
WB.writeRegister:5 = 0
WB.RegWrite:1 = 1
WB.MemtoReg:1 = 0

00000048: 
IF/ID state at end of cycle 3:
Instruction (in assembly language) is : addiu $17,$6,22976
Instruction (in hexadecimal) is : 0x24D159C0
IF/ID.PC:32 = 76
IF/ID.I:32 = 24D159C0
Sub-fields of IF/ID.I are:
IF/ID.I:[31-26] (opcode) = 001001 [9(ten)]
IF/ID.I:[25-21] (rs) = 00110 [$6 aka $a2]
IF/ID.I:[20-16] (rt) = 10001 [$17 aka $s1]
IF/ID.I:[15-11] (rd) = 01011 [$11 aka $t3]
IF/ID.I:[10-6] (shamt) = 00111 [7(ten)]
IF/ID.I:[5-0] (funct) = 000000 [0(ten)]
IF/ID.I:[15-0] (const) = 0101100111000000 [22976(ten)]

ID/EX state at end of cycle 3:
Instruction (in assembly language) is : add   $11,$6,$17
Instruction (in hexadecimal) is : 0x00D15920
ID/EX.PC:32 = 72
ID/EX.readData1:32 = 16 ($6 contains 16)
ID/EX.readData2:32 = 27 ($17 contains 27)
ID/EX.SEConstant:32 = 22816 (0x00005920) (sign extended IF/ID.I:[15-0])
Note: SEConstant:[15-11] = 11 [rd]
Note: SEConstant:[10-6] = 4 [shamt]
Note: SEConstant:[5-0] = 32 [funct]
ID/EX.rdIndex:5 = 11 (from IF/ID.I:[15-11])
ID/EX.rtIndex:5 = 17 (from IF/ID.I:[20-16])
ID/EX.RegDst:1 = 1
ID/EX.ALUOp:2 = 2
ID/EX.ALUSrc:1 = 0
ID/EX.Branch:1 = 0
ID/EX.MemWrite:1 = 0
ID/EX.MemRead:1 = 0
ID/EX.RegWrite:1 = 1
ID/EX.MemtoReg:1 = 0

EX/MEM state at end of cycle 3:
Instruction (in assembly language) is : lw    $17,22976($6)
Instruction (in hexadecimal) is : 0x8CD159C0
EX/MEM.BTA:32 = 91972 ((22976 << 2) + 68) [00016744]
EX/MEM.ALUResult:32 = 22992
EX/MEM.ALUZero:1 = 0
EX/MEM.writeDataMem:32 = 27 ($17 contains 27)
EX/MEM.writeRegister:5 = 17
EX/MEM.Branch:1 = 0
EX/MEM.MemWrite:1 = 0
EX/MEM.MemRead:1 = 1
EX/MEM.RegWrite:1 = 1
EX/MEM.MemtoReg:1 = 1

MEM/WB state at end of cycle 3:
Instruction (in assembly language) is : sll   $0,$0,0
Instruction (in hexadecimal) is : 0x00000000
MEM/WB.ALUResult:32 = 0
MEM/WB.readDataMem:32 = 0
MEM/WB.writeRegister:5 = 0
MEM/WB.RegWrite:1 = 1
MEM/WB.MemtoReg:1 = 0

WB state at end of cycle 3:
Instruction (in assembly language) is : sll   $0,$0,0
Instruction (in hexadecimal) is : 0x00000000
WB.ALUResult:32 = 0
WB.readDataMem:32 = 0
WB.writeRegister:5 = 0
WB.RegWrite:1 = 1
WB.MemtoReg:1 = 0

0000004C: 
IF/ID state at end of cycle 4:
Instruction (in assembly language) is : sll   $11,$17,7
Instruction (in hexadecimal) is : 0x001159C0
IF/ID.PC:32 = 80
IF/ID.I:32 = 001159C0
Sub-fields of IF/ID.I are:
IF/ID.I:[31-26] (opcode) = 000000 [0(ten)]
IF/ID.I:[25-21] (rs) = 00000 [$0 aka $zero]
IF/ID.I:[20-16] (rt) = 10001 [$17 aka $s1]
IF/ID.I:[15-11] (rd) = 01011 [$11 aka $t3]
IF/ID.I:[10-6] (shamt) = 00111 [7(ten)]
IF/ID.I:[5-0] (funct) = 000000 [0(ten)]
IF/ID.I:[15-0] (const) = 0101100111000000 [22976(ten)]

ID/EX state at end of cycle 4:
Instruction (in assembly language) is : addiu $17,$6,22976
Instruction (in hexadecimal) is : 0x24D159C0
ID/EX.PC:32 = 76
ID/EX.readData1:32 = 16 ($6 contains 16)
ID/EX.readData2:32 = 27 ($17 contains 27)
ID/EX.SEConstant:32 = 22976 (0x000059C0) (sign extended IF/ID.I:[15-0])
Note: SEConstant:[15-11] = 11 [rd]
Note: SEConstant:[10-6] = 7 [shamt]
Note: SEConstant:[5-0] = 0 [funct]
ID/EX.rdIndex:5 = 11 (from IF/ID.I:[15-11])
ID/EX.rtIndex:5 = 17 (from IF/ID.I:[20-16])
ID/EX.RegDst:1 = 0
ID/EX.ALUOp:2 = 0
ID/EX.ALUSrc:1 = 1
ID/EX.Branch:1 = 0
ID/EX.MemWrite:1 = 0
ID/EX.MemRead:1 = 0
ID/EX.RegWrite:1 = 1
ID/EX.MemtoReg:1 = 0

EX/MEM state at end of cycle 4:
Instruction (in assembly language) is : add   $11,$6,$17
Instruction (in hexadecimal) is : 0x00D15920
EX/MEM.BTA:32 = 91336 ((22816 << 2) + 72) [000164C8]
EX/MEM.ALUResult:32 = 43
EX/MEM.ALUZero:1 = 0
EX/MEM.writeDataMem:32 = 27 ($17 contains 27)
EX/MEM.writeRegister:5 = 11
EX/MEM.Branch:1 = 0
EX/MEM.MemWrite:1 = 0
EX/MEM.MemRead:1 = 0
EX/MEM.RegWrite:1 = 1
EX/MEM.MemtoReg:1 = 0

MEM/WB state at end of cycle 4:
Instruction (in assembly language) is : lw    $17,22976($6)
Instruction (in hexadecimal) is : 0x8CD159C0
MEM/WB.ALUResult:32 = 22992
MEM/WB.readDataMem:32 = 91904
MEM/WB.writeRegister:5 = 17
MEM/WB.RegWrite:1 = 1
MEM/WB.MemtoReg:1 = 1

WB state at end of cycle 4:
Instruction (in assembly language) is : sll   $0,$0,0
Instruction (in hexadecimal) is : 0x00000000
WB.ALUResult:32 = 0
WB.readDataMem:32 = 0
WB.writeRegister:5 = 0
WB.RegWrite:1 = 1
WB.MemtoReg:1 = 0

00000050: 
IF/ID state at end of cycle 5:
Instruction (in assembly language) is : sw    $17,22976($6)
Instruction (in hexadecimal) is : 0xACD159C0
IF/ID.PC:32 = 84
IF/ID.I:32 = ACD159C0
Sub-fields of IF/ID.I are:
IF/ID.I:[31-26] (opcode) = 101011 [43(ten)]
IF/ID.I:[25-21] (rs) = 00110 [$6 aka $a2]
IF/ID.I:[20-16] (rt) = 10001 [$17 aka $s1]
IF/ID.I:[15-11] (rd) = 01011 [$11 aka $t3]
IF/ID.I:[10-6] (shamt) = 00111 [7(ten)]
IF/ID.I:[5-0] (funct) = 000000 [0(ten)]
IF/ID.I:[15-0] (const) = 0101100111000000 [22976(ten)]

ID/EX state at end of cycle 5:
Instruction (in assembly language) is : sll   $11,$17,7
Instruction (in hexadecimal) is : 0x001159C0
ID/EX.PC:32 = 80
ID/EX.readData1:32 = 0 ($0 contains 0)
ID/EX.readData2:32 = 27 ($17 contains 27)
ID/EX.SEConstant:32 = 22976 (0x000059C0) (sign extended IF/ID.I:[15-0])
Note: SEConstant:[15-11] = 11 [rd]
Note: SEConstant:[10-6] = 7 [shamt]
Note: SEConstant:[5-0] = 0 [funct]
ID/EX.rdIndex:5 = 11 (from IF/ID.I:[15-11])
ID/EX.rtIndex:5 = 17 (from IF/ID.I:[20-16])
ID/EX.RegDst:1 = 1
ID/EX.ALUOp:2 = 2
ID/EX.ALUSrc:1 = 0
ID/EX.Branch:1 = 0
ID/EX.MemWrite:1 = 0
ID/EX.MemRead:1 = 0
ID/EX.RegWrite:1 = 1
ID/EX.MemtoReg:1 = 0

EX/MEM state at end of cycle 5:
Instruction (in assembly language) is : addiu $17,$6,22976
Instruction (in hexadecimal) is : 0x24D159C0
EX/MEM.BTA:32 = 91980 ((22976 << 2) + 76) [0001674C]
EX/MEM.ALUResult:32 = 22992
EX/MEM.ALUZero:1 = 0
EX/MEM.writeDataMem:32 = 27 ($17 contains 27)
EX/MEM.writeRegister:5 = 17
EX/MEM.Branch:1 = 0
EX/MEM.MemWrite:1 = 0
EX/MEM.MemRead:1 = 0
EX/MEM.RegWrite:1 = 1
EX/MEM.MemtoReg:1 = 0

MEM/WB state at end of cycle 5:
Instruction (in assembly language) is : add   $11,$6,$17
Instruction (in hexadecimal) is : 0x00D15920
MEM/WB.ALUResult:32 = 43
MEM/WB.readDataMem:32 = 0
MEM/WB.writeRegister:5 = 11
MEM/WB.RegWrite:1 = 1
MEM/WB.MemtoReg:1 = 0

WB state at end of cycle 5:
Instruction (in assembly language) is : lw    $17,22976($6)
Instruction (in hexadecimal) is : 0x8CD159C0
WB.ALUResult:32 = 22992
WB.readDataMem:32 = 91904
WB.writeRegister:5 = 17
WB.RegWrite:1 = 1
WB.MemtoReg:1 = 1

00000054: 
IF/ID state at end of cycle 6:
Instruction (in assembly language) is : slt   $1,$2,$3
Instruction (in hexadecimal) is : 0x0043082A
IF/ID.PC:32 = 88
IF/ID.I:32 = 0043082A
Sub-fields of IF/ID.I are:
IF/ID.I:[31-26] (opcode) = 000000 [0(ten)]
IF/ID.I:[25-21] (rs) = 00010 [$2 aka $v0]
IF/ID.I:[20-16] (rt) = 00011 [$3 aka $v1]
IF/ID.I:[15-11] (rd) = 00001 [$1 aka $at]
IF/ID.I:[10-6] (shamt) = 00000 [0(ten)]
IF/ID.I:[5-0] (funct) = 101010 [42(ten)]
IF/ID.I:[15-0] (const) = 0000100000101010 [2090(ten)]

ID/EX state at end of cycle 6:
Instruction (in assembly language) is : sw    $17,22976($6)
Instruction (in hexadecimal) is : 0xACD159C0
ID/EX.PC:32 = 84
ID/EX.readData1:32 = 16 ($6 contains 16)
ID/EX.readData2:32 = 27 ($17 contains 27)
ID/EX.SEConstant:32 = 22976 (0x000059C0) (sign extended IF/ID.I:[15-0])
Note: SEConstant:[15-11] = 11 [rd]
Note: SEConstant:[10-6] = 7 [shamt]
Note: SEConstant:[5-0] = 0 [funct]
ID/EX.rdIndex:5 = 11 (from IF/ID.I:[15-11])
ID/EX.rtIndex:5 = 17 (from IF/ID.I:[20-16])
ID/EX.RegDst:1 = 0
ID/EX.ALUOp:2 = 0
ID/EX.ALUSrc:1 = 1
ID/EX.Branch:1 = 0
ID/EX.MemWrite:1 = 1
ID/EX.MemRead:1 = 0
ID/EX.RegWrite:1 = 0
ID/EX.MemtoReg:1 = 0

EX/MEM state at end of cycle 6:
Instruction (in assembly language) is : sll   $11,$17,7
Instruction (in hexadecimal) is : 0x001159C0
EX/MEM.BTA:32 = 91984 ((22976 << 2) + 80) [00016750]
EX/MEM.ALUResult:32 = 3456
EX/MEM.ALUZero:1 = 0
EX/MEM.writeDataMem:32 = 27 ($17 contains 27)
EX/MEM.writeRegister:5 = 11
EX/MEM.Branch:1 = 0
EX/MEM.MemWrite:1 = 0
EX/MEM.MemRead:1 = 0
EX/MEM.RegWrite:1 = 1
EX/MEM.MemtoReg:1 = 0

MEM/WB state at end of cycle 6:
Instruction (in assembly language) is : addiu $17,$6,22976
Instruction (in hexadecimal) is : 0x24D159C0
MEM/WB.ALUResult:32 = 22992
MEM/WB.readDataMem:32 = 0
MEM/WB.writeRegister:5 = 17
MEM/WB.RegWrite:1 = 1
MEM/WB.MemtoReg:1 = 0

WB state at end of cycle 6:
Instruction (in assembly language) is : add   $11,$6,$17
Instruction (in hexadecimal) is : 0x00D15920
WB.ALUResult:32 = 43
WB.readDataMem:32 = 0
WB.writeRegister:5 = 11
WB.RegWrite:1 = 1
WB.MemtoReg:1 = 0

00000058: 
IF/ID state at end of cycle 7:
Instruction (in assembly language) is : sub   $1,$2,$3
Instruction (in hexadecimal) is : 0x00430822
IF/ID.PC:32 = 92
IF/ID.I:32 = 00430822
Sub-fields of IF/ID.I are:
IF/ID.I:[31-26] (opcode) = 000000 [0(ten)]
IF/ID.I:[25-21] (rs) = 00010 [$2 aka $v0]
IF/ID.I:[20-16] (rt) = 00011 [$3 aka $v1]
IF/ID.I:[15-11] (rd) = 00001 [$1 aka $at]
IF/ID.I:[10-6] (shamt) = 00000 [0(ten)]
IF/ID.I:[5-0] (funct) = 100010 [34(ten)]
IF/ID.I:[15-0] (const) = 0000100000100010 [2082(ten)]

ID/EX state at end of cycle 7:
Instruction (in assembly language) is : slt   $1,$2,$3
Instruction (in hexadecimal) is : 0x0043082A
ID/EX.PC:32 = 88
ID/EX.readData1:32 = 12 ($2 contains 12)
ID/EX.readData2:32 = 13 ($3 contains 13)
ID/EX.SEConstant:32 = 2090 (0x0000082A) (sign extended IF/ID.I:[15-0])
Note: SEConstant:[15-11] = 1 [rd]
Note: SEConstant:[10-6] = 0 [shamt]
Note: SEConstant:[5-0] = 42 [funct]
ID/EX.rdIndex:5 = 1 (from IF/ID.I:[15-11])
ID/EX.rtIndex:5 = 3 (from IF/ID.I:[20-16])
ID/EX.RegDst:1 = 1
ID/EX.ALUOp:2 = 2
ID/EX.ALUSrc:1 = 0
ID/EX.Branch:1 = 0
ID/EX.MemWrite:1 = 0
ID/EX.MemRead:1 = 0
ID/EX.RegWrite:1 = 1
ID/EX.MemtoReg:1 = 0

EX/MEM state at end of cycle 7:
Instruction (in assembly language) is : sw    $17,22976($6)
Instruction (in hexadecimal) is : 0xACD159C0
EX/MEM.BTA:32 = 91988 ((22976 << 2) + 84) [00016754]
EX/MEM.ALUResult:32 = 22992
EX/MEM.ALUZero:1 = 0
EX/MEM.writeDataMem:32 = 27 ($17 contains 27)
EX/MEM.writeRegister:5 = 17
EX/MEM.Branch:1 = 0
EX/MEM.MemWrite:1 = 1
EX/MEM.MemRead:1 = 0
EX/MEM.RegWrite:1 = 0
EX/MEM.MemtoReg:1 = 0

MEM/WB state at end of cycle 7:
Instruction (in assembly language) is : sll   $11,$17,7
Instruction (in hexadecimal) is : 0x001159C0
MEM/WB.ALUResult:32 = 3456
MEM/WB.readDataMem:32 = 0
MEM/WB.writeRegister:5 = 11
MEM/WB.RegWrite:1 = 1
MEM/WB.MemtoReg:1 = 0

WB state at end of cycle 7:
Instruction (in assembly language) is : addiu $17,$6,22976
Instruction (in hexadecimal) is : 0x24D159C0
WB.ALUResult:32 = 22992
WB.readDataMem:32 = 0
WB.writeRegister:5 = 17
WB.RegWrite:1 = 1
WB.MemtoReg:1 = 0

0000005C: 
IF/ID state at end of cycle 8:
Instruction (in assembly language) is : beq   $0,$0,0000005C
Instruction (in hexadecimal) is : 0x1000FFFF
IF/ID.PC:32 = 96
IF/ID.I:32 = 1000FFFF
Sub-fields of IF/ID.I are:
IF/ID.I:[31-26] (opcode) = 000100 [4(ten)]
IF/ID.I:[25-21] (rs) = 00000 [$0 aka $zero]
IF/ID.I:[20-16] (rt) = 00000 [$0 aka $zero]
IF/ID.I:[15-11] (rd) = 11111 [$31 aka $ra]
IF/ID.I:[10-6] (shamt) = 11111 [31(ten)]
IF/ID.I:[5-0] (funct) = 111111 [63(ten)]
IF/ID.I:[15-0] (const) = 1111111111111111 [-1(ten)]

ID/EX state at end of cycle 8:
Instruction (in assembly language) is : sub   $1,$2,$3
Instruction (in hexadecimal) is : 0x00430822
ID/EX.PC:32 = 92
ID/EX.readData1:32 = 12 ($2 contains 12)
ID/EX.readData2:32 = 13 ($3 contains 13)
ID/EX.SEConstant:32 = 2082 (0x00000822) (sign extended IF/ID.I:[15-0])
Note: SEConstant:[15-11] = 1 [rd]
Note: SEConstant:[10-6] = 0 [shamt]
Note: SEConstant:[5-0] = 34 [funct]
ID/EX.rdIndex:5 = 1 (from IF/ID.I:[15-11])
ID/EX.rtIndex:5 = 3 (from IF/ID.I:[20-16])
ID/EX.RegDst:1 = 1
ID/EX.ALUOp:2 = 2
ID/EX.ALUSrc:1 = 0
ID/EX.Branch:1 = 0
ID/EX.MemWrite:1 = 0
ID/EX.MemRead:1 = 0
ID/EX.RegWrite:1 = 1
ID/EX.MemtoReg:1 = 0

EX/MEM state at end of cycle 8:
Instruction (in assembly language) is : slt   $1,$2,$3
Instruction (in hexadecimal) is : 0x0043082A
EX/MEM.BTA:32 = 8448 ((2090 << 2) + 88) [00002100]
EX/MEM.ALUResult:32 = 1
EX/MEM.ALUZero:1 = 0
EX/MEM.writeDataMem:32 = 13 ($3 contains 13)
EX/MEM.writeRegister:5 = 1
EX/MEM.Branch:1 = 0
EX/MEM.MemWrite:1 = 0
EX/MEM.MemRead:1 = 0
EX/MEM.RegWrite:1 = 1
EX/MEM.MemtoReg:1 = 0

MEM/WB state at end of cycle 8:
Instruction (in assembly language) is : sw    $17,22976($6)
Instruction (in hexadecimal) is : 0xACD159C0
MEM/WB.ALUResult:32 = 22992
MEM/WB.readDataMem:32 = 0
MEM/WB.writeRegister:5 = 17
MEM/WB.RegWrite:1 = 0
MEM/WB.MemtoReg:1 = 0

WB state at end of cycle 8:
Instruction (in assembly language) is : sll   $11,$17,7
Instruction (in hexadecimal) is : 0x001159C0
WB.ALUResult:32 = 3456
WB.readDataMem:32 = 0
WB.writeRegister:5 = 11
WB.RegWrite:1 = 1
WB.MemtoReg:1 = 0

00000060: 
IF/ID state at end of cycle 9:
Instruction (in assembly language) is : or    $4,$5,$6
Instruction (in hexadecimal) is : 0x00A62025
IF/ID.PC:32 = 100
IF/ID.I:32 = 00A62025
Sub-fields of IF/ID.I are:
IF/ID.I:[31-26] (opcode) = 000000 [0(ten)]
IF/ID.I:[25-21] (rs) = 00101 [$5 aka $a1]
IF/ID.I:[20-16] (rt) = 00110 [$6 aka $a2]
IF/ID.I:[15-11] (rd) = 00100 [$4 aka $a0]
IF/ID.I:[10-6] (shamt) = 00000 [0(ten)]
IF/ID.I:[5-0] (funct) = 100101 [37(ten)]
IF/ID.I:[15-0] (const) = 0010000000100101 [8229(ten)]

ID/EX state at end of cycle 9:
Instruction (in assembly language) is : beq   $0,$0,0000005C
Instruction (in hexadecimal) is : 0x1000FFFF
ID/EX.PC:32 = 96
ID/EX.readData1:32 = 0 ($0 contains 0)
ID/EX.readData2:32 = 0 ($0 contains 0)
ID/EX.SEConstant:32 = -1 (0xFFFFFFFF) (sign extended IF/ID.I:[15-0])
Note: SEConstant:[15-11] = 31 [rd]
Note: SEConstant:[10-6] = 31 [shamt]
Note: SEConstant:[5-0] = 63 [funct]
ID/EX.rdIndex:5 = 31 (from IF/ID.I:[15-11])
ID/EX.rtIndex:5 = 0 (from IF/ID.I:[20-16])
ID/EX.RegDst:1 = 0
ID/EX.ALUOp:2 = 1
ID/EX.ALUSrc:1 = 0
ID/EX.Branch:1 = 1
ID/EX.MemWrite:1 = 0
ID/EX.MemRead:1 = 0
ID/EX.RegWrite:1 = 0
ID/EX.MemtoReg:1 = 0

EX/MEM state at end of cycle 9:
Instruction (in assembly language) is : sub   $1,$2,$3
Instruction (in hexadecimal) is : 0x00430822
EX/MEM.BTA:32 = 8420 ((2082 << 2) + 92) [000020E4]
EX/MEM.ALUResult:32 = -1
EX/MEM.ALUZero:1 = 0
EX/MEM.writeDataMem:32 = 13 ($3 contains 13)
EX/MEM.writeRegister:5 = 1
EX/MEM.Branch:1 = 0
EX/MEM.MemWrite:1 = 0
EX/MEM.MemRead:1 = 0
EX/MEM.RegWrite:1 = 1
EX/MEM.MemtoReg:1 = 0

MEM/WB state at end of cycle 9:
Instruction (in assembly language) is : slt   $1,$2,$3
Instruction (in hexadecimal) is : 0x0043082A
MEM/WB.ALUResult:32 = 1
MEM/WB.readDataMem:32 = 0
MEM/WB.writeRegister:5 = 1
MEM/WB.RegWrite:1 = 1
MEM/WB.MemtoReg:1 = 0

WB state at end of cycle 9:
Instruction (in assembly language) is : sw    $17,22976($6)
Instruction (in hexadecimal) is : 0xACD159C0
WB.ALUResult:32 = 22992
WB.readDataMem:32 = 0
WB.writeRegister:5 = 17
WB.RegWrite:1 = 0
WB.MemtoReg:1 = 0

00000064: 
IF/ID state at end of cycle 10:
Instruction (in assembly language) is : and   $3,$4,$5
Instruction (in hexadecimal) is : 0x00851824
IF/ID.PC:32 = 104
IF/ID.I:32 = 00851824
Sub-fields of IF/ID.I are:
IF/ID.I:[31-26] (opcode) = 000000 [0(ten)]
IF/ID.I:[25-21] (rs) = 00100 [$4 aka $a0]
IF/ID.I:[20-16] (rt) = 00101 [$5 aka $a1]
IF/ID.I:[15-11] (rd) = 00011 [$3 aka $v1]
IF/ID.I:[10-6] (shamt) = 00000 [0(ten)]
IF/ID.I:[5-0] (funct) = 100100 [36(ten)]
IF/ID.I:[15-0] (const) = 0001100000100100 [6180(ten)]

ID/EX state at end of cycle 10:
Instruction (in assembly language) is : or    $4,$5,$6
Instruction (in hexadecimal) is : 0x00A62025
ID/EX.PC:32 = 100
ID/EX.readData1:32 = 15 ($5 contains 15)
ID/EX.readData2:32 = 16 ($6 contains 16)
ID/EX.SEConstant:32 = 8229 (0x00002025) (sign extended IF/ID.I:[15-0])
Note: SEConstant:[15-11] = 4 [rd]
Note: SEConstant:[10-6] = 0 [shamt]
Note: SEConstant:[5-0] = 37 [funct]
ID/EX.rdIndex:5 = 4 (from IF/ID.I:[15-11])
ID/EX.rtIndex:5 = 6 (from IF/ID.I:[20-16])
ID/EX.RegDst:1 = 1
ID/EX.ALUOp:2 = 2
ID/EX.ALUSrc:1 = 0
ID/EX.Branch:1 = 0
ID/EX.MemWrite:1 = 0
ID/EX.MemRead:1 = 0
ID/EX.RegWrite:1 = 1
ID/EX.MemtoReg:1 = 0

EX/MEM state at end of cycle 10:
Instruction (in assembly language) is : beq   $0,$0,0000005C
Instruction (in hexadecimal) is : 0x1000FFFF
EX/MEM.BTA:32 = 92 ((-1 << 2) + 96) [0000005C]
EX/MEM.ALUResult:32 = 0
EX/MEM.ALUZero:1 = 1
EX/MEM.writeDataMem:32 = 0 ($0 contains 0)
EX/MEM.writeRegister:5 = 0
EX/MEM.Branch:1 = 1
EX/MEM.MemWrite:1 = 0
EX/MEM.MemRead:1 = 0
EX/MEM.RegWrite:1 = 0
EX/MEM.MemtoReg:1 = 0

MEM/WB state at end of cycle 10:
Instruction (in assembly language) is : sub   $1,$2,$3
Instruction (in hexadecimal) is : 0x00430822
MEM/WB.ALUResult:32 = -1
MEM/WB.readDataMem:32 = 0
MEM/WB.writeRegister:5 = 1
MEM/WB.RegWrite:1 = 1
MEM/WB.MemtoReg:1 = 0

WB state at end of cycle 10:
Instruction (in assembly language) is : slt   $1,$2,$3
Instruction (in hexadecimal) is : 0x0043082A
WB.ALUResult:32 = 1
WB.readDataMem:32 = 0
WB.writeRegister:5 = 1
WB.RegWrite:1 = 1
WB.MemtoReg:1 = 0

00000068: 
IF/ID state at end of cycle 11:
Instruction (in assembly language) is : sll   $2,$1,0
Instruction (in hexadecimal) is : 0x00011000
IF/ID.PC:32 = 108
IF/ID.I:32 = 00011000
Sub-fields of IF/ID.I are:
IF/ID.I:[31-26] (opcode) = 000000 [0(ten)]
IF/ID.I:[25-21] (rs) = 00000 [$0 aka $zero]
IF/ID.I:[20-16] (rt) = 00001 [$1 aka $at]
IF/ID.I:[15-11] (rd) = 00010 [$2 aka $v0]
IF/ID.I:[10-6] (shamt) = 00000 [0(ten)]
IF/ID.I:[5-0] (funct) = 000000 [0(ten)]
IF/ID.I:[15-0] (const) = 0001000000000000 [4096(ten)]

ID/EX state at end of cycle 11:
Instruction (in assembly language) is : and   $3,$4,$5
Instruction (in hexadecimal) is : 0x00851824
ID/EX.PC:32 = 104
ID/EX.readData1:32 = 14 ($4 contains 14)
ID/EX.readData2:32 = 15 ($5 contains 15)
ID/EX.SEConstant:32 = 6180 (0x00001824) (sign extended IF/ID.I:[15-0])
Note: SEConstant:[15-11] = 3 [rd]
Note: SEConstant:[10-6] = 0 [shamt]
Note: SEConstant:[5-0] = 36 [funct]
ID/EX.rdIndex:5 = 3 (from IF/ID.I:[15-11])
ID/EX.rtIndex:5 = 5 (from IF/ID.I:[20-16])
ID/EX.RegDst:1 = 1
ID/EX.ALUOp:2 = 2
ID/EX.ALUSrc:1 = 0
ID/EX.Branch:1 = 0
ID/EX.MemWrite:1 = 0
ID/EX.MemRead:1 = 0
ID/EX.RegWrite:1 = 1
ID/EX.MemtoReg:1 = 0

EX/MEM state at end of cycle 11:
Instruction (in assembly language) is : or    $4,$5,$6
Instruction (in hexadecimal) is : 0x00A62025
EX/MEM.BTA:32 = 33016 ((8229 << 2) + 100) [000080F8]
EX/MEM.ALUResult:32 = 31
EX/MEM.ALUZero:1 = 0
EX/MEM.writeDataMem:32 = 16 ($6 contains 16)
EX/MEM.writeRegister:5 = 4
EX/MEM.Branch:1 = 0
EX/MEM.MemWrite:1 = 0
EX/MEM.MemRead:1 = 0
EX/MEM.RegWrite:1 = 1
EX/MEM.MemtoReg:1 = 0

MEM/WB state at end of cycle 11:
Instruction (in assembly language) is : beq   $0,$0,0000005C
Instruction (in hexadecimal) is : 0x1000FFFF
MEM/WB.ALUResult:32 = 0
MEM/WB.readDataMem:32 = 0
MEM/WB.writeRegister:5 = 0
MEM/WB.RegWrite:1 = 0
MEM/WB.MemtoReg:1 = 0

WB state at end of cycle 11:
Instruction (in assembly language) is : sub   $1,$2,$3
Instruction (in hexadecimal) is : 0x00430822
WB.ALUResult:32 = -1
WB.readDataMem:32 = 0
WB.writeRegister:5 = 1
WB.RegWrite:1 = 1
WB.MemtoReg:1 = 0

0000005C: 
IF/ID state at end of cycle 12:
Instruction (in assembly language) is : sll   $3,$2,1
Instruction (in hexadecimal) is : 0x00021840
IF/ID.PC:32 = 96
IF/ID.I:32 = 00021840
Sub-fields of IF/ID.I are:
IF/ID.I:[31-26] (opcode) = 000000 [0(ten)]
IF/ID.I:[25-21] (rs) = 00000 [$0 aka $zero]
IF/ID.I:[20-16] (rt) = 00010 [$2 aka $v0]
IF/ID.I:[15-11] (rd) = 00011 [$3 aka $v1]
IF/ID.I:[10-6] (shamt) = 00001 [1(ten)]
IF/ID.I:[5-0] (funct) = 000000 [0(ten)]
IF/ID.I:[15-0] (const) = 0001100001000000 [6208(ten)]

ID/EX state at end of cycle 12:
Instruction (in assembly language) is : sll   $2,$1,0
Instruction (in hexadecimal) is : 0x00011000
ID/EX.PC:32 = 108
ID/EX.readData1:32 = 0 ($0 contains 0)
ID/EX.readData2:32 = 11 ($1 contains 11)
ID/EX.SEConstant:32 = 4096 (0x00001000) (sign extended IF/ID.I:[15-0])
Note: SEConstant:[15-11] = 2 [rd]
Note: SEConstant:[10-6] = 0 [shamt]
Note: SEConstant:[5-0] = 0 [funct]
ID/EX.rdIndex:5 = 2 (from IF/ID.I:[15-11])
ID/EX.rtIndex:5 = 1 (from IF/ID.I:[20-16])
ID/EX.RegDst:1 = 1
ID/EX.ALUOp:2 = 2
ID/EX.ALUSrc:1 = 0
ID/EX.Branch:1 = 0
ID/EX.MemWrite:1 = 0
ID/EX.MemRead:1 = 0
ID/EX.RegWrite:1 = 1
ID/EX.MemtoReg:1 = 0

EX/MEM state at end of cycle 12:
Instruction (in assembly language) is : and   $3,$4,$5
Instruction (in hexadecimal) is : 0x00851824
EX/MEM.BTA:32 = 24824 ((6180 << 2) + 104) [000060F8]
EX/MEM.ALUResult:32 = 14
EX/MEM.ALUZero:1 = 0
EX/MEM.writeDataMem:32 = 15 ($5 contains 15)
EX/MEM.writeRegister:5 = 3
EX/MEM.Branch:1 = 0
EX/MEM.MemWrite:1 = 0
EX/MEM.MemRead:1 = 0
EX/MEM.RegWrite:1 = 1
EX/MEM.MemtoReg:1 = 0

MEM/WB state at end of cycle 12:
Instruction (in assembly language) is : or    $4,$5,$6
Instruction (in hexadecimal) is : 0x00A62025
MEM/WB.ALUResult:32 = 31
MEM/WB.readDataMem:32 = 0
MEM/WB.writeRegister:5 = 4
MEM/WB.RegWrite:1 = 1
MEM/WB.MemtoReg:1 = 0

WB state at end of cycle 12:
Instruction (in assembly language) is : beq   $0,$0,0000005C
Instruction (in hexadecimal) is : 0x1000FFFF
WB.ALUResult:32 = 0
WB.readDataMem:32 = 0
WB.writeRegister:5 = 0
WB.RegWrite:1 = 0
WB.MemtoReg:1 = 0

00000060: 
IF/ID state at end of cycle 13:
Instruction (in assembly language) is : sll   $4,$3,2
Instruction (in hexadecimal) is : 0x00032080
IF/ID.PC:32 = 100
IF/ID.I:32 = 00032080
Sub-fields of IF/ID.I are:
IF/ID.I:[31-26] (opcode) = 000000 [0(ten)]
IF/ID.I:[25-21] (rs) = 00000 [$0 aka $zero]
IF/ID.I:[20-16] (rt) = 00011 [$3 aka $v1]
IF/ID.I:[15-11] (rd) = 00100 [$4 aka $a0]
IF/ID.I:[10-6] (shamt) = 00010 [2(ten)]
IF/ID.I:[5-0] (funct) = 000000 [0(ten)]
IF/ID.I:[15-0] (const) = 0010000010000000 [8320(ten)]

ID/EX state at end of cycle 13:
Instruction (in assembly language) is : sll   $3,$2,1
Instruction (in hexadecimal) is : 0x00021840
ID/EX.PC:32 = 96
ID/EX.readData1:32 = 0 ($0 contains 0)
ID/EX.readData2:32 = 12 ($2 contains 12)
ID/EX.SEConstant:32 = 6208 (0x00001840) (sign extended IF/ID.I:[15-0])
Note: SEConstant:[15-11] = 3 [rd]
Note: SEConstant:[10-6] = 1 [shamt]
Note: SEConstant:[5-0] = 0 [funct]
ID/EX.rdIndex:5 = 3 (from IF/ID.I:[15-11])
ID/EX.rtIndex:5 = 2 (from IF/ID.I:[20-16])
ID/EX.RegDst:1 = 1
ID/EX.ALUOp:2 = 2
ID/EX.ALUSrc:1 = 0
ID/EX.Branch:1 = 0
ID/EX.MemWrite:1 = 0
ID/EX.MemRead:1 = 0
ID/EX.RegWrite:1 = 1
ID/EX.MemtoReg:1 = 0

EX/MEM state at end of cycle 13:
Instruction (in assembly language) is : sll   $2,$1,0
Instruction (in hexadecimal) is : 0x00011000
EX/MEM.BTA:32 = 16492 ((4096 << 2) + 108) [0000406C]
EX/MEM.ALUResult:32 = 11
EX/MEM.ALUZero:1 = 0
EX/MEM.writeDataMem:32 = 11 ($1 contains 11)
EX/MEM.writeRegister:5 = 2
EX/MEM.Branch:1 = 0
EX/MEM.MemWrite:1 = 0
EX/MEM.MemRead:1 = 0
EX/MEM.RegWrite:1 = 1
EX/MEM.MemtoReg:1 = 0

MEM/WB state at end of cycle 13:
Instruction (in assembly language) is : and   $3,$4,$5
Instruction (in hexadecimal) is : 0x00851824
MEM/WB.ALUResult:32 = 14
MEM/WB.readDataMem:32 = 0
MEM/WB.writeRegister:5 = 3
MEM/WB.RegWrite:1 = 1
MEM/WB.MemtoReg:1 = 0

WB state at end of cycle 13:
Instruction (in assembly language) is : or    $4,$5,$6
Instruction (in hexadecimal) is : 0x00A62025
WB.ALUResult:32 = 31
WB.readDataMem:32 = 0
WB.writeRegister:5 = 4
WB.RegWrite:1 = 1
WB.MemtoReg:1 = 0

00000064: 
IF/ID state at end of cycle 14:
Instruction (in assembly language) is : sll   $5,$4,3
Instruction (in hexadecimal) is : 0x000428C0
IF/ID.PC:32 = 104
IF/ID.I:32 = 000428C0
Sub-fields of IF/ID.I are:
IF/ID.I:[31-26] (opcode) = 000000 [0(ten)]
IF/ID.I:[25-21] (rs) = 00000 [$0 aka $zero]
IF/ID.I:[20-16] (rt) = 00100 [$4 aka $a0]
IF/ID.I:[15-11] (rd) = 00101 [$5 aka $a1]
IF/ID.I:[10-6] (shamt) = 00011 [3(ten)]
IF/ID.I:[5-0] (funct) = 000000 [0(ten)]
IF/ID.I:[15-0] (const) = 0010100011000000 [10432(ten)]

ID/EX state at end of cycle 14:
Instruction (in assembly language) is : sll   $4,$3,2
Instruction (in hexadecimal) is : 0x00032080
ID/EX.PC:32 = 100
ID/EX.readData1:32 = 0 ($0 contains 0)
ID/EX.readData2:32 = 13 ($3 contains 13)
ID/EX.SEConstant:32 = 8320 (0x00002080) (sign extended IF/ID.I:[15-0])
Note: SEConstant:[15-11] = 4 [rd]
Note: SEConstant:[10-6] = 2 [shamt]
Note: SEConstant:[5-0] = 0 [funct]
ID/EX.rdIndex:5 = 4 (from IF/ID.I:[15-11])
ID/EX.rtIndex:5 = 3 (from IF/ID.I:[20-16])
ID/EX.RegDst:1 = 1
ID/EX.ALUOp:2 = 2
ID/EX.ALUSrc:1 = 0
ID/EX.Branch:1 = 0
ID/EX.MemWrite:1 = 0
ID/EX.MemRead:1 = 0
ID/EX.RegWrite:1 = 1
ID/EX.MemtoReg:1 = 0

EX/MEM state at end of cycle 14:
Instruction (in assembly language) is : sll   $3,$2,1
Instruction (in hexadecimal) is : 0x00021840
EX/MEM.BTA:32 = 24928 ((6208 << 2) + 96) [00006160]
EX/MEM.ALUResult:32 = 24
EX/MEM.ALUZero:1 = 0
EX/MEM.writeDataMem:32 = 12 ($2 contains 12)
EX/MEM.writeRegister:5 = 3
EX/MEM.Branch:1 = 0
EX/MEM.MemWrite:1 = 0
EX/MEM.MemRead:1 = 0
EX/MEM.RegWrite:1 = 1
EX/MEM.MemtoReg:1 = 0

MEM/WB state at end of cycle 14:
Instruction (in assembly language) is : sll   $2,$1,0
Instruction (in hexadecimal) is : 0x00011000
MEM/WB.ALUResult:32 = 11
MEM/WB.readDataMem:32 = 0
MEM/WB.writeRegister:5 = 2
MEM/WB.RegWrite:1 = 1
MEM/WB.MemtoReg:1 = 0

WB state at end of cycle 14:
Instruction (in assembly language) is : and   $3,$4,$5
Instruction (in hexadecimal) is : 0x00851824
WB.ALUResult:32 = 14
WB.readDataMem:32 = 0
WB.writeRegister:5 = 3
WB.RegWrite:1 = 1
WB.MemtoReg:1 = 0

00000068: 
IF/ID state at end of cycle 15:
Instruction (in assembly language) is : sll   $6,$5,4
Instruction (in hexadecimal) is : 0x00053100
IF/ID.PC:32 = 108
IF/ID.I:32 = 00053100
Sub-fields of IF/ID.I are:
IF/ID.I:[31-26] (opcode) = 000000 [0(ten)]
IF/ID.I:[25-21] (rs) = 00000 [$0 aka $zero]
IF/ID.I:[20-16] (rt) = 00101 [$5 aka $a1]
IF/ID.I:[15-11] (rd) = 00110 [$6 aka $a2]
IF/ID.I:[10-6] (shamt) = 00100 [4(ten)]
IF/ID.I:[5-0] (funct) = 000000 [0(ten)]
IF/ID.I:[15-0] (const) = 0011000100000000 [12544(ten)]

ID/EX state at end of cycle 15:
Instruction (in assembly language) is : sll   $5,$4,3
Instruction (in hexadecimal) is : 0x000428C0
ID/EX.PC:32 = 104
ID/EX.readData1:32 = 0 ($0 contains 0)
ID/EX.readData2:32 = 14 ($4 contains 14)
ID/EX.SEConstant:32 = 10432 (0x000028C0) (sign extended IF/ID.I:[15-0])
Note: SEConstant:[15-11] = 5 [rd]
Note: SEConstant:[10-6] = 3 [shamt]
Note: SEConstant:[5-0] = 0 [funct]
ID/EX.rdIndex:5 = 5 (from IF/ID.I:[15-11])
ID/EX.rtIndex:5 = 4 (from IF/ID.I:[20-16])
ID/EX.RegDst:1 = 1
ID/EX.ALUOp:2 = 2
ID/EX.ALUSrc:1 = 0
ID/EX.Branch:1 = 0
ID/EX.MemWrite:1 = 0
ID/EX.MemRead:1 = 0
ID/EX.RegWrite:1 = 1
ID/EX.MemtoReg:1 = 0

EX/MEM state at end of cycle 15:
Instruction (in assembly language) is : sll   $4,$3,2
Instruction (in hexadecimal) is : 0x00032080
EX/MEM.BTA:32 = 33380 ((8320 << 2) + 100) [00008264]
EX/MEM.ALUResult:32 = 52
EX/MEM.ALUZero:1 = 0
EX/MEM.writeDataMem:32 = 13 ($3 contains 13)
EX/MEM.writeRegister:5 = 4
EX/MEM.Branch:1 = 0
EX/MEM.MemWrite:1 = 0
EX/MEM.MemRead:1 = 0
EX/MEM.RegWrite:1 = 1
EX/MEM.MemtoReg:1 = 0

MEM/WB state at end of cycle 15:
Instruction (in assembly language) is : sll   $3,$2,1
Instruction (in hexadecimal) is : 0x00021840
MEM/WB.ALUResult:32 = 24
MEM/WB.readDataMem:32 = 0
MEM/WB.writeRegister:5 = 3
MEM/WB.RegWrite:1 = 1
MEM/WB.MemtoReg:1 = 0

WB state at end of cycle 15:
Instruction (in assembly language) is : sll   $2,$1,0
Instruction (in hexadecimal) is : 0x00011000
WB.ALUResult:32 = 11
WB.readDataMem:32 = 0
WB.writeRegister:5 = 2
WB.RegWrite:1 = 1
WB.MemtoReg:1 = 0

0000006C: Quitting

*/
