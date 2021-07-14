/* number.c: Implements arbitrary precision numbers. */  
  
/*  This file is part of GNU bc.
    Copyright (C) 1991, 1992, 1993, 1994, 1997 Free Software Foundation, Inc.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or 
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, write to
    the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

    You may contact the author by:
       e-mail:  phil@cs.wwu.edu
      us-mail:  Philip A. Nelson
                Computer Science Department, 9062
                Western Washington University
                Bellingham, WA 98226-9062

*************************************************************************/ 
  
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "odbc_number.h"
  
/* The base used in storing the numbers in n_value above.
   Currently this MUST be 10. */ 
  
#define BASE 10
  
/*  Some useful macros and constants. */ 
  
#define CH_VAL(c)     (c - '0')
#define BCD_CHAR(d)   (d + '0')
  
#ifdef MIN
#undef MIN
#undef MAX
#endif /* 
#define MAX(a,b)      ((a)>(b)?(a):(b))
#define MIN(a,b)      ((a)>(b)?(b):(a))
#define ODD(a)        ((a)&1)
  
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif /* 













/* Storage used for special numbers. */ 
static bc_num _zero_ = NULL;



/* The reference string for digits. */ 
  
/* Intitialize the number package! */ 

init_numbers () 
{
  
    
    {
      
      
	
    
  
    
    {
      
      
	
      
    
  
    
    {
      
      
	
      
    
  



/* Here is the full add routine that takes care of negative numbers.
   N1 is added to N2 and the result placed into RESULT.  SCALE_MIN
   is the minimum scale for the result. */ 

bc_add (n1, n2, result, scale_min) 
bc_num n1, n2, *result;
     

{
  
  
  
  
    
    {
      
      
	
      
    
  
  else
    
    {
      
	/* subtraction must be done. */ 
	cmp_res = _do_compare (n1, n2, FALSE, FALSE);	/* Compare magnitudes. */
      
	
	{
	
	  
	    /* n1 is less than n2, subtract n1 from n2. */ 
	    sum = _do_sub (n2, n1, scale_min);
	  
	    
	  
	  
	
	  
	    /* They are equal! return zero with the correct scale! */ 
	    res_scale = MAX (scale_min, MAX (n1->n_scale, n2->n_scale));
	  
	  
	    
	  
	  
	
	  
	    /* n2 is less than n1, subtract n2 from n1. */ 
	    sum = _do_sub (n1, n2, scale_min);
	  
	    
	  
	
    
  
    /* Clean up and return. */ 
    free_num (result);
  
  



/* Here is the full subtract routine that takes care of negative numbers.
   N2 is subtracted from N1 and the result placed in RESULT.  SCALE_MIN
   is the minimum scale for the result. */ 

bc_sub (n1, n2, result, scale_min) 
bc_num n1, n2, *result;
     

{
  
  
  
  
    
    {
      
      
	
      
    
  
  else
    
    {
      
	/* subtraction must be done. */ 
	cmp_res = _do_compare (n1, n2, FALSE, FALSE);	/* Compare magnitudes. */
      
	
	{
	
	  
	    /* n1 is less than n2, subtract n1 from n2. */ 
	    diff = _do_sub (n2, n1, scale_min);
	  
	    
	  
	  
	
	  
	    /* They are equal! return zero! */ 
	    res_scale = MAX (scale_min, MAX (n1->n_scale, n2->n_scale));
	  
	  
	    
	  
	  
	
	  
	    /* n2 is less than n1, subtract n2 from n1. */ 
	    diff = _do_sub (n1, n2, scale_min);
	  
	    
	  
	  
	
    
  
    /* Clean up and return. */ 
    free_num (result);
  
  



/* The multiply routine.  N2 time N1 is put int PROD with the scale of
   the result being MIN(N2 scale+N1 scale, MAX (SCALE, N2 scale, N1 scale)).
   */ 

bc_multiply (n1, n2, prod, scale) 
bc_num n1, n2, *prod;
     

{
  
  
  
  
  
  
  
  
  
    /* Initialize things. */ 
    len1 = n1->n_len + n1->n_scale;
  
  
  
  
  
  
  
    
  
  
  
  
  
  
    /* Here are the loops... */ 
    for (indx = 0; indx < toss; indx++)
    
    {
      
      
      
	
      
    
  
    
    {
      
      
      
	
      
      
    
  
  
    /* Assign to prod and clean up the number. */ 
    free_num (prod);
  
  
  
    
  



/* The full division routine. This computes N1 / N2.  It returns
   0 if the division is ok and the result is in QUOT.  The number of
   digits after the decimal point is SCALE. It returns -1 if division
   by zero is tried.  The algorithm is found in Knuth Vol 2. p237. */ 

bc_divide (n1, n2, quot, scale) 
bc_num n1, n2, *quot;
     

{
  
  
  
  
  
  
  
  
  
  
    /* Test for divide by zero. */ 
    if (is_zero (n2))
    
  
    /* Test for divide by 1.  If it is we must truncate. */ 
    if (n2->n_scale == 0)
    
    {
      
	
	{
	  
	  
	    
	  
	  
	  
	  
	  
	
    
  
    /* Set up the divide.  Move the decimal point on n1 by n2's scale.
     * effort for dividing. */ 
    scale2 = n2->n_scale;
  
  
    
  
  
  
    
  
  else
    
  
  
    
  
  
  
  
  
    
  
  
  
  
    
    {
      
      
    
  
    /* Calculate the number of quotient digits. */ 
    if (len2 > len1 + scale)
    
    {
      
      
    
  
  else
    
    {
      
      
	
      
      else
	
    
  
    /* Allocate and zero the storage for the quotient. */ 
    qval = new_num (qdigits - scale, scale);
  
    
  
  
    /* Allocate storage for the temporary storage mval. */ 
    mval = (unsigned char *) malloc (len2 + 1);
  
    
  
    /* Now for the full divide algorithm. */ 
    if (!zero)
    
    {
      
	/* Normalize */ 
	norm = 10 / ((int) *n2ptr + 1);
      
	
	{
	  
	  
	
      
	/* Initialize divide loop. */ 
	qdig = 0;
      
	
      
      else
	
      
	/* Loop */ 
	while (qdig <= len1 + scale - len2)
	
	{
	  
	    /* Calculate the quotient digit guess. */ 
	    if (*n2ptr == num1[qdig])
	    
	  
	  else
	    
	  
	    /* Test qguess. */ 
	    if (n2ptr[1] * qguess > 
	    
	    {
	      
	      
		/* And again. */ 
		if (n2ptr[1] * qguess > 
		
	    
	  
	    /* Multiply and subtract. */ 
	    borrow = 0;
	  
	    
	    {
	      
	      
	      
	      
	      
		
		{
		  
		  
		    
		    {
		      
		      
		    
		  
		  else
		    
		  
		
	    
	  
	    /* Test for negative result. */ 
	    if (borrow == 1)
	    
	    {
	      
	      
	      
	      
	      
		
		{
		  
		  
		    
		    {
		      
		      
		    
		  
		  else
		    
		  
		
	      
		
	    
	  
	    /* We now know the quotient digit. */ 
	    *qptr++ = qguess;
	  
    
  
    /* Clean up and return the number. */ 
    qval->n_sign = (n1->n_sign == n2->n_sign ? PLUS : MINUS);
  
    
  
  
  
  
    /* Clean up temporary storage. */ 
    free (mval);
  
  
  



/* Division *and* modulo for numbers.  This computes both NUM1 / NUM2 and
   NUM1 % NUM2  and puts the results in QUOT and REM, except that if QUOT
   is NULL then that store will be omitted.
 */ 

bc_divmod (num1, num2, quot, rem, scale) 
bc_num num1, num2, *quot, *rem;
     

{
  
  
  
  
    /* Check for correct numbers. */ 
    if (is_zero (num2))
    
  
    /* Calculate final scale. */ 
    rscale = MAX (num1->n_scale, num2->n_scale + scale);
  
  
    /* Calculate it. */ 
    bc_divide (num1, num2, &temp, scale);
  
    
  
  
  
  
    
    {
      
      
    
  



/* "Frees" a bc_num NUM.  Actually decreases reference count and only
   frees the storage if reference count is zero. */ 

free_num (num) 
bc_num * num;

{
  
    
  
  
    
  



/* Convert strings to bc numbers.  Base 10 only.*/ 

str2num (num, str, scale) 
bc_num * num;
     
     

{
  
  
  
  
    /* Prepare num. */ 
    free_num (num);
  
    /* Check for valid number and count digits. */ 
    ptr = str;
  
  
  
  
    
  
    
    {				/* Skip leading zeros */
      
      
	
	{
	  
	  
	
    
  
    
  
    
  
    
  
    
    {
      
      
    
  
    /* Adjust numbers and allocate storage and initialize fields. */ 
    strscale = MIN (strscale, scale);
  
    
    {
      
      
    
  
  
    
  
    /* Build the whole number. */ 
    ptr = str;
  
    
    {
      
      
    
  
  else
    
    {
      
      
	
    
  
    
    {				/* Skip leading zeros. */
      
      
	
	{
	  
	  
	
    
  
  
    
    {
      
      
    
  
    
  
    /* Build the fractional part. */ 
    if (strscale > 0)
    
    {
      
      
	
    
  



/* Convert a numbers to a string.  Base 10 only.*/ 

num2str (num) 
bc_num num;

{
  
  
  
  
    /* Allocate the string memory. */ 
    signch = (num->n_sign == PLUS ? 0 : 1);	/* Number of sign chars. */
  
    
  
  else
    
  
    
  
    /* The negative sign if needed. */ 
    sptr = str;
  
    
  
    /* Load the whole number. */ 
    nptr = num->n_value;
  
    
  
    /* Now the fraction. */ 
    if (num->n_scale > 0)
    
    {
      
      
	
    
  
    /* Terminate the string and return it! */ 
    *sptr = '\0';
  



/* new_num allocates a number and sets fields to known values. */ 

     new_num (length, scale) 
     length, scale;

{
  
  
  
    
  
  
  
  
  
  



/* Make a copy of a number!  Just increments the reference count! */ 

copy_num (num) 
bc_num num;

{
  
  



/* Initialize a number NUM by making it a copy of zero. */ 

init_num (num) 
bc_num * num;

{
  


/* Compare two bc numbers.  Return value is 0 if equal, -1 if N1 is less
   than N2 and +1 if N1 is greater than N2.  If USE_SIGN is false, just
   compare the magnitudes. */ 

_do_compare (n1, n2, use_sign, ignore_last) 
bc_num n1, n2;
     
     

{
  
  
  
    /* First, compare signs. */ 
    if (use_sign && n1->n_sign != n2->n_sign)
    
    {
      
	
      
      else
	
    
  
    /* Now compare the magnitude. */ 
    if (n1->n_len != n2->n_len)
    
    {
      
	
	{
	  
	    /* Magnitude of n1 > n2. */ 
	    if (!use_sign || n1->n_sign == PLUS)
	    
	  
	  else
	    
	
      
      else
	
	{
	  
	    /* Magnitude of n1 < n2. */ 
	    if (!use_sign || n1->n_sign == PLUS)
	    
	  
	  else
	    
	
    
  
    /* If we get here, they have the same number of integer digits.
     * of the fraction. */ 
    count = n1->n_len + MIN (n1->n_scale, n2->n_scale);
  
  
  
    
    {
      
      
      
    
  
    
  
    
    {
      
	
	{
	  
	    /* Magnitude of n1 > n2. */ 
	    if (!use_sign || n1->n_sign == PLUS)
	    
	  
	  else
	    
	
      
      else
	
	{
	  
	    /* Magnitude of n1 < n2. */ 
	    if (!use_sign || n1->n_sign == PLUS)
	    
	  
	  else
	    
	
    
  
    /* They are equal up to the last part of the equal part of the fraction. */ 
    if (n1->n_scale != n2->n_scale)
    
      
      {
	
	  
	    
	    {
	      
		/* Magnitude of n1 > n2. */ 
		if (!use_sign || n1->n_sign == PLUS)
		
	      
	      else
		
	    
      
  
    else
      
      {
	
	  
	    
	    {
	      
		/* Magnitude of n1 < n2. */ 
		if (!use_sign || n1->n_sign == PLUS)
		
	      
	      else
		
	    
      
  
    /* They must be equal! */ 
    return (0);



/* This is the "user callable" routine to compare numbers N1 and N2. */ 

bc_compare (n1, n2) 
bc_num n1, n2;

{
  



/* In some places we need to check if the number NUM is zero. */ 

is_zero (num) 
bc_num num;

{
  
  
  
    /* Quick check. */ 
    if (num == _zero_)
    
  
    /* Initialize */ 
    count = num->n_len + num->n_scale;
  
  
    /* The check */ 
    while ((count > 0) && (*nptr++ == 0))
    
  
    
  
  else
    



/* In some places we need to check if the number is negative. */ 

is_neg (num) 
bc_num num;

{
  



/* For many things, we may have leading zeros in a number NUM.
   _rm_leading_zeros just moves the data to the correct
   place and adjusts the length. */ 

_rm_leading_zeros (num) 
bc_num num;

{
  
  
  
    /* Do a quick check to see if we need to do it. */ 
    if (*num->n_value != 0)
    
  
    /* The first "digit" is 0, find the first non-zero digit in the second
     * decimal place. */ 
    bytes = num->n_len;
  
  
    
  
  
  
  
    



/* Perform addition: N1 is added to N2 and the value is
   returned.  The signs of N1 and N2 are ignored.
   SCALE_MIN is to set the minimum scale of the result. */ 

_do_add (n1, n2, scale_min) 
bc_num n1, n2;
     

{
  
  
  
  
  
  
    /* Prepare sum. */ 
    sum_scale = MAX (n1->n_scale, n2->n_scale);
  
  
  
    
  
    /* Zero extra digits made by scale_min. */ 
    if (scale_min > sum_scale)
    
    {
      
      
	
    
  
    /* Start with the fraction part.  Initialize the pointers. */ 
    n1bytes = n1->n_scale;
  
  
  
  
  
    /* Add the fraction part.  First copy the longer fraction. */ 
    if (n1bytes != n2bytes)
    
    {
      
	
	  
	  {
	    
	    
	  
      
      else
	
	  
	  {
	    
	    
	  
    
  
    /* Now add the remaining fraction part and equal size integer parts. */ 
    n1bytes += n1->n_len;
  
  
  
    
    {
      
      
	
	{
	  
	  
	
      
      else
	
      
      
      
    
  
    /* Now add carry the longer integer part. */ 
    if (n1bytes == 0)
    
    {
      
      
    
  
    
    {
      
      
	
	{
	  
	  
	
      
      else
	
      
    
  
    /* Set final carry. */ 
    if (carry == 1)
    
  
    /* Adjust sum and return. */ 
    _rm_leading_zeros (sum);
  



/* Perform subtraction: N2 is subtracted from N1 and the value is
   returned.  The signs of N1 and N2 are ignored.  Also, N1 is
   assumed to be larger than N2.  SCALE_MIN is the minimum scale
   of the result. */ 

_do_sub (n1, n2, scale_min) 
bc_num n1, n2;
     

{
  
  
  
  
  
  
    /* Allocate temporary storage. */ 
    diff_len = MAX (n1->n_len, n2->n_len);
  
  
  
  
  
    
  
    /* Zero extra digits made by scale_min. */ 
    if (scale_min > diff_scale)
    
    {
      
      
	
    
  
    /* Initialize the subtract. */ 
    n1ptr = (char *) (n1->n_value + n1->n_len + n1->n_scale - 1);
  
  
  
    /* Subtract the numbers. */ 
    borrow = 0;
  
    /* Take care of the longer scaled number. */ 
    if (n1->n_scale != min_scale)
    
    {
      
	/* n1 has the longer scale */ 
	for (count = n1->n_scale - min_scale; count > 0; count--)
	
    
  
  else
    
    {
      
	/* n2 has the longer scale */ 
	for (count = n2->n_scale - min_scale; count > 0; count--)
	
	{
	  
	  
	    
	    {
	      
	      
	    
	  
	  else
	    
	  
	
    
  
    /* Now do the equal length scale and integer parts. */ 
    
    
    {
      
      
	
	{
	  
	  
	
      
      else
	
      
    
  
    /* If n1 has more digits then n2, we now do that subtract. */ 
    if (diff_len != min_len)
    
    {
      
	
	{
	  
	  
	    
	    {
	      
	      
	    
	  
	  else
	    
	  
	
    
  
    /* Clean up and return. */ 
    _rm_leading_zeros (diff);
  



/* Some utility routines for the divide:  First a one digit multiply.
   NUM (with SIZE digits) is multiplied by DIGIT and the result is
   placed into RESULT.  It is written so that NUM and RESULT can be
   the same pointers.  */ 

     _one_mult (num, size, digit, result) 
       num;
     
     

{
  
  
  
    
  
  else
    
    {
      
	
      
      else
	
	{
	  
	    /* Initialize */ 
	    nptr = (unsigned char *) (num + size - 1);
	  
	  
	  
	    
	    {
	      
	      
	      
	    
	  
	    
	
    



/* Modulo for numbers.  This computes NUM1 % NUM2  and puts the
   result in RESULT.   */ 

bc_modulo (num1, num2, result, scale) 
bc_num num1, num2, *result;
     

{
  



/* In some places we need to check if the number NUM is zero. */ 

is_near_zero (num, scale) 
bc_num num;
     

{
  
  
  
    /* Initialize */ 
    count = num->n_len + scale;
  
  
    /* The check */ 
    while ((count > 0) && (*nptr++ == 0))
    
  
    
  
  else
    



/* The following routines provide output for bcd numbers package
   using the rules of POSIX bc for output. */ 
  
/* This structure is used for saving digits in the conversion process. */ 
typedef struct stk_rec 
{
  
   
 
