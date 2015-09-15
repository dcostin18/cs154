
/* 
 * CS:APP Data Lab 
 * 
 * <Denise Costin dcostin>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//#include "absVal.c"
//#include "addOK.c"
//#include "allEvenBits.c"
//#include "allOddBits.c"
//#include "anyEvenBit.c"


// int main()
// {
//   //rempwr2(15,2) = 3, rempwr2(-35,3) = -3
//   printf("%d\n", rempwr2(15,2));
// }

/* 
 * anyOddBit - return 1 if any odd-numbered bit in word set to 1
 *   Examples anyOddBit(0x5) = 0, anyOddBit(0x7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */

int anyOddBit(int x) {
  /* 0x55 is the hex representation for 01010101, which masks the even-numbered bits.
   * 1's are then anded to the odd bits, so if any odd bit is set to 1, the expression
   * will not be zero, and return false when double-negated.
  */
  int x0 = x & 0xFF;
  // printf("0: %d\n", x0);
  int x1 = (x >> 8) & 0xFF;
  // printf("1: %d\n", x1);
  int x2 = (x >> 16) & 0xFF;
  // printf("2: %d\n", x2);
  int x3 = (x >> 24) & 0xFF;
  // printf("3: %d\n", x3);
  int bit_mask = 0xAA;

  // mask out odd bits with 10101010, see if any still contain 1's
  return (!!(x0 & bit_mask) | !!(x1 & bit_mask) | !!(x2 & bit_mask) | !!(x3 & bit_mask));

}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
  // ~x + 1 ensures 1 will be 31st bit if x!=0
  // then simply shift 31st bit and & with 1 to get a single bit
  int neg = ~x + 1;
  return ((~x & ~neg) >> 31) & 1;
}
//#include "bitAnd.c"
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
  int count = x & 1;  x = x >> 1;
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;  
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;  
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;  
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;  
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;  
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;  
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;  
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;
  count = count + (x & 1); x = x >> 1;


  return count;

}
/* 
 * bitMask - Generate a mask consisting of all 1's 
 *   lowbit and highbit
 *   Examples: bitMask(5,3) = 0x38
 *   Assume 0 <= lowbit <= 31, and 0 <= highbit <= 31
 *   If lowbit > highbit, then mask should be all 0's
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int bitMask(int highbit, int lowbit) {
  // number of 1's in final result
  int sub = highbit + ~lowbit + 1;
  // produces as many 1's as needed
  int ones = (2 << sub) + ~1 + 1;
  // all 1's if highbit > lowbit, all 0's otherwise
  int mask = ~(sub >> 31);
  // shift ones over by lowbit, apply mask
  return ((ones << lowbit) & mask);

}
//#include "bitNor.c"
//#include "bitOr.c"
/*
 * bitParity - returns 1 if x contains an odd number of 0's
 *   Examples: bitParity(5) = 0, bitParity(7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */

int bitParity(int x) {
  int count = !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1; 
  count = count + !(x & 1); x = x >> 1; 
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1; 
  count = count + !(x & 1); x = x >> 1; 
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1;
  count = count + !(x & 1); x = x >> 1; 
  count = count + !(x & 1); x = x >> 1; 
  count = count + !(x & 1); x = x >> 1; 
 
  return (count & 1);
  
}
//#include "bitXor.c"
//#include "byteSwap.c"
//#include "conditional.c"
//#include "copyLSB.c"
//#include "divpwr2.c"
//#include "evenBits.c"
//#include "ezThreeFourths.c"
//#include "fitsBits.c"
//#include "fitsShort.c"
//#include "float_abs.c"
//#include "float_f2i.c"
//#include "float_half.c"
//#include "float_i2f.c"
//#include "float_neg.c"
//#include "float_twice.c"
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
  // front byte is x >> 8n
  int front = x >> (n << 3);
  // masks away all but first byte
  int mask = ~(~0 << 8);
  // returns first (only) byte
  return front & mask;
  // moves byte n to very right
  // int front = x >> (n << 3);
  // int masked = front & (0xFF);

  // // printf("Original: %d\n", x);
  // // int front = x >> (4 * n);
  // // printf("Shifted: %d\n", front);
  // // int mask1 = front ^ 0xF;
  // // printf("Mask1: %d\n", mask1);
  // // int mask2 = mask1 ^ 0xF;
  // // printf("Mask2: %d\n", mask2);
  // return masked;
}
//#include "greatestBitPos.c"
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  return 0;
}
//#include "ilog2.c"
//#include "implication.c"
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  // checks if second to last nibble = 0011
  int one = !((x >> 4) + ~3 + 1);
  // checks that last digit < 10 (0 to 9)
  int two = !!(((x & 0xf) + ~10+1) >> 31);
  //returns if both true
  return one & two;

}
/* 
 * isEqual - return 1 if x == y, and 0 otherwise 
 *   Examples: isEqual(5,5) = 1, isEqual(4,5) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int isEqual(int x, int y) {
  // res = 0 if and only if all bytes are equal, else is any integer
  int res = x ^ y;
  return !res;
}
/* 
 * isGreater - if x > y  then return 1, else return 0 
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isGreater(int x, int y) {
  // Idea: if x>0 and y<0, return 1. 
  // If sign(x) = sign(y) & (x-y)>0, return 1. Else return 0
  int a = x;
  int b = ~y + 1;
  int diff = a+b;
  int signdiff = (diff >> 31) & 1;
  int signx = (x >> 31) & 1;
  int signy = (y >> 31) & 1;

  // int samesign = signx == signy;
  int samesign = !(signx + ~signy+1);
  // return ((signx == 0 & signy == 1) | (samesign & (signdiff == 0)));
  return ((!(signx + ~0 + 1) & !(signy + ~1 + 1)) | (samesign & !(signdiff + ~0+ 1) & !!diff));

}
//#include "isLess.c"
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0.
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  // Same general approach as for isGreater.
  int a = ~x+1;
  int b = y;
  int diff = a+b;
  int signdiff = (diff >> 31) & 1;
  int signx = (x >> 31) & 1;
  int signy = (y >> 31) & 1;

  int samesign = !(signx + ~signy+1);
  return ((!(signy + ~0 + 1) & !(signx + ~1 + 1)) | (samesign & !(signdiff + ~0+ 1)));

}

// #include "isNegative.c"
/* 
 * isNonNegative - return 1 if x >= 0, return 0 otherwise 
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 3
 */
int isNonNegative(int x) {
  // int mask = 10000...0
  int mask = 0x1 << 31;
  // for x>=0, most significant bit is 0, so return 1. Else return 0.
  return !(mask & x);
}
//#include "isNonZero.c"
/* 
 * isNotEqual - return 0 if x == y, and 1 otherwise 
 *   Examples: isNotEqual(5,5) = 0, isNotEqual(4,5) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int isNotEqual(int x, int y) {
  // Applying bang to isEqual gives isNotEqual.
  int res = x ^ y;
  return !!res;
}
//#include "isPositive.c"
//#include "isPower2.c"
//#include "isTmax.c"
//#include "isTmin.c"
//#include "isZero.c"
//#include "leastBitPos.c"
/*
 * leftBitCount - returns count of number of consective 1's in
 *     left-hand (most significant) end of word.
 *   Examples: leftBitCount(-1) = 32, leftBitCount(0xFFF0F0F0) = 12
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 50
 *   Rating: 4
 */
int leftBitCount(int x) {
  return 0;
}
//#include "logicalNeg.c"
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
  // mask_blur masks out the "smeared bits" of x>>n (form: 000...11111)
  int mask_blur = ~((!!n<<31)>>(n + ~1 + 1));
  // Shift over, then mask blur and return.
  int logical_shift = (x >> n) & mask_blur;

  return logical_shift;

}
//#include "minusOne.c"
//#include "multFiveEighths.c"
//#include "negate.c"
//#include "oddBits.c"
/* 
 * rempwr2 - Compute x%(2^n), for 0 <= n <= 30
 *   Negative arguments should yield negative remainders
 *   Examples: rempwr2(15,2) = 3, rempwr2(-35,3) = -3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int rempwr2(int x, int n) {
  // Idea: removing 2^n = taking least significant n bytes for x>0
  int negx = ~x + 1;
  int signx = x >> 31;
  // Compute abs(x)
  int absvx = (~signx & x) | (signx & negx);

  // Take least significant n bytes for abs(x)
  int mod = (~(~0<<n)) & absvx;
  int negmod = ~mod + 1;
  // Return negated least sig. n bytes if x was negative
  // Else simply return least sig. n bytes
  return ((~signx & mod) | (signx & negmod));


}
/* 
 * replaceByte(x,n,c) - Replace byte n in x with c
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: replaceByte(0x12345678,1,0xab) = 0x1234ab78
 *   You can assume 0 <= n <= 3 and 0 <= c <= 255
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 3
 */
int replaceByte(int x, int n, int c) {
  // Mask current byte n
  int bitmask = ~(0xFF << (n << 3));
  // Shift c over to byte n, or it with masked x
  return (x & bitmask) | (c << (n << 3));
}
//#include "reverseBytes.c"
//#include "rotateLeft.c"
/* 
 * rotateRight - Rotate x to the right by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateRight(0x87654321,4) = 0x76543218
 *   Legal ops: ~ & ^ | + << >> !
 *   Max ops: 25
 *   Rating: 3 
 */
int rotateRight(int x, int n) {
  // Mask out current most significant n bytes so they can be replaced by tail n bytes
  int mask_blur = ~((!!n<<31)>>(n + ~1 + 1));
  // Perform logical right shift by n on x
  int logical_shift = (x >> n) & mask_blur;

  // Shift over least significant n bytes and add to front of logically shifted x
  int looped_bits = (x << (33 + ~n)) & ((!!n << 31) >> 31);
  return (logical_shift + looped_bits);


}
//#include "satAdd.c"
//#include "satMul2.c"
/*
 * satMul3 - multiplies by 3, saturating to Tmin or Tmax if overflow
 *  Examples: satMul3(0x10000000) = 0x30000000
 *            satMul3(0x30000000) = 0x7FFFFFFF (Saturate to TMax)
 *            satMul3(0x70000000) = 0x7FFFFFFF (Saturate to TMax)
 *            satMul3(0xD0000000) = 0x80000000 (Saturate to TMin)
 *            satMul3(0xA0000000) = 0x80000000 (Saturate to TMin)
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 3
 */
int satMul3(int x)
{
  // Interpret x*3 as (x*2) + x = y + x
  int y = x << 1;
  int signx = (x >> 31) & 1;
  int signy = (y >> 31) & 1;
  int am = x + y;
  int signam = (am >> 31) & 1;
  int mask = ~(1 << 31);

  // if(signx == 0 && signy == 1) return 0x7FFFFFFF;
  int cond1 = (~(!signx & signy) + 1) & mask;
  // else if(signx == 1 && signy == 0) return 0x80000000;
  int cond2 = (!signy & signx) << 31;
  // else if(signx == 0 && signy == 0 && signam == 1) return 0x7FFFFFFF;
  int cond3 = (~(!signx & !signy & signam) + 1) & mask;
  // else if((signx == signy) && (signx == signam)) return am;
  int cond4 = (~(!(signx + ~signy + 1) & !(signx + ~signam + 1)) + 1) & am;
  // else if(signx == 1 && signy == 1 && signam == 0) return 0x80000000;
  int cond5 = (signx & signy & !signam) << 31;

  return (cond1 | cond2 | cond3 | cond4 | cond5);
}

//#include "sign.c"
//#include "sm2tc.c"
/* 
 * subOK - Determine if can compute x-y without overflow
 *   Example: subOK(0x80000000,0x80000000) = 1,
 *            subOK(0x80000000,0x70000000) = 0, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int subOK(int x, int y) {

  int a = x;
  int b = ~y + 1;
  int sum = a + b;

  int signsum = (sum >> 31) & 1;

  return !!(!(a + ~b + 1) & !!(a + ~signsum + 1));



}
//#include "tc2sm.c"
//#include "thirdBits.c"
//#include "tmax.c"
//#include "tmin.c"
//#include "trueFiveEighths.c"
//#include "trueThreeFourths.c"
//#include "upperBits.c"