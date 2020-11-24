#ifndef _dbg_h
#define _dbg_h

#ifdef _ERR_CHECK

#define check(A, M)\
	if(!(A)){\
		fprintf(stderr,"[ERROR] %s", M); }

#define check_mem(A) check((A), "Out of memory.")

#else

#define check(A, M)
#define check_mem(A)

#endif

#endif
