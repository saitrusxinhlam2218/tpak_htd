static char sz__FILE__[]="@(#)set_bit.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  set_bit.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:49:35
 * @(#)  Last delta: 12/2/94 at 18:22:16
 * @(#)  SCCS File: /taxi/sccs/s.set_bit.c
 *                                                                           *
 *****************************************************************************/

#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"

extern struct cisam_cl call_struct;
	
/* test_bit: 	tests whether a bot defined by bit_num is set to1 or 0. Returns 1 or 0. 
 *		The least significant bit (the rightmost bit is numbered 0. 
 */		
int test_bit(bit_num,size,struct_ptr)
	int bit_num;	/* # of the bit to be tested; leaset significant = 0 */
	int size;	/* size of bit struct in bytes */
	unsigned char *struct_ptr;	/* pointer to the bit struct */
{
	unsigned char *bit_ptr;	/* poinetr to bit struct */
	unsigned char mask;
	int shift_by;	/* shift the mask by */

	bit_ptr = (unsigned char *)struct_ptr + bit_num/8;
	mask = 0x80;	/* start from bit 0 */
	shift_by = bit_num % 8;
	mask >>= shift_by;

	if ( *bit_ptr & mask )
		/* bit is set */
		return(1);
	else 
		/* bit is not set */
		return(0);

}  /* end test_bit() */


/* set_bit: sets the bit specified by bit_num to 1. The size of the bit structure and its address 
 *		should be passed as arguments
 */
set_bit(bit_num,size,struct_ptr)
	int bit_num;	/* # of the bit to be tested; leaset significant = 0 */
	int size;	/* size of bit struct in bytes */
	unsigned char *struct_ptr;	/* pointer to the bit struct */
{
	unsigned char *bit_ptr;	/* poinetr to bit struct */
	unsigned char mask;
	int shift_by;	/* shift the mask by */

	mask = 0x80;
	bit_ptr = (unsigned char *)struct_ptr + bit_num/8;
	shift_by = bit_num % 8;
	mask >>= shift_by;

	*bit_ptr = *bit_ptr | mask;

}  /* end set_bit() */

/* set_call_bit: sets the bit # passed as argument in the status_bits field of call_struct */
set_call_bit(bit_num)
	int bit_num;	/* # of the bit to be set */
{
	unsigned char mask;
	int shift_by;	/* shift the mask by */
	int byte_to_use;	/* # of the byte to be used in call_struct.status[] */

	mask = 0x80;	/* start from bit 0 */
	shift_by = bit_num % 8;
	mask = mask >> shift_by;
	byte_to_use = bit_num/8;

	/* set the appropriate bit */
	call_struct.status_bits[byte_to_use] |= mask;

}   /* end set_call_bit() */


/* unset_call_bit: unsets the bit # passed as argument in the status_bits field of call_struct */
/* WARNING: The following code along with all bitwise operations should be considered non-portable,
 	    platform this code is based on is ULTRIX(RISC) */

unset_call_bit(bit_num)
int bit_num;				/* # of the bit to be unset */
{
	unsigned char mask;
	int shift_by;			/* shift the mask by */
	int byte_to_use;		/* # of the byte to be used in call_struct.status[] */

	mask = 0x80;			/* start with 00000001 */
	shift_by = bit_num % 8; 	/* Find bit # */

	mask >>= shift_by;
	mask = ~mask;

	byte_to_use = bit_num / 8;

	/* unset the appropriate bit */
	call_struct.status_bits[byte_to_use] &= mask;
}

	