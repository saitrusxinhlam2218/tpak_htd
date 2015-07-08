/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: parman.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1999/07/21 17:37:49 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/async_dual/parman.c,v $
*
*  Copyright (c) 1992 - Motorola Mobile Data Division, Seattle, WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: parman.c,v $	$Revision: 1.2 $"

/* HEADER */
/******************************************************************************
NAME:
		parman.c
		========


DESCRIPTION:
		PARMAN module with functions to be performed on user
	application code requests as from Configuration Manager package.
	For detailed description see document:
		"Parameter Manager Programmer`s Manual".


INPUT DATA:


OUTPUT DATA:


NOTES:


UPDATE HISTORY:

Darko N. Vranesic	March 27, 1987		Original.
Raymond Chau		June 16, 1987		Phase II Implementation.
Patrick TOng		March 9, 1988		Added show_parsts() routine.


*******************************************************************************
*
*		Copyright (c), 1987, 1988
*		All rights reserved
*
*	MOBILE DATA INTERNATIONAL, INC.
*		RICHMOND, BRITISH COLUMBIA
*		CANADA
*
******************************************************************************/
/* END HEADER */

/* INCLUDE FILES */

#include	<errno.h>
#include	<stdio.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<string.h>
#include	<sys/types.h>
#include	<sys/ipc.h>
#include	<sys/shm.h>
#include	<sys/sem.h>
#include	"std.h"
#include	"parman.h"
#include	"_parman.h"
#include        "taxipak.h"

#define	ERROR	-1
LOCAL CHAR SccsId[]="@(#)parman.c	1.0	88/03/09	PT	NG";

/* IMPORTED VARIABLES AND FUNCTIONS */

/* LOCAL DEFINES */
LOCAL	CHAR	*CRLF_ptr = "\015\012";	/* Carriage return char */

/* LOCAL TYPEDEFS */

IMPORT	INT	open_set_cnt;	/* # of parmaeter Sets currently opened */
IMPORT	INT	parset_index;	/* Index within par_set table */
/* IMPORT	INT	par_errno;	PARMAN error code (in parman_import.h) */

IMPORT	struct
{
	CHAR	set_name[MAXLEN_PS_NAME];
	INT	id;
	BYTE	*status_ptr;
} par_set[MAX_SET_OPENED];

IMPORT	CHAR	pm_path[80];	/* PARMAN Sets directory path */
IMPORT	INT	pm_sal_id;	/* Status Area Lock identifier (semid) */
IMPORT	INT	pm_sa_id;	/* Status Area identifier (shmid) */
IMPORT	CHAR	*shm_ptr;	/* Pointer to the beginning of the raw SHM */
IMPORT	SHM	*sts_area;	/* Pointer to the beginning of Status Area */
IMPORT	INT	process_id;	/* Process identifier (system wide) */

IMPORT 	INT	sts_area_index;	/* sts_area table : current row index */
IMPORT	INT	sts_index;	/* sts_area table : current sts column index */

IMPORT	CHAR	rw_buffer[RECORD_LENGTH];	/* scratch read/write buffer */

IMPORT	struct
{
	INT	l_type;
	INT	l_whence;
	LONGINT		l_start;
	LONGINT		l_len;
	INT		l_pid;
} *flock_ptr;


IMPORT	struct	sembuf	PM_sembuf;
IMPORT	struct	sembuf	*PM_sops;

/* Subroutines */

INT	check_accflag();
INT	check_lockflag();
INT	check_waitflag();
INT	find_parset_id();
INT	get_sa_ids();
INT	get_pm_path();
INT	opnset_add();
INT	opnset_del();
INT	sa_ent_add();
INT	sa_ent_del();
INT	sa_lock();
INT	sa_unlock();
INT	set_par_errno();
INT	sa_attach();
INT	sa_detach();
INT	check_accperm();
INT	check_nonprint();
INT	check_par_name();
INT	find_par_name();
INT	find_parset_name();


/* CODE */


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>     parset_create     <<<<<<< */

INT parset_create (parset_name_ptr, access_perm)

CHAR	*parset_name_ptr;
INT access_perm;
{
	CHAR	set_path[NOC_PATHNAME];		/* full Set name */
	CHAR	*set_path_ptr = &set_path[0];
	INT	parset_id;	/* Parameter Set identifier */
	CHAR	*pm_path_ptr;

/* Check syntax of the access_perm argument */
	if (check_accperm(access_perm) == ERROR)
	{
		par_errno = PM_ERR_INVARG;
		return(ERROR);
	}

/* Get PARMAN Sets directory path */
	if (get_pm_path() == ERROR)
	{
		par_errno = PM_ERR_CONFIG;
		return(ERROR);
	}

/* File name will be full path name, so we have to define it
   as pm_path + parset_name */
	pm_path_ptr = pm_path;			/* Set PARMAN path pointer */
	strcpy(set_path_ptr, pm_path_ptr);	/* copy path */
	strcat(set_path_ptr, parset_name_ptr);	/* append file name */

/* Test if the file already exists */
	if (open(set_path_ptr, (O_RDWR | O_CREAT | O_EXCL)) == -1)
	{
		par_errno = PM_ERR_SETEXI;
		return(ERROR);
	}
	else
		unlink(set_path_ptr);

/* Create the file */
	parset_id = creat(set_path_ptr, access_perm);

	if(parset_id == -1)
	{
		set_par_errno(errno, SR_SET_CREATE);   /* create failed */
		return(ERROR);
	}

	if (close(parset_id) == -1)
	{
		set_par_errno(errno, SR_SET_CLOSE);
		return(ERROR);
	}

	return(SUCCESS);
}




/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>     parset_delete     <<<<<<< */
INT	parset_delete(parset_name_ptr)

CHAR	*parset_name_ptr;
{
	CHAR	set_path[NOC_PATHNAME];
	CHAR	*set_path_ptr = &set_path[0];
	CHAR	*pm_path_ptr;
	INT	parset_id = -1;
	INT	i;


/* Get pm_path */
	if (get_pm_path() == ERROR)
	{
		par_errno = PM_ERR_CONFIG;
		return(ERROR);
	}

	pm_path_ptr = pm_path;
	strcpy(set_path_ptr, pm_path_ptr);
	strcat(set_path_ptr, parset_name_ptr);


/* If this file is in the par_set table, return BUSY */
	for (i = 0; i < MAX_SET_OPENED; i++)
	{
	    if (par_set[i].id != FREE)
	    {
		/* compare only for valid, used entries in the par_set table */
		if (strncmp(parset_name_ptr, &par_set[i].set_name[0],
						MAXLEN_PS_NAME) == 0)
		{
			/* Set name found => set is opened */
			par_errno = PM_ERR_BUSY;
			return(ERROR);
		}
	    }
	}

/* Get file descriptor for given set_name */
	if ((parset_id = open(set_path_ptr, O_RDONLY)) == -1)
	{
		if (errno == EACCES)
			par_errno = PM_ERR_ACCESS;
		else
			par_errno = PM_ERR_UNKSET;
		return(ERROR);
	}
	close(parset_id);	/* Open was done due to check of existance */

/* Check if Set is busy */
	/* If no Sets opened, attach to Status Area */
	if (open_set_cnt == 0)
	{
		if (get_sa_ids() == ERROR)
			return(ERROR);		/* CONFIG error */
		else
		{
			if (sa_attach() == ERROR)
			{
				par_errno = PM_ERR_STSACC;
				return(ERROR);
			}
		}
	}

/* Find set_name in the Status Area, and if found, return BUSY */
	if (find_parset_name(parset_name_ptr) == SUCCESS)
	{
		par_errno = PM_ERR_BUSY;
		if (open_set_cnt == 0)
			sa_detach();
		return(ERROR);
	}

/* Deatch from the Status Area if necessary */
	if (open_set_cnt == 0)
	{
		if (sa_detach() == ERROR)
		{
			par_errno = PM_ERR_STSACC;
			return(ERROR);
		}
	}

/* Delete the file using full path name */
	if (unlink(set_path_ptr) == -1)
	{
		set_par_errno(errno, SR_SET_DELETE);
		return(ERROR);
	}

	return(SUCCESS);
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>     parsts_set     <<<<<<< */
INT	parsts_set(parset_id)

INT	parset_id;
{
	CHAR	*help_ptr;
	INT	i;

	/* find set_name in the par_set table given id */
	if (find_parset_id(parset_id) == ERROR)
	{
		par_errno = PM_ERR_INVARG;
		return(ERROR);
	}

	/* found, pointed by parset_index */
	help_ptr = par_set[parset_index].set_name;

	/* lock the Status Area */
	if (sa_lock() == ERROR)
	{
		par_errno = PM_ERR_SALOCK;
		return(ERROR);
	}

	/* set the corresponding sts_area_index for the set name */
	if (find_parset_name(help_ptr) == ERROR)
	{
		par_errno = PM_ERR_UNKSET;
		sa_unlock();
		return(ERROR);
	}

	/* set the status for all the active process_id's */
	for (i = 0; i < MAXPROC_PERSET; i++)
	{
		if (sts_area->rec[sts_area_index].sts[i].process_id != FREE)
	       	    sts_area->rec[sts_area_index].sts[i].status = PM_STAT_SET;
	}

	/* unlock the Status Area */
	if (sa_unlock() == ERROR)
	{
		par_errno = PM_ERR_SAULOCK;
		return(ERROR);
	}
	else
		return(SUCCESS);
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>     parval_create     <<<<<<< */
INT	parval_create(parset_id, par_name_ptr, par_value_ptr)

INT	parset_id;
CHAR	*par_name_ptr;
CHAR	*par_value_ptr;
{
	INT	i;		/* loop index */


	if (find_parset_id(parset_id) == ERROR)
	{
		par_errno = PM_ERR_INVARG;
		return(ERROR);
	}

	if (check_par_name(par_name_ptr) == ERROR)
	{
		par_errno = PM_ERR_INVARG;
		return(ERROR);
	}

	if (MAXLEN_P_VALUE < strlen(par_value_ptr))
	{
		par_errno = PM_ERR_TOOBIG;
		return(ERROR);
	}

	if (check_nonprint(par_value_ptr) == ERROR)
	{
		par_errno = PM_ERR_NONPRI;
		return(ERROR);
	}

/* Look for Parameter name in the file */
	if (find_par_name(parset_id, par_name_ptr) == SUCCESS)
	{
		par_errno = PM_ERR_PAREXI;
		return(ERROR);
	}

/* Write new record at the end of file */
	if (lseek(parset_id, 0, OFF_END) == -1)
		return(ERROR);

/* Prepare buffer and write it into the file */
	strncpy(&rw_buffer[0], par_name_ptr, MAXLEN_P_NAME);
	for(i = strlen(par_name_ptr); i < MAXLEN_P_NAME; i++)
		rw_buffer[i] = '\040';		/* trailing spaces */
	rw_buffer[i] = '\0';   /* stuff null at the end for string operation */
	strncat(&rw_buffer[0], par_value_ptr, MAXLEN_P_VALUE);
	for(i = strlen(rw_buffer); i < (MAXLEN_P_NAME + MAXLEN_P_VALUE); i++)
		rw_buffer[i] = '\040';		/* trailing spaces */
	rw_buffer[i] = '\0';   /* stuff null at the end for string operation */
	strcat(&rw_buffer[0], CRLF_ptr);	/* trailing <CR> */

	if (write(parset_id, rw_buffer, RECORD_LENGTH) == -1)
	{
		set_par_errno(errno, SR_VAL_WRITE);
		return(ERROR);
	}

	return(SUCCESS);
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>     parval_delete     <<<<<<< */
INT	parval_delete(parset_id, par_name_ptr)

INT	parset_id;
CHAR	*par_name_ptr;
{
	INT	i;

	if (find_parset_id(parset_id) == ERROR)
	{
		par_errno = PM_ERR_INVARG;
		return(ERROR);
	}

	if (find_par_name(parset_id, par_name_ptr) == ERROR)
	{
		par_errno = PM_ERR_UNKPAR;
		return(ERROR);
	}

/* Prepare buffer and write "deleted record" into the file */
	for(i = 0; i < (MAXLEN_P_NAME + MAXLEN_P_VALUE); i++)
		rw_buffer[i] = '\052';		/*   ="*"   */
	rw_buffer[i] = '\0';   /* stuff null at the end for string operation */
	strcat(&rw_buffer[0], CRLF_ptr);	/* trailing <CR> */

	if (write(parset_id, rw_buffer, RECORD_LENGTH) == -1)
	{
		set_par_errno(errno, SR_VAL_WRITE);
		return(ERROR);
	}

	return(SUCCESS);
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>     parval_write     <<<<<<< */
INT	parval_write(parset_id, par_name_ptr, par_value_ptr)

INT	parset_id;
CHAR	*par_name_ptr;
CHAR	*par_value_ptr;
{
	INT	noc;
	INT	i;


	if (find_parset_id(parset_id) == ERROR)
	{
		par_errno = PM_ERR_INVARG;
		return(ERROR);
	}

	if (check_par_name(par_name_ptr) == ERROR)
	{
		par_errno = PM_ERR_INVARG;
		return(ERROR);
	}

	if (MAXLEN_P_VALUE < strlen(par_value_ptr))
	{
		par_errno = PM_ERR_TOOBIG;
		return(ERROR);
	}

	if (check_nonprint(par_value_ptr) == ERROR)
	{
		par_errno = PM_ERR_NONPRI;
		return(ERROR);
	}

	if (find_par_name(parset_id, par_name_ptr) == ERROR)
	{
		par_errno = PM_ERR_UNKPAR;
		return(ERROR);
	}

/* Prepare buffer and write it into the file */
	strncpy(&rw_buffer[0], par_name_ptr, MAXLEN_P_NAME);
	for(i = strlen(par_name_ptr); i < MAXLEN_P_NAME; i++)
		rw_buffer[i] = '\040';		/* trailing spaces */
	rw_buffer[i] = '\0';   /* stuff null at the end for string operation */
	strncat(&rw_buffer[0], par_value_ptr, MAXLEN_P_VALUE);
	for(i = strlen(rw_buffer); i < (MAXLEN_P_NAME + MAXLEN_P_VALUE); i++)
		rw_buffer[i] = '\040';		/* trailing spaces */
	rw_buffer[i] = '\0';   /* stuff null at the end for string operation */
	strcat(&rw_buffer[0], CRLF_ptr);	/* trailing <CR> */

	if ((noc = write(parset_id, rw_buffer, RECORD_LENGTH)) == -1)
	{
		set_par_errno(errno, SR_VAL_WRITE);
		return(ERROR);
	}

	/* Determine value length written */
	noc = MAXLEN_P_VALUE;
	for (i = (MAXLEN_P_NAME + MAXLEN_P_VALUE - 1); rw_buffer[i] == '\040';
									i--)
		noc -= 1;	/* decrement value length */

	if (i <= 0)
		return(0);
	else
		return(noc);
}



/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>     check_accperm     <<<<<<< */
INT	check_accperm (access_perm)

INT	access_perm;
{
	if (((access_perm & PM_PERM_ALL) == 0) || ((access_perm & 0111) != 0))
	{
		par_errno = PM_ERR_INVARG;
		return(ERROR);
	}

	return(SUCCESS);
}



/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>     check_nonprint     <<<<<<< */

INT	check_nonprint(string_ptr)

CHAR	*string_ptr;
{
	INT	i;
	CHAR	*ptr;
	INT	ret = SUCCESS;

	ptr = string_ptr;

	for(i = 0; (i < MAXLEN_P_VALUE) && (i < strlen(string_ptr)); i++)
	{
		if ((*ptr < '\040') || (*ptr > '\176'))
		{
			ret = ERROR;
			break;
		}
		else
			ptr++;
	}

	return(ret);
}




/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>     check_par_name     <<<<<<< */

INT	check_par_name(par_name_ptr)

CHAR	*par_name_ptr;
{
	CHAR	*ptr;
	INT	i = -1;
	INT	ret = ERROR;

	ptr = par_name_ptr;

	while (++i < strlen(par_name_ptr))
	{
		if ((*ptr == '\056') ||
		    ((*ptr >= '\060') && (*ptr <= '\071')) ||
		    ((*ptr >= '\101') && (*ptr <= '\132')) ||
		    (*ptr == '\137') ||
		    ((*ptr >= '\141') && (*ptr <= '\172')))
		{
			ret = SUCCESS;
			ptr++;
		}
		else
		{
			ret = ERROR;
			break;
		}
	}

	if (i > MAXLEN_P_NAME)
		return(ERROR);
	else
		return(ret);
}





/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>     find_par_name     <<<<<<< */

INT	find_par_name(parset_id, par_name_ptr)

INT	parset_id;
CHAR	*par_name_ptr;
{
	INT	noc;
	INT	rec_pos;
	CHAR	*value[MAXLEN_P_VALUE];

/* Position file pointer to the file beginning */
	if (lseek(parset_id, 0, OFF_BEG) == -1)
		return(ERROR);

/* Search for the Parameter name */
	if ((noc = parval_read(parset_id, par_name_ptr, value, MAXLEN_P_VALUE))
								!= ERROR)
	{
		/* name found, position pointer to that record */
		if ((rec_pos = lseek(parset_id, 0, OFF_CURR)) == -1)
			return(ERROR);
		else
		{
			/* rec_pos is # of bytes from the file beginning */
			/* prepare file pointer for overwriting this record */
			if (lseek(parset_id, (rec_pos - RECORD_LENGTH), OFF_BEG)
									== -1)
				return(ERROR);
			else
				return(SUCCESS);
		}
	}
	else
		return(ERROR);
}




/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>     find_parset_name     <<<<<<< */

INT	find_parset_name(parset_name_ptr)

CHAR	*parset_name_ptr;
{
	INT	i;
	INT	found = FALSE;

	/* search for the index given the set name */
	for (i = 0; (i < MAXNUM_PARSET) && (found == FALSE); i++)
	{
		if (strcmp(sts_area->rec[i].set_name, parset_name_ptr) == 0)
		{
			sts_area_index = i;
			found = TRUE;
		}
	}

	/* return the result */
	if (found != TRUE)
		return(ERROR);
	else
		return(SUCCESS);
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>     show_parsts     <<<<<<< */
INT	show_parsts(parset_id)

INT	parset_id;
{
	CHAR	*help_ptr;
	INT	i;
	INT     stu;		/* temporary set status */
	CHAR	stustr[7];	/* status string */

	/* find set_name in the par_set table given id */
	if (find_parset_id(parset_id) == ERROR)
	{
		par_errno = PM_ERR_INVARG;
		return(ERROR);
	}

	/* found, pointed by parset_index */
	help_ptr = par_set[parset_index].set_name;

	/* lock the Status Area */
	if (sa_lock() == ERROR)
	{
		par_errno = PM_ERR_SALOCK;
		return(ERROR);
	}

	/* set the corresponding sts_area_index for the set name */
	if (find_parset_name(help_ptr) == ERROR)
	{
		par_errno = PM_ERR_UNKSET;
		sa_unlock();
		return(ERROR);
	}

	/* display status of all the active process_id's */
	for (i = 0; i < MAXPROC_PERSET; i++)
	{
		if (sts_area->rec[sts_area_index].sts[i].process_id != FREE)
		{
		    stu = sts_area->rec[sts_area_index].sts[i].status;
		    if (stu == PM_STAT_SET)
			strcpy(stustr,"SET");
                    else if
		       (stu == PM_STAT_RESET)
			strcpy(stustr,"RESET");
                    else
			strcpy(stustr,"UNKNOW");

		    fprintf(stdout,"Pid: %d  Status: %s\n",
			    sts_area->rec[sts_area_index].sts[i].process_id,
	       	            stustr);
		}
	}

	/* unlock the Status Area */
	if (sa_unlock() == ERROR)
	{
		par_errno = PM_ERR_SAULOCK;
		return(ERROR);
	}
	else
		return(SUCCESS);
}

