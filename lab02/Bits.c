// ADT for Bit-strings
// COMP1521 17s2 Week02 Lab Exercise
// Written by John Shepherd, July 2017
// Modified by ...

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "Bits.h"

// assumes that an unsigned int is 32 bits
#define BITS_PER_WORD 32
#define NULL_CHAR 1
#define WILL_OVERFLOW 1
#define WONT_OVERFLOW 0
#define INT_MAX 2147483647
#define UINT_MAX 4294967295U
#define LONG_MAX 9223372036854775807
#define ULONG_MAX 18446744073709551615U
#define BITS_PER_HEX 4

// A bit-string is an array of unsigned ints (each a 32-bit Word)
// The number of bits (hence Words) is determined at creation time
// Words are indexed from right-to-left
// words[0] contains the most significant bits
// words[nwords-1] contains the least significant bits
// Within each Word, bits are indexed right-to-left
// Bit position 0 in the Word is the least significant bit
// Bit position 31 in the Word is the most significant bit

typedef unsigned int Word;

struct BitsRep {
   int nwords;   // # of Words
   Word *words;  // array of Words
};

// make a new empty Bits with space for at least nbits
// rounds up to nearest multiple of BITS_PER_WORD
Bits makeBits(int nbits)
{
   Bits new;
   new = malloc(sizeof(struct BitsRep));
   assert(new != NULL);
   int  nwords;
   if (nbits%BITS_PER_WORD == 0)
      nwords = nbits/BITS_PER_WORD;
   else
      nwords = 1+nbits/BITS_PER_WORD;
   new->nwords = nwords;
   // calloc sets to all 0's
   new->words = calloc(nwords, sizeof(Word));
   assert(new->words != NULL);
   return new;
}

// release space used by Bits
void  freeBits(Bits b)
{
   assert(b != NULL && b->words != NULL);
   free(b->words);
   free(b);
}

// form bit-wise AND of two Bits a,b
// store result in res Bits
void andBits(Bits a, Bits b, Bits res)
{
   for (int w_index = 0; w_index < a->nwords; w_index++)
   {
      res->words[w_index] = a->words[w_index] & b->words[w_index];
   }
}

// form bit-wise OR of two Bits a,b
// store result in res Bits
void orBits(Bits a, Bits b, Bits res)
{
   for (int w_index = 0; w_index < a->nwords; w_index++)
   {
      res->words[w_index] = a->words[w_index] | b->words[w_index];
   }
}

// form bit-wise negation of Bits a,b
// store result in res Bits
void invertBits(Bits a, Bits res)
{
   for (int w_index = 0; w_index < a->nwords; w_index++)
   {
      res->words[w_index] = ~ a->words[w_index];
   }
}

// left shift Bits
void leftShiftBits(Bits b, int shift, Bits res)
{  
   //***hacky solution - use memset to intialise a var sized array to store bit pattern in***
   int total_bits = b->nwords * BITS_PER_WORD;
   char full_bit_pattern[total_bits + NULL_CHAR];
   full_bit_pattern[total_bits] = '\0';
   memset(full_bit_pattern, 0, sizeof(full_bit_pattern));

   get_entire_bit_pattern(b, full_bit_pattern);

   //shift bit pattern to the left
   for (int total_bits_index = 0; total_bits_index < total_bits; total_bits_index++)
   {
      if (total_bits_index + shift < total_bits)
      {
         full_bit_pattern[total_bits_index] = full_bit_pattern[total_bits_index + shift];
      }
      else
      {
         full_bit_pattern[total_bits_index] = '0';
      } 
   }

   //evaluate dec number from each string of 32 bits and store in separate words
   setBitsFromString(res, full_bit_pattern);
}

// right shift Bits
void rightShiftBits(Bits b, int shift, Bits res)
{
   int total_bits = b->nwords * BITS_PER_WORD;
   char full_bit_pattern[total_bits + NULL_CHAR];
   full_bit_pattern[total_bits] = '\0';
   memset(full_bit_pattern, 0, sizeof(full_bit_pattern));

   get_entire_bit_pattern(b, full_bit_pattern);

   //shift bit pattern to the right //FIX THIS LOGIC OVERWRITING FIRST BINARY DIGIT WITH A 0
   for (int total_bits_index = total_bits - 1; total_bits_index >= 0; total_bits_index--)
   {
      if (total_bits_index - shift >= 0)
      {
         full_bit_pattern[total_bits_index] = full_bit_pattern[total_bits_index - shift];
      }
      else
      {
         full_bit_pattern[total_bits_index] = '0';
      } 
   }

   //evaluate dec number from each string of 32 bits and store in separate words
   setBitsFromString(res, full_bit_pattern); //tried this but ERROR: ptrace: operation not permitted
}

// copy value from one Bits object to another
void setBitsFromBits(Bits from, Bits to)
{
   for (int w_index = 0; w_index < from->nwords; w_index++)
   {
      to->words[w_index] = from->words[w_index];
   }
}

// assign a bit-string (sequence of 0's and 1's) to Bits
// if the bit-string is longer than the size of Bits, truncate higher-order bits
void setBitsFromString(Bits b, char *bitseq)
{  
   int w_count;
   for (w_count = 0; w_count < b->nwords; w_count++)
      b->words[w_count] = 0;

   int bit_shift = 0;
   int b_index = strlen(bitseq) - 1;
   for (w_count = b->nwords - 1; w_count >= 0; w_count--)
   {
      for (; b_index >= 0; b_index--)
      {
         if (bit_shift == BITS_PER_WORD)
         {
            bit_shift = 0;
            break;
         }
         if (bitseq[b_index] == '1')
         {
            b->words[w_count] |= 1u<<bit_shift;
         }
         bit_shift++;
      }
      
   }
}

void showBits(Bits b)
{
   for(int word_count = 0; word_count < b->nwords; word_count++)
   {
      for(int bit_count = 0; bit_count < BITS_PER_WORD; bit_count++)
      {
         if ((b->words[word_count] >> (BITS_PER_WORD - 1 - bit_count)) & 1)
         {
            printf("1");
         }
         else
         {
            printf("0");
         }
      }
   }
}

void showDecimal(Bits b)
{
   int overflow = WONT_OVERFLOW;
   if (b->nwords > 2)
   {
      int higher_bit_words = b->nwords - 2;
      for (int w_index = 0; w_index < higher_bit_words; w_index++)
      {
         if (b->words[w_index] != 0)
         {
            overflow = WILL_OVERFLOW;
            printf("trying to compute decimal value of binary string ");
            showBits(b);
            printf(" will cause overflow");
            break;
         }
      }
   }

   if (overflow == WONT_OVERFLOW)
   {
      int power = 0;
      double dec_value = 0;

      for (int w_count = b->nwords - 1; w_count >= 0; w_count--)
      {
         for (int b_count = 0; b_count < BITS_PER_WORD; b_count++)
         {
            if (b->words[w_count] >> b_count & 1)
            {
               dec_value += pow(2, power);
            }
            power++;
         }
      }
      printf("%.0lf", dec_value);
      if (dec_value == INT_MAX)
      {
         printf(" (INT_MAX)");
      }
      else if (dec_value == UINT_MAX)
      {
         printf(" (UNSIGNED_INT_MAX)");
      }
      else if (dec_value == LONG_MAX)
      {
         printf(" (LONG_MAX)");
      }
      else if (dec_value == ULONG_MAX) //this is slightly buggy as the double is only precise to the power of 53, afterwards it rounds to ULONG_MAX
      {
         printf(" (UNSIGNED LONG_MAX)");
      }
   }
}

void showHex(Bits b)
{
   printf("0x");
   for (int w_count = 0; w_count < b->nwords; w_count++)
   {
      long int hex_value = b->words[w_count];
      if (hex_value != 0 || w_count == b->nwords - 1)
         printf("%lX", hex_value);
   }
}

void showOct(Bits b)
{
   printf("o");
   for (int w_count = 0; w_count < b->nwords; w_count++)
   {
      long int octal_value = b->words[w_count];
      if (octal_value != 0 || w_count == b->nwords - 1)
         printf("%lo", octal_value);
   }
}

void get_entire_bit_pattern(Bits b, char *full_bit_pattern) 
{
   int arr_index = 0;// w_index = 0;

   for(int word_count = 0; word_count < b->nwords; word_count++)
   {
      for(int bit_count = 0; bit_count < BITS_PER_WORD; bit_count++)
      {
         if ((b->words[word_count] >> (BITS_PER_WORD - 1 - bit_count)) & 1)
         {
            full_bit_pattern[arr_index] = '1';
         }
         else
         {
            full_bit_pattern[arr_index] = '0';
         }
         arr_index++;
      }
   }
}

/* MATHEMATICAL APPROACH TO SHOWBITS
// display a Bits value as sequence of 0's and 1's
void showBits(Bits b)
{
   assert(b != NULL);

   unsigned int current_value = 0, binary_value = 0;

   for (int w_index = 0; w_index < b->nwords; w_index++) //word index
   {
      int power = 31;
      current_value = b->words[w_index];

      for (int count = 0; count < BITS_PER_WORD; count++) 
      {
         binary_value = pow(2, power);

         if (binary_value <= current_value)
         {
            printf("1");
            current_value -= binary_value;
         } 
         else
         {
            printf("0");
         }

         power--;
      }
   }
}
*/

/* SETTING BITS FROM STRING - MATHEMATICAL APPROACH
   int len = strlen(bitseq), seq_index = (len - 1), seq_counter = 0;
   int word_index = (b->nwords - 1), power = 0;
   unsigned int dec_value = 0;

   //set word values back to 0 to reset binary all back to 0
   for (int w_count = 0; w_count < b->nwords; w_count++)
      b->words[w_count] = 0;

   //loop through words
   while (word_index >= 0)
   {
      //loop through bit sequence
      while (seq_index >= 0 && power < BITS_PER_WORD)
      {
         //binary_value = bitseq[seq_index] - '0';
         if (bitseq[seq_index] == '1')
         {
            dec_value += pow(2, power); //binary is base 2
         }

         if (seq_index == 0 || power == BITS_PER_WORD - 1) //if you go through 32 binary digits (or exhaust a string shorter than 32 bits) then save value in word and move onto next word
         {
            b->words[word_index] = dec_value;
         }

         power++;
         seq_index--;
         seq_counter++;
      }

      word_index--;
      dec_value = 0; 
      power = 0;
   }
   */
  
  /*
      MATHEMATICAL APPROACH - CALCULATING HEX BY HAND TO UNDERSTAND CONCEPT
      int power = BITS_PER_HEX - 1;
      long int hex_value = 0;

      for (int w_count = 0; w_count < b->nwords; w_count++)
      {
         for (int b_count = BITS_PER_WORD - 1; b_count >= 0; b_count--)
         {
            if (power < 0)
            {
               printf("%lX", hex_value);
               power = BITS_PER_HEX - 1;
               hex_value = 0;
            }

            if (b->words[w_count] >> b_count & 1)
            {
               hex_value += pow(2, power);
            }
            power--;
         }
      }
      */