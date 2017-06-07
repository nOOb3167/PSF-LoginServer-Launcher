#ifndef _PSFLSL_MISC_H_
#define _PSFLSL_MISC_H_

#include <stddef.h>

/* nulling destruction - DELETER(P) */
#define PSFLSL_DELETE_F(PTR_PTR_VARNAME, FNAME)            \
  do {                                                     \
    decltype(PTR_PTR_VARNAME) ptr_ptr = (PTR_PTR_VARNAME); \
	if (*ptr_ptr) {                                        \
      if (!!((FNAME)(*ptr_ptr))) assert(0);                \
      *ptr_ptr = NULL;                                     \
	}                                                      \
  } while (0)

#define PSFLSL_ERR_NO_CLEAN(THE_R) do { r = (THE_R); goto noclean; } while(0)
#define PSFLSL_ERR_CLEAN(THE_R) do { r = (THE_R); goto clean; } while(0)
#define PSFLSL_GOTO_CLEAN() do { goto clean; } while(0)

/* WARNING: evaluates arguments multiple times. rework using block with decltype assignment. */
#define PSFLSL_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define PSFLSL_MIN(x, y) (((x) < (y)) ? (x) : (y))

void psflsl_debug_break();

int psflsl_buf_ensure_haszero(const char *Buf, size_t BufSize);
int psflsl_buf_copy_zero_terminate(
	const char *SrcBuf, size_t LenSrc,
	char *ioDstBuf, size_t DstBufSize, size_t *oLenDst);

int psflsl_buf_strnlen(const char *Buf, size_t BufSize, size_t *oLenBufOpt);

#endif /* _PSFLSL_MISC_H_ */
