/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ipc.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1999/07/21 17:37:48 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/async_dual/ipc.c,v $
*
*  Copyright (c) 1992 - Motorola Mobile Data Division, Seattle, WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: ipc.c,v $	$Revision: 1.2 $"

/* HEADER */
/******************************************************************************
NAME:
		ipc.c
		=====

DESCRIPTION:
		Contains source of all IPC routines for System V compatible
Operating Systems regarding Message Queues implementation.

Basic modules :
***************

							RETURN CODE
NAME		PARAMETERS			ON SUCCESS	ON ERROR
=============================================================================
ipc_create					ipcs_id		ERR_xxx

		1. ipcs_name_ptr
		2. ipcs_size
		3. ipcp	
______________________________________________________________________________
ipcs_delete					Zero (0)	ERR_xxx

		1. ipcs_name_ptr
______________________________________________________________________________
ipcs_open					ipcs_id		ERR_xxx

		1. ipcs_name_ptr
		2. access_flag	
______________________________________________________________________________
ipcs_close					Zero (0)	ERR_xxx

		1. ipcs_id
______________________________________________________________________________
ipcs_getname					Zero (0)	ERR_xxx

		1. ipcs_id
		2. ipcs_name_ptr
______________________________________________________________________________
ipcs_info					Zero (0)	ERR_xxx

		1. ipcs_id
		2. ipcs_inf_buf_ptr
______________________________________________________________________________
ipce_write					Zero (0)	ERR_xxx

		1. ipcs_id
		2. ipcs_ptr
		3. ipce_len
		4. wait_flag
______________________________________________________________________________
ipce_read					Number of	ERR_xxx
						bytes read

		1. ipcs_id
		2. ipcs_ptr
		3. max_size
		4. wait_flag
==============================================================================






Lowest primitives :
*******************

							RETURN CODE
NAME		PARAMETERS			ON SUCCESS	ON ERROR
==============================================================================
perm_syntax					SUCCESS		ERROR (-1)
		1. perm
______________________________________________________________________________
check_perm					SUCCESS		ERROR (-1)
		1. ipcs_id
		2. ipcp
		3. ipc_ipci_ptr
______________________________________________________________________________
reterr						ERR_xxx		ERR_DEFAULT
		1. error_code
		2. module_number
______________________________________________________________________________
ipcdb_open					SUCCESS		ERR_CONFIG
______________________________________________________________________________
ipcdb_close					SUCCESS		ERROR (-1)
______________________________________________________________________________
ipcdb_write					SUCCESS		ERROR (-1)
		1. ipcs_id
		2. ipcs_name_ptr
		3. ipcp
______________________________________________________________________________
ipcdb_fnd_name					ipcs_id		ERR_xxx
		1. ipcs_name_ptr
		2. ipcs_name_len
______________________________________________________________________________
ipcdb_fnd_id					SUCCESS		ERR_xxx
		1. ipcs_id
		2. ipcs_name_ptr (OUTPUT parameter)
______________________________________________________________________________
ipcdb_delete					ipcs_id		ERR_xxx
		1. ipcs_name_ptr
==============================================================================



INPUT DATA:

	Depends on module. See DESCRIPTION.

OUTPUT DATA:

	Depends on module. See DESCRIPTION.

NOTES:

	All this is valid for Message Queues implementation only!!!

UPDATE HISTORY:

Darko N. Vranesic	Jan 14, 1988	Added returning of ERR_INTRPT.
					Defined Sccsid.
Darko N. Vranesic	February 24, 1987	IPCX_ were removed from
						structure definitions in
						IPCS.h & IPCI.h.
Darko N. Vranesic	February 10, 1987	Original.


*******************************************************************************
*
*		Copyright (c), 1987
*		All rights reserved
*
*	MOBILE DATA INTERNATIONAL, INC.
*		RICHMOND, BRITISH COLUMBIA
*		CANADA
*
******************************************************************************/
/* END HEADER */

/* INCLUDE FILES */

#include	<stdio.h>
#include	<unistd.h>
#include	<errno.h>
#include	<sys/types.h>
#include	<sys/ipc.h>
#include	<sys/msg.h>
#include	"std.h"
#include	"MDIPC.h"
#include	"IPCS.h"
#include	"IPCI.h"
#include        "taxipak.h"

/* IMPORTED VARIABLES AND FUNCTIONS */
extern int	errno;

/* LOCAL DEFINES */
#define	ERROR	-1
#define	IPC_FILE	"/usr/taxi/run/ipc.db"

/* LOCAL TYPEDEFS */

LOCAL CHAR Sccsid[]="@(#)ipc.c	1.3	Jan 14, 1988	DNV	NG";

IPCS	ipc_ipcs;
IPCS	*ipc_ipcs_ptr;
IPCI	ipc_ipci;
IPCI	*ipc_ipci_ptr;


FILE	*ipcdb_ptr;	/* Pointer to the IPC database after open */
INT	ipcdb_fd;	/* IPC database file descriptor */

/* CODE */

/* ************************ ipcs_create ************************ */
ipcs_create (ipcs_name_ptr, ipcs_size, ipcp)
CHAR	*ipcs_name_ptr;
INT	ipcs_size;
UINT	ipcp;
{
	INT	ipcs_id;		/* IPC structure identifier */
	INT	rc;			/* Return code from subroutines */

	ipc_ipci_ptr = &ipc_ipci;

	/* Check for parameters */
	if (ipcs_size <= -1)
		return(ERR_INVPAR);	/* Size less than 0 */
	if ((ipcp <= SUCCESS) || (perm_syntax(ipcp) != SUCCESS))
		return(ERR_INVPAR);	/* Bad permission parameter */


	/* Open IPC database for update */
	if (ipcdb_open() != SUCCESS)
		return(ERR_CONFIG);	/* Error is defined in ipcdb_open */


	/* Search for IPC structure name in IPC database; we will allow
				creation only if the name was not found */
	if ((ipcs_id = ipcdb_fnd_name(ipcs_name_ptr, strlen(ipcs_name_ptr)))
								<= ERROR)
	{
		if (ipcs_id != ERR_NMINVA)
		{
			/* Close IPC database */
			if (ipcdb_close() == ERROR)
				return(ERR_CONFIG);
			else
				return(ipcs_id);	/* Fnd error */
		}
		/* else name not found, so we can create it */
	}
	else
	{
		if (ipcdb_close() != SUCCESS)
			return(ERR_CONFIG);	/* IPC database close error */
		else
			return(ERR_NMPRES);	/* Name already present */
	}


	/* Try to get IPC structure ID and, if successful,
              define maximum # of bytes in MQ and put entry into IPC database */
	if ((ipcs_id = msgget(MDIPC_KEY, (ipcp | MDIPC_UNIQUE))) == ERROR)
	{
		if (ipcdb_close() == ERROR)	/* Close IPC database */
			return(ERR_CONFIG);
		else
			return(reterr(errno, CREATE));/* Error in MQ get call */
	}


	/* Change max. MQ size from default to that one specified by caller */
	/* Watch that from this point, if any error encountered, we have to
							remove created MQ */
	if ((rc = ipcs_info(ipcs_id, ipc_ipci_ptr)) != SUCCESS)
	{
		/* Remove MQ created due to error */
		msgctl(ipcs_id, IPCF_DELETE, ipc_ipci_ptr);
		if (ipcdb_close() == ERROR)	/* Close IPC database */
			return(ERR_CONFIG);
		else
			return(rc); /* Error in MQ info call */
	}
	else
	{
		ipc_ipci_ptr->qbytes = ipcs_size;
		if (msgctl(ipcs_id, IPCF_SET, ipc_ipci_ptr) != SUCCESS)
		{
			/* Remove MQ created due to error */
			msgctl(ipcs_id, IPCF_DELETE, ipc_ipci_ptr);
			if (ipcdb_close() == ERROR)	/* Close IPC database */
				return(ERR_CONFIG);
			else
			{
				if (errno == EPERM)
					/* Unable to raise MQ size */
					return(ERR_TOOBIG);
				else
					return(reterr(errno, INFO));
			}
		}
	}


	if (ipcdb_write(ipcs_id, ipcs_name_ptr, ipcp) == ERROR)
	{
		msgctl(ipcs_id, IPCF_DELETE, ipc_ipci_ptr);
		if (ipcdb_close() == ERROR)	/* Close IPC database */
			return(ERR_CONFIG);
		else
			return(reterr(errno, WRITE));
	}

	if (ipcdb_close() == ERROR)	/* Close IPC database */
	{
		msgctl(ipcs_id, IPCF_DELETE, ipc_ipci_ptr);
		return(ERR_CONFIG);
	}

	return(ipcs_id);   /* Operation successful, return IPCS identifier */
}



/* ************************ ipcs_delete ************************ */
ipcs_delete(ipcs_name_ptr)
CHAR	*ipcs_name_ptr;
{
	INT	rc;	/* Return code */
	INT	rc1;	/* Return code (next) */


	/* Check existance of specified IPC structure name in IPC database
					and remove entire record from there */
	if (ipcdb_open() != SUCCESS)
		return(ERR_CONFIG);	/* Error is defined in ipcdb_open */


	/* Search for IPC structure name in IPC database;
			If failed, that means we can not delete it */
	if((rc = ipcdb_fnd_name(ipcs_name_ptr, strlen(ipcs_name_ptr))) <= ERROR)
	{
		if (ipcdb_close() == ERROR)	/* Close IPC database */
			return(ERR_CONFIG);
		else
			return(rc);	/* Error from delete name */
	}
	else
	{
		ipc_ipci_ptr = &ipc_ipci;
		/* Remove MQ from the system  but call INFO first to
			define ipc_ipci_ptr an dif successful, delete
			entry from IPC_CONFIG database */
		if((rc1 = ipcs_info(rc, ipc_ipci_ptr)) != SUCCESS)
			{
			if (ipcdb_close() == ERROR)
				return(ERR_CONFIG);
			else
				return(rc1);
			}
		else if (msgctl(rc, IPCF_DELETE, ipc_ipci_ptr) != SUCCESS)
			{
			if (ipcdb_close() == ERROR)
				return(ERR_CONFIG);
			else
				return(reterr(errno, INFO));
			}
		else if ((rc = ipcdb_delete(ipcs_name_ptr)) <= ERROR)
			{
			if (ipcdb_close() == ERROR)
				return(ERR_CONFIG);
			else
				return(rc);
			}
		else
			{
			if (ipcdb_close() == ERROR)
				return(ERR_CONFIG);
			else
				return(SUCCESS);
			}
	}
}

	

/* ************************ ipcs_open ************************ */
ipcs_open(ipcs_name_ptr, access_flag)
CHAR	*ipcs_name_ptr;
UINT	access_flag;
{
	INT	rc;
	INT	ipcs_id;


	ipc_ipci_ptr = &ipc_ipci;


	/* Check permission parameter */
	if ((access_flag <= SUCCESS) ||
		((access_flag != IPCF_READ) && (access_flag != IPCF_UPDATE)))
		return(ERR_INVPAR);	/* Bad permission parameter */


	/* Lookup in the IPC database if the name exists.
		If so, check if access to IPC structure is to be granted */
	if((rc = ipcdb_open()) != SUCCESS)
		return(rc);			/* IPC database access error */


	/* From this point, if error, do not forget to close IPC database */
	if ((ipcs_id = ipcdb_fnd_name(ipcs_name_ptr, strlen(ipcs_name_ptr)))
								<= ERROR)
	{
		if (ipcdb_close() == ERROR)	/* Close IPC database */
			return(ERR_CONFIG);
		else
			return(ERR_NMINVA);
	}

	/* Check if access permission will be granted depending on the
			permission that was defined by IPC structure creator */
	if ((rc = ipcs_info(ipcs_id, ipc_ipci_ptr)) != SUCCESS)
	{
		if (ipcdb_close() != SUCCESS)
			return(ERR_CONFIG);
		else
			return(rc);
	}

	/* Check permission */
	if ((rc = check_perm(ipcs_id, access_flag, ipc_ipci_ptr)) != SUCCESS)
	{
		if (ipcdb_close() != SUCCESS)
			return(ERR_CONFIG);
		else
			return(ERR_ACCESS);
	}


	if (ipcdb_close() != SUCCESS)
		return(ERR_CONFIG);		/* IPC database close error */

	return(ipcs_id);	/* Success, return IPC structure identifier */
}




/* ************************ ipcs_close ************************ */
ipcs_close(ipcs_id)
INT	ipcs_id;
{
	CHAR	ipcs_name[IPCS_NAME_MXSZ];
	CHAR	*ipcs_name_ptr;

	ipc_ipcs_ptr = &ipc_ipcs;
	ipcs_name_ptr = &ipcs_name[0];


	/* Lookup in the IPC database if the name exists.
		If so, close the IPC database and return success */
	if (ipcdb_open() != SUCCESS)
		return(ERR_CONFIG);		/* IPC database access error */


	/* From this point, if error, do not forget to close IPC database */
	/* Try to find ipcs_id in the IPC database */
	if (ipcdb_fnd_id(ipcs_id, ipcs_name_ptr) != SUCCESS)
	{
		if (ipcdb_close() == ERROR)	/* Close IPC database */
			return(ERR_CONFIG);
		else
			return(ERR_IDINVA);
	}
	

	/* ipcs_close is dummy call for MESSAGE QUEUES implementation */
	return(SUCCESS);
}






/* ************************ ipcs_getname ************************ */
ipcs_getname(ipcs_id, ipcs_name_ptr)
INT	ipcs_id;
CHAR	*ipcs_name_ptr;
{
	INT	rc;


	/* Lookup in the IPC database if the name exists.
		If so, return IPC structure identifier */
	if((rc = ipcdb_open()) != SUCCESS)
		return(rc);			/* IPC database access error */


	/* From this point, if error, do not forget to close IPC database */
	if ((rc = ipcdb_fnd_id(ipcs_id, ipcs_name_ptr)) <= ERROR)
	{
		if (ipcdb_close() == ERROR)	/* Close IPC database */
			return(ERR_CONFIG);
		else
			return(ERR_IDINVA);
	}

	if (ipcdb_close() == ERROR)	/* Close IPC database */
		return(ERR_CONFIG);
	else
		return(SUCCESS);
}







/* ************************ ipcs_info ************************ */
ipcs_info(ipcs_id, ipcs_inf_buf)
INT	ipcs_id;
IPCI	*ipcs_inf_buf;
{
	CHAR	*ipcs_name_ptr;
	CHAR	ipcs_name[IPCS_NAME_MXSZ];

	ipc_ipcs_ptr = &ipc_ipcs;
	ipcs_name_ptr = &ipcs_name[0];


	if (msgctl(ipcs_id, IPCF_STAT, ipcs_inf_buf) != SUCCESS)
		return(reterr(errno,INFO));
	else
		return(SUCCESS);
}




/* ************************ ipce_write ************************ */
ipce_write(ipcs_id, ipcs_ptr, ipce_len, wait_flag)
INT	ipcs_id;
IPCS	*ipcs_ptr;
INT	ipce_len;
INT	wait_flag;
{
	INT	rc;

	rc = SUCCESS;

	/* Check for valid wait_flag value */
	switch (wait_flag)
	{
	case IPCF_WAIT:
		wait_flag = IPC_SYSF_WAIT;
		break;
	case IPCF_NOWAIT:
		wait_flag = IPC_SYSF_NOWAIT;
		break;
	default:
		rc = ERR_INVPAR;
		break;
	}

	if (rc != SUCCESS)
		return(rc);

	/* Perform write */
	if ((rc = msgsnd(ipcs_id, ipcs_ptr, ipce_len, wait_flag)) != SUCCESS)
		return(reterr(errno, WRITE));
	else
		return(SUCCESS);
}



/* ************************ ipce_read ************************ */
ipce_read(ipcs_id, ipcs_ptr, max_size, wait_flag)
INT	ipcs_id;
IPCS	*ipcs_ptr;
INT	max_size;
INT	wait_flag;
{
	INT	rc;

	rc = SUCCESS;

	/* Check for valid wait_flag value */
	switch (wait_flag)
	{
	case IPCF_WAIT:
		wait_flag = IPC_SYSF_WAIT;
		break;
	case IPCF_NOWAIT:
		wait_flag = IPC_SYSF_NOWAIT;
		break;
	default:
		rc = ERR_INVPAR;
		break;
	}

	if (rc != SUCCESS)
		return(rc);

	/* Perform read */
	if ((rc = msgrcv(ipcs_id, ipcs_ptr, max_size, MIN_PRIOR, wait_flag))
							<= ERROR)
		return(reterr(errno, READ));
	else
		return(rc);
}





/* ------------------------ perm_syntax ------------------------ */
perm_syntax(perm)
UINT	perm;
{
	if (((perm & IPCP_ALL) == 0) || ((perm & 0111) != 00))
		return(ERROR);
	else
		return(SUCCESS);
}




/* ------------------------ check_perm ------------------------ */
check_perm(ipcs_id, ipcp, chkp_ipci_ptr)
INT	ipcs_id;
UINT	ipcp;
IPCI	*chkp_ipci_ptr;
{
	UINT	perm_r;		/* Read permission */
	UINT	perm_w;		/* Write permission */
	UINT	perm_rw;	/* Read/write permission */
	UINT	acc_mod;	/* Access (permission) mode */


	/* Define who is user */
	if (getuid() == chkp_ipci_ptr->perm.uid)
	{
		/* User is IPC structure owner */
		perm_r = IPCP_UR;
		perm_w = IPCP_UW;
		perm_rw = IPCP_UALL;
	}
	else if (getgid() == chkp_ipci_ptr->perm.gid)
	{
		/* User has the same group as IPC structure owner */
		perm_r = IPCP_GR;
		perm_w = IPCP_GW;
		perm_rw = IPCP_GALL;
	}
	else
	{
		/* Belongs to others */
		perm_r = IPCP_OR;
		perm_w = IPCP_OW;
		perm_rw = IPCP_OALL;
	}
		
	
	/* Check permission */
	acc_mod = perm_rw & chkp_ipci_ptr->perm.mode;

	if (acc_mod == perm_rw)
		/* Read/write access permitted */
		return(SUCCESS);

	if (acc_mod == perm_r)
		/* Read access only permitted */
	{
		if ((ipcp & perm_rw) != perm_r)
			/* Something else than read access requested */
			return(ERROR);
		else
			/* Read access requested */
			return(SUCCESS);
	}
	else
		return(ERROR);
}





/* ------------------------ reterr ------------------------ */
reterr(error_code, module_no)
INT	error_code;
INT	module_no;
{
	INT	rc;

	switch (module_no)
	{
	case CREATE:
		switch (error_code)
		{
		case ENOSPC:
			rc = ERR_NOSPAC;
			break;
		case EACCES:
			rc = ERR_ACCESS;
			break;
		case ENOENT:
			rc = ERR_UNACRE;
			break;
		case EEXIST:
			rc = ERR_IDPRES;
			break;
		case EINTR:
			rc = ERR_INTRPT;
			break;
		default:
			rc = ERR_DEFAULT;
			break;
		};
		break;

	case INFO:
		switch (error_code)
		{
		case EINVAL:
			rc = ERR_IDINVA;
			break;
		case EACCES:
			/* Fall through  - return(ERR_ACCESS) */
		case EPERM:
			rc = ERR_ACCESS;
			break;
		case EFAULT:
			rc = ERR_PTRINV;
			break;
		case EINTR:
			rc = ERR_INTRPT;
			break;
		default:
			rc = ERR_DEFAULT;
			break;
		};
		break;

	case	WRITE:
		switch (error_code)
		{
		case EACCES:
			rc = ERR_ACCESS;
			break;
		case EINVAL:
			rc = ERR_INVPAR;
			break;
		case EAGAIN:
			rc = ERR_SFULL;
			break;
		case EFAULT:
			rc = ERR_PTRINV;
			break;
		case EINTR:
			rc = ERR_INTRPT;
			break;
		default:
			rc = ERR_DEFAULT;
			break;
		};
		break;

	case READ:
		switch (error_code)
		{
		case EINVAL:
			rc = ERR_INVPAR;
			break;
		case ENOMSG:
			rc = ERR_SEMPTY;
			break;
		case E2BIG:
			rc = ERR_TOOBIG;
			break;
		case EFAULT:
			rc = ERR_PTRINV;
			break;
		case EACCES:
			rc = ERR_ACCESS;
			break;
		case EINTR:
			rc = ERR_INTRPT;
			break;
		default:
			rc = ERR_DEFAULT;
			break;
		};
		break;
	}

	return(rc);
}



/* ------------------------ ipcdb_open ------------------------ */
ipcdb_open()
{
	/* Open IPC database for read/write access */
	if ((ipcdb_ptr = fopen(IPC_FILE, "r+")) == NULL)
		return(ERR_CONFIG);	/* Open failed */

	/* Get file descriptor */
	if ((ipcdb_fd = fileno(ipcdb_ptr)) == ERROR)
		return(ERR_CONFIG);

	return(SUCCESS);	/* IPC database successfully opened */
}






/* ------------------------ ipcdb_close ------------------------ */
ipcdb_close()
{
	if (fclose(ipcdb_ptr) != SUCCESS)
		return(ERROR);
	else
		return(SUCCESS);
}






/* ------------------------ ipcdb_write ------------------------ */
ipcdb_write(ipcs_id, ipcs_name_ptr, ipcp)
INT	ipcs_id;
CHAR	*ipcs_name_ptr;
UINT	ipcp;
{
	/* Position cursor to right before EOF */
	if (fseek(ipcdb_ptr, OFF_EXACT, OFF_END) != SUCCESS)
		return(ERROR);

	/* Lock file */
	if (lockf(ipcdb_fd, F_LOCK, 0) == ERROR)
		return(ERROR);

	/* Perform write */
	if(fprintf(ipcdb_ptr, "%s %hd %ho\n", ipcs_name_ptr, ipcs_id, ipcp)
								<= ERROR)
		return(ERROR);

	/* Unlock file */
	if (lockf(ipcdb_fd, F_ULOCK, 0) == ERROR)
		return(ERROR);

	return(SUCCESS);
}





/* ------------------------ ipcdb_fnd_name ------------------------ */
ipcdb_fnd_name(ipcs_name_ptr, ipcs_name_len)
CHAR	*ipcs_name_ptr;
INT	ipcs_name_len;
{
	BYTE	found = 0;			/* Found flag */
	CHAR	file_name[IPCS_NAME_MXSZ];	/* IPC structure name read */
	CHAR	*search_ptr;			/* ipcs_name_ptr passed */
	INT	ipcs_id;			/* IPC structure ID read */
	INT	ipcp;				/* Priority read */
	INT	i;				/* Loop index */
	INT	equ_nochar;			/* Equal chars counter */


	/* Position stream pointer to the beginning of IPC database file */
	if (fseek(ipcdb_ptr, 0, OFF_BEG) <= ERROR)
		return(ERR_CONFIG);

	/* Perform loop in which IPC structure names should be compared with
						name passed by caller */
	while (found == 0)
	{
		search_ptr = ipcs_name_ptr;
		if (fscanf(ipcdb_ptr, "%s%hd%ho", file_name, &ipcs_id, &ipcp)
							> SUCCESS)
		{
			fseek(ipcdb_ptr, 1, OFF_CURR);
			equ_nochar = 0;
			for (i = 0; i < ipcs_name_len; i++)
			{
				/* Compare each character */
				if (*search_ptr++ == file_name[i])
					++equ_nochar;
				else
					/* Break "if-loop" */
					i = ipcs_name_len;
			}
			if (equ_nochar == ipcs_name_len)
				found = 1;
		}
		else
			found = -1;
	}


	if (found == 1)
		return(ipcs_id);	/* Return IPC structure identifier */
	else
		return(ERR_NMINVA);	/* Name not found */
}





/* ------------------------ ipcdb_fnd_id ------------------------ */
ipcdb_fnd_id(ipcs_id, ipcs_name_ptr)
INT	ipcs_id;
CHAR	*ipcs_name_ptr;
{
	INT	id_read;
	UINT	ipcp;
	BYTE	found = 0;


	/* Position stream pointer to the beginning of IPC database file */
	if (fseek(ipcdb_ptr, 0, OFF_BEG) <= ERROR)
		return(ERR_CONFIG);

	/* Perform loop in which IPC structure names should be compared with
						name passed by caller */
	while (fscanf(ipcdb_ptr, "%s%hd%ho",
			ipcs_name_ptr, &id_read, &ipcp) > SUCCESS)
	{
		fseek(ipcdb_ptr, 1, OFF_CURR);
		if (id_read == ipcs_id)
		/* Required ID found, name has been copied by fscanf */
		{
			found = 1;
			break;
		}
	}


	if (found == 1)
		return(SUCCESS);
	else
		return(ERR_IDINVA);
}






/* ------------------------ ipcdb_delete ------------------------ */
ipcdb_delete(ipcs_name_ptr)
CHAR	*ipcs_name_ptr;
{
	BYTE	found = 0;			/* Found flag */
	CHAR	file_name[IPCS_NAME_MXSZ];	/* IPC structure name read */
	CHAR	*file_name_ptr;
	CHAR	*search_ptr;			/* ipcs_name_ptr passed */
	INT	ipcs_id;			/* IPC structure ID read */
	INT	ipcp;				/* Priority read */
	INT	i;				/* Loop index */
	INT	equ_nochar;			/* Equal chars counter */
	INT	nread;				/* # of chars read */
	CHAR	*star_char = "*";
	CHAR	*terminator = '\0';
	LONGINT	last_CR = 0;			/* offset of last <CR> */
	LONGINT	curr_CR = 0;			/* offset of current <CR> */
	INT	position = 0;



	/* Position stream pointer to the beginning of IPC database file */
	if (fseek(ipcdb_ptr, 0, OFF_BEG) <= ERROR)
		return(ERR_CONFIG);

	/* Perform loop in which IPC structure names should be compared with
						name passed by caller */
	while (found == 0)
	{
		search_ptr = ipcs_name_ptr;
		if ((nread = fscanf(ipcdb_ptr, "%s%hd%ho",
				file_name, &ipcs_id, &ipcp)) > SUCCESS)
		{
			last_CR = curr_CR;
			curr_CR = ftell(ipcdb_ptr);

			fseek(ipcdb_ptr, 1, OFF_CURR);
			equ_nochar = 0;
			for (i = 0; i < strlen(ipcs_name_ptr); i++)
			{
				/* Compare each character */
				if (*search_ptr++ == file_name[i])
					++equ_nochar;
				else
					/* Break "if-loop" */
					i = strlen(ipcs_name_ptr);
			}
			if (equ_nochar == strlen(ipcs_name_ptr))
				found = 1;
		}
		else
			found = -1;
	}


	if (found == 1)
	{
		/* Delete this entry */
		file_name_ptr = &file_name[0];

		/* Move file pointer back for nread bytes and rewrite */
		if (last_CR == 0)
		{
			nread = curr_CR;	/* # of bytes to rewrite */
			position = 0;		/* from stream beginning */
		}
		else
		{
			nread = curr_CR - last_CR;
			position = last_CR + 1;
		}

		fseek(ipcdb_ptr, 0, OFF_BEG);		/* Rewind streem */
		if (position != 0)
			/* Position to name to del. */
			fseek(ipcdb_ptr, position, OFF_BEG);

		/* Lock IPC database while deleteng */
		if (lockf(ipcdb_fd, F_LOCK, 0) == ERROR)
			return(ERR_CONFIG);

		/* Set up deleted entry */
		for (i = 0; i < (nread - 6); i++)
			file_name[i] = *star_char;
		file_name[(nread - 6)] = *terminator;

		if (fprintf(ipcdb_ptr, "%s %hd %ho\n",
				 file_name, ERROR, 00) <= ERROR)
		{
			lockf(ipcdb_fd, F_ULOCK, 0);
			return(ERR_CONFIG);
		}
		else
		{
			if (lockf(ipcdb_fd, F_ULOCK, 0) == ERROR)
				return(ERR_CONFIG);
			else
				return(ipcs_id);
		}
	}
	else
		return(ERR_NMINVA);
}
