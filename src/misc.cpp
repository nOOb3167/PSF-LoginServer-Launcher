#ifdef _MSC_VER
#pragma warning(disable : 4267 4102)  // conversion from size_t, unreferenced label
#endif /* _MSC_VER */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif /* _MSC_VER */


#include <cassert>
#include <cstdlib>
#include <cstring>

#include <windows.h>

#include <psflsl/filesys.h>
#include <psflsl/misc.h>

void psflsl_debug_break()
{
	DebugBreak();
}

int psflsl_buf_ensure_haszero(const char *Buf, size_t BufSize)
{
	return !memchr(Buf, '\0', BufSize);
}

int psflsl_buf_copy_zero_terminate(
	const char *SrcBuf, size_t LenSrc,
	char *ioDstBuf, size_t DstBufSize, size_t *oLenDst)
{
	int r = 0;

	if (!!(r = psflsl_buf_strnlen(SrcBuf, LenSrc + 1, NULL)))
		PSFLSL_GOTO_CLEAN();

	if (LenSrc >= DstBufSize)
		PSFLSL_ERR_CLEAN(1);

	memcpy(ioDstBuf, SrcBuf, LenSrc);
	memset(ioDstBuf + LenSrc, '\0', 1);

	if (oLenDst)
		*oLenDst = LenSrc;

clean:

	return r;
}

int psflsl_win_file_exist(
	const char *FileNameBuf, size_t LenFileName,
	size_t *oIsExist)
{
	int r = 0;

	int IsExist = 0;

	if (!!(r = psflsl_buf_ensure_haszero(FileNameBuf, LenFileName + 1)))
		PSFLSL_GOTO_CLEAN();

	/* https://blogs.msdn.microsoft.com/oldnewthing/20071023-00/?p=24713/ */
	/* INVALID_FILE_ATTRIBUTES if file does not exist, apparently */
	IsExist = !(INVALID_FILE_ATTRIBUTES == GetFileAttributes(FileNameBuf));

	if (oIsExist)
		*oIsExist = IsExist;

clean:

	return r;
}

int psflsl_buf_strnlen(const char *Buf, size_t BufSize, size_t *oLenBufOpt)
{
	size_t LenBuf = strnlen(Buf, BufSize);
	if (oLenBufOpt)
		*oLenBufOpt = LenBuf;
	return LenBuf == BufSize;
}
