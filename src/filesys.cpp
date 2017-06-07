#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif /* _MSC_VER */

#include <cassert>
#include <cstring>

#include <windows.h>
#include <shlwapi.h> // PathAppend etc

#include <psflsl/misc.h>
#include <psflsl/filesys.h>

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

int psflsl_build_path_interpret_relative_current_executable(
	const char *PossiblyRelativePathBuf, size_t LenPossiblyRelativePath,
	char *ioPathBuf, size_t PathBufSize, size_t *oLenPathBuf)
{
	int r = 0;

	size_t PossiblyRelativePathIsAbsolute = 0;

	if (!!(r = psflsl_win_path_is_absolute(
		PossiblyRelativePathBuf, LenPossiblyRelativePath,
		&PossiblyRelativePathIsAbsolute)))
	{
		PSFLSL_GOTO_CLEAN();
	}

	if (PossiblyRelativePathIsAbsolute) {

		if (!!(r = psflsl_buf_copy_zero_terminate(
			PossiblyRelativePathBuf, LenPossiblyRelativePath,
			ioPathBuf, PathBufSize, oLenPathBuf)))
		{
			PSFLSL_GOTO_CLEAN();
		}

	} else {

		if (!!(r = psflsl_build_current_executable_relative_filename(
			PossiblyRelativePathBuf, LenPossiblyRelativePath,
			ioPathBuf, PathBufSize, oLenPathBuf)))
		{
			PSFLSL_GOTO_CLEAN();
		}

	}

clean:

	return r;
}

int psflsl_build_current_executable_relative_filename(
	const char *RelativeBuf, size_t LenRelative,
	char *ioCombinedBuf, size_t CombinedBufSize, size_t *oLenCombined)
{
	int r = 0;

	size_t LenPathCurrentExecutableDir = 0;
	char PathCurrentExecutableDirBuf[512] = {};
	size_t LenPathModification = 0;
	char PathModificationBuf[512] = {};

	/* get directory */
	if (!!(r = psflsl_get_current_executable_directory(
		PathCurrentExecutableDirBuf, sizeof PathCurrentExecutableDirBuf, &LenPathCurrentExecutableDir)))
	{
		PSFLSL_ERR_CLEAN(1);
	}

	/* ensure relative and append */

	if (!!(r = psflsl_win_path_append_abs_rel(
		PathCurrentExecutableDirBuf, LenPathCurrentExecutableDir,
		RelativeBuf, LenRelative,
		PathModificationBuf, sizeof PathModificationBuf, &LenPathModification)))
	{
		PSFLSL_GOTO_CLEAN();
	}

	/* canonicalize into output */

	if (!!(r = psflsl_win_path_canonicalize(
		PathModificationBuf, LenPathModification,
		ioCombinedBuf, CombinedBufSize, oLenCombined)))
	{
		PSFLSL_GOTO_CLEAN();
	}

clean:

	return r;
}

int psflsl_win_file_exist_ensure(const char *FileNameBuf, size_t LenFileName)
{
	int r = 0;

	size_t IsExist = 0;

	if (!!(r = psflsl_win_file_exist(FileNameBuf, LenFileName, &IsExist)))
		PSFLSL_GOTO_CLEAN();

	if (! IsExist)
		PSFLSL_ERR_CLEAN(1);

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

int psflsl_win_path_is_absolute(const char *PathBuf, size_t LenPath, size_t *oIsAbsolute)
{
	int r = 0;

	size_t IsAbsolute = false;

	if (!!(r = psflsl_buf_strnlen(PathBuf, LenPath + 1, NULL)))
		PSFLSL_GOTO_CLEAN();

	/* maximum length for PathIsRelative */
	if (LenPath > MAX_PATH)
		PSFLSL_ERR_CLEAN(1);

	IsAbsolute = ! PathIsRelative(PathBuf);

	if (oIsAbsolute)
		*oIsAbsolute = IsAbsolute;

clean:

	return r;
}

int psflsl_win_path_canonicalize(
	const char *InputPathBuf, size_t LenInputPath,
	char *ioOutputPathBuf, size_t OutputPathBufSize, size_t *oLenOutputPath)
{
	int r = 0;

	/** required length for PathCanonicalize **/
	if (OutputPathBufSize < MAX_PATH || LenInputPath > MAX_PATH)
		PSFLSL_ERR_CLEAN(1);

	/** this does fucking nothing (ex retains mixed slash backslash) **/
	if (! PathCanonicalize(ioOutputPathBuf, InputPathBuf))
		PSFLSL_ERR_CLEAN(1);

	if (!!(r = psflsl_buf_strnlen(ioOutputPathBuf, OutputPathBufSize, oLenOutputPath)))
		PSFLSL_GOTO_CLEAN();

clean:

	return r;
}

int psflsl_win_path_append_abs_rel(
	const char *AbsoluteBuf, size_t LenAbsolute,
	const char *RelativeBuf, size_t LenRelative,
	char *ioOutputPathBuf, size_t OutputPathBufSize, size_t *oLenOutputPath)
{
	int r = 0;

	size_t LenOutputPathTmp = 0;

	/** maximum length for PathIsRelative and PathAppend **/
	if (LenAbsolute > MAX_PATH || LenRelative > MAX_PATH)
		PSFLSL_ERR_CLEAN(1);

	if (PathIsRelative(AbsoluteBuf))
		PSFLSL_GOTO_CLEAN();

	if (! PathIsRelative(RelativeBuf))
		PSFLSL_GOTO_CLEAN();

	/* prep output buffer with absolute path */

	if (!!(r = psflsl_buf_copy_zero_terminate(
		AbsoluteBuf, LenAbsolute,
		ioOutputPathBuf, OutputPathBufSize, &LenOutputPathTmp)))
	{
		PSFLSL_GOTO_CLEAN();
	}

	/* append */

	if (! PathAppend(ioOutputPathBuf, RelativeBuf))
		PSFLSL_ERR_CLEAN(1);

	if (!!(r = psflsl_buf_strnlen(ioOutputPathBuf, OutputPathBufSize, oLenOutputPath)))
		PSFLSL_GOTO_CLEAN();

clean:

	return r;
}

int psflsl_get_current_executable_filename(char *ioFileNameBuf, size_t FileNameSize, size_t *oLenFileName)
{
	int r = 0;

	DWORD dwFileNameSize = (DWORD) FileNameSize;
	DWORD LenFileName = 0;

	LenFileName = GetModuleFileName(NULL, ioFileNameBuf, dwFileNameSize);
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
