/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: mem_mgmt.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/10/14 15:22:14 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/mem_mgmt.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#define	MM_PID_MASK	0x0000FFFF
#define	MM_CMD_MASK	0xFFFF0000
#define	MM_LOG_ON	0x00010000
#define	MM_LOG_OFF	0x00000000
#define	TCALLOC(type, num)		(type *) mm_calloc(num, sizeof(type), __FILE__, __LINE__)
#define	TFREE(ptr)			mm_free(ptr, __FILE__, __LINE__)
#define	TPKTIDFREE(ptr)			mm_pktid_free(ptr, __FILE__, __LINE__)
#define	TMALLOC(type, num)		(type *) mm_malloc(sizeof(type) * num, __FILE__, __LINE__)
#define	TPKTIDMALLOC(type, num, vh_nbr)	(type *) mm_pktid_malloc(sizeof(type) * num, __FILE__, __LINE__, vh_nbr)
#define	TREALLOC(ptr, type, num)	(type *) mm_realloc(ptr, sizeof(type) * num, __FILE__, __LINE__)
void *	mm_calloc(unsigned, unsigned, char *, int);
void	mm_cmd();
void *	mm_malloc(unsigned, char *, int);
void	mm_free(char *, char *, int);
void *	mm_realloc(char *, unsigned, char *, int);
