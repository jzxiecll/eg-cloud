#ifndef __EG_ERROR_H__
#define __EG_ERROR_H__



enum EG_ERROR {
	/* First Generic Error codes */
	EG_SUCCESS = 0,
	EG_FAIL,     /* 1 */
	EG_E_PERM,   /* 2: Operation not permitted */
	EG_E_NOENT,  /* 3: No such file or directory */
	EG_E_SRCH,   /* 4: No such process */
	EG_E_INTR,   /* 5: Interrupted system call */
	EG_E_IO,     /* 6: I/O error */
	EG_E_NXIO,   /* 7: No such device or address */
	EG_E_2BIG,   /* 8: Argument list too long */
	EG_E_NOEXEC, /* 9: Exec format error */
	EG_E_BADF,   /* 10: Bad file number */
	EG_E_CHILD,  /* 11: No child processes */
	EG_E_AGAIN,  /* 12: Try again */
	EG_E_NOMEM,  /* 13: Out of memory */
	EG_E_ACCES,  /* 14: Permission denied */
	EG_E_FAULT,  /* 15: Bad address */
	EG_E_NOTBLK, /* 16: Block device required */
	EG_E_BUSY,   /* 17: Device or resource busy */
	EG_E_EXIST,  /* 18: File exists */
	EG_E_XDEV,   /* 19: Cross-device link */
	EG_E_NODEV,  /* 20: No such device */
	EG_E_NOTDIR, /* 21: Not a directory */
	EG_E_ISDIR,  /* 22: Is a directory */
	EG_E_INVAL,  /* 23: Invalid argument */
	EG_E_NFILE,  /* 24: File table overflow */
	EG_E_MFILE,  /* 25: Too many open files */
	EG_E_NOTTY,  /* 26: Not a typewriter */
	EG_E_TXTBSY, /* 27: Text file busy */
	EG_E_FBIG,   /* 28: File too large */
	EG_E_NOSPC,  /* 29: No space left on device */
	EG_E_SPIPE,  /* 30: Illegal seek */
	EG_E_ROFS,   /* 31: Read-only file system */
	EG_E_MLINK,  /* 32: Too many links */
	EG_E_PIPE,   /* 33: Broken pipe */
	EG_E_DOM,    /* 34: Math argument out of domain of func */
	EG_E_RANGE,  /* 35: Math result not representable */


	EG_E_CRC,    /* 36: Error in CRC check */
	EG_E_UNINIT,  /* 37: Module is not yet initialized */
	EG_E_TIMEOUT, /* 38: Timeout occurred during operation */
};

#endif /* ! __EG_ERROR_H__ */

