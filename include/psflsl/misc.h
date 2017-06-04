#ifndef _PSFLSL_MISC_H_
#define _PSFLSL_MISC_H_

#include <stddef.h>

#define PSFLSL_ERR_NO_CLEAN(THE_R) do { r = (THE_R); goto noclean; } while(0)
#define PSFLSL_ERR_CLEAN(THE_R) do { r = (THE_R); goto clean; } while(0)
#define PSFLSL_GOTO_CLEAN() do { goto clean; } while(0)

void psflsl_debug_break();

int psflsl_buf_ensure_haszero(const char *Buf, size_t BufSize);

int psflsl_build_modified_filename(
	const char *BaseFileNameBuf, size_t LenBaseFileName,
	const char *ExpectedSuffix, size_t LenExpectedSuffix,
	const char *ExpectedExtension, size_t LenExpectedExtension,
	const char *ExtraSuffix, size_t LenExtraSuffix,
	const char *ExtraExtension, size_t LenExtraExtension,
	char *ioModifiedFileNameBuf, size_t ModifiedFileNameSize, size_t *oLenModifiedFileName);

int psflsl_win_path_directory(
	const char *InputPathBuf, size_t LenInputPath,
	char *ioOutputPathBuf, size_t OutputPathBufSize, size_t *oLenOutputPath);

int psflsl_buf_strnlen(const char *Buf, size_t BufSize, size_t *oLenBufOpt);

int psflsl_get_current_executable_filename(char *ioFileNameBuf, size_t FileNameSize, size_t *oLenFileName);
int psflsl_get_current_executable_directory(
	char *ioCurrentExecutableDirBuf, size_t CurrentExecutableDirSize, size_t *oLenCurrentExecutableDir);

#endif /* _PSFLSL_MISC_H_ */
