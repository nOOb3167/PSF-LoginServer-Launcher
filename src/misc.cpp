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

#include <psflsl/misc.h>

void psflsl_debug_break()
{
	DebugBreak();
}

int psflsl_buf_ensure_haszero(const char *Buf, size_t BufSize)
{
	return !memchr(Buf, '\0', BufSize);
}

int psflsl_build_modified_filename(
	const char *BaseFileNameBuf, size_t LenBaseFileName,
	const char *ExpectedSuffix, size_t LenExpectedSuffix,
	const char *ExpectedExtension, size_t LenExpectedExtension,
	const char *ExtraSuffix, size_t LenExtraSuffix,
	const char *ExtraExtension, size_t LenExtraExtension,
	char *ioModifiedFileNameBuf, size_t ModifiedFileNameSize, size_t *oLenModifiedFileName)
{
	int r = 0;

	const size_t OffsetStartOfCheck = LenBaseFileName - LenExpectedSuffix;
	const size_t OffsetStartOfChange = LenBaseFileName - LenExpectedExtension;
	const size_t LenModifiedFileName = OffsetStartOfChange + LenExtraSuffix + LenExtraExtension;
    
	if (LenBaseFileName < LenExpectedSuffix)
		PSFLSL_ERR_CLEAN(1);
	if (LenExpectedSuffix < LenExpectedExtension)
		PSFLSL_ERR_CLEAN(1);

	if (strcmp(ExpectedSuffix, BaseFileNameBuf + OffsetStartOfCheck) != 0)
		PSFLSL_ERR_CLEAN(1);
	if (strcmp(ExpectedExtension, BaseFileNameBuf + OffsetStartOfChange) != 0)
		PSFLSL_ERR_CLEAN(1);

	if (ModifiedFileNameSize < OffsetStartOfChange + LenExtraSuffix + LenExtraExtension + 1 /*zero terminator*/)
		PSFLSL_ERR_CLEAN(1);

	memcpy(ioModifiedFileNameBuf, BaseFileNameBuf, OffsetStartOfChange);
	memcpy(ioModifiedFileNameBuf + OffsetStartOfChange, ExtraSuffix, LenExtraSuffix);
	memcpy(ioModifiedFileNameBuf + OffsetStartOfChange + LenExtraSuffix, ExtraExtension, LenExtraExtension);
	memset(ioModifiedFileNameBuf + OffsetStartOfChange + LenExtraSuffix + LenExtraExtension, '\0', 1);

	assert(ioModifiedFileNameBuf[LenModifiedFileName] == '\0');

	if (oLenModifiedFileName)
		*oLenModifiedFileName = LenModifiedFileName;

clean:

	return r;
}

int psflsl_win_path_directory(
	const char *InputPathBuf, size_t LenInputPath,
	char *ioOutputPathBuf, size_t OutputPathBufSize, size_t *oLenOutputPath)
{
	int r = 0;

	char Drive[_MAX_DRIVE] = {};
	char Dir[_MAX_DIR] = {};
	char FName[_MAX_FNAME] = {};
	char Ext[_MAX_EXT] = {};

	/* http://www.flounder.com/msdn_documentation_errors_and_omissions.htm
	*    see for _splitpath: """no more than this many characters will be written to each buffer""" */
	_splitpath(InputPathBuf, Drive, Dir, FName, Ext);

	if (!!(r = _makepath_s(ioOutputPathBuf, OutputPathBufSize, Drive, Dir, NULL, NULL)))
		PSFLSL_GOTO_CLEAN();

	if (!!(r = psflsl_buf_strnlen(ioOutputPathBuf, OutputPathBufSize, oLenOutputPath)))
		PSFLSL_GOTO_CLEAN();

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

int psflsl_get_current_executable_filename(char *ioFileNameBuf, size_t FileNameSize, size_t *oLenFileName)
{
	int r = 0;

	DWORD LenFileName = 0;

	LenFileName = GetModuleFileName(NULL, ioFileNameBuf, FileNameSize);
	if (!(LenFileName != 0 && LenFileName < FileNameSize))
		PSFLSL_ERR_CLEAN(1);

	if (oLenFileName)
		*oLenFileName = LenFileName;

clean:

	return r;
}

int psflsl_get_current_executable_directory(
	char *ioCurrentExecutableDirBuf, size_t CurrentExecutableDirSize, size_t *oLenCurrentExecutableDir)
{
	int r = 0;

	size_t LenCurrentExecutable = 0;
	char CurrentExecutableBuf[512] = {};

	if (!!(r = psflsl_get_current_executable_filename(
		CurrentExecutableBuf, sizeof CurrentExecutableBuf, &LenCurrentExecutable)))
	{
		PSFLSL_GOTO_CLEAN();
	}

	if (!!(r = psflsl_win_path_directory(
		CurrentExecutableBuf, LenCurrentExecutable,
		ioCurrentExecutableDirBuf, CurrentExecutableDirSize, oLenCurrentExecutableDir)))
	{
		PSFLSL_GOTO_CLEAN();
	}

clean:

	return r;
}
