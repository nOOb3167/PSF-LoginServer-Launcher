#ifndef _PSFLSL_FILESYS_H_
#define _PSFLSL_FILESYS_H_

#include <stddef.h>

int psflsl_build_modified_filename(
	const char *BaseFileNameBuf, size_t LenBaseFileName,
	const char *ExpectedSuffix, size_t LenExpectedSuffix,
	const char *ExpectedExtension, size_t LenExpectedExtension,
	const char *ExtraSuffix, size_t LenExtraSuffix,
	const char *ExtraExtension, size_t LenExtraExtension,
	char *ioModifiedFileNameBuf, size_t ModifiedFileNameSize, size_t *oLenModifiedFileName);
int psflsl_build_path_interpret_relative_current_executable(
	const char *PossiblyRelativePathBuf, size_t LenPossiblyRelativePath,
	char *ioPathBuf, size_t PathBufSize, size_t *oLenPathBuf);
int psflsl_build_current_executable_relative_filename(
	const char *RelativeBuf, size_t LenRelative,
	char *ioCombinedBuf, size_t CombinedBufSize, size_t *oLenCombined);
int psflsl_build_path_expand_separated(
	const char *PathBuf, size_t LenPath,
	const char *ExtBuf, size_t LenExt,
	const char *SeparatorBuf, size_t LenSeparator,
	char *ExpandedBuf, size_t ExpandedSize, size_t *oLenExpanded);

int psflsl_win_file_exist_ensure(const char *FileNameBuf, size_t LenFileName);
int psflsl_win_file_exist(
	const char *FileNameBuf, size_t LenFileName,
	size_t *oIsExist);
int psflsl_win_path_directory(
	const char *InputPathBuf, size_t LenInputPath,
	char *ioOutputPathBuf, size_t OutputPathBufSize, size_t *oLenOutputPath);
int psflsl_win_path_is_absolute(const char *PathBuf, size_t LenPath, size_t *oIsAbsolute);
int psflsl_win_path_canonicalize(
	const char *InputPathBuf, size_t LenInputPath,
	char *ioOutputPathBuf, size_t OutputPathBufSize, size_t *oLenOutputPath);
int psflsl_win_path_append_abs_rel(
	const char *AbsoluteBuf, size_t LenAbsolute,
	const char *RelativeBuf, size_t LenRelative,
	char *ioOutputPathBuf, size_t OutputPathBufSize, size_t *oLenOutputPath);

int psflsl_get_current_executable_filename(char *ioFileNameBuf, size_t FileNameSize, size_t *oLenFileName);
int psflsl_get_current_executable_directory(
	char *ioCurrentExecutableDirBuf, size_t CurrentExecutableDirSize, size_t *oLenCurrentExecutableDir);

#endif /* _PSFLSL_FILESYS_H_ */
