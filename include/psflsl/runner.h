#ifndef _PSFLSL_RUNNER_H_
#define _PSFLSL_RUNNER_H_

int psflsl_win_file_exist_ensure(const char *FileNameBuf, size_t LenFileName);
int psflsl_process_start(
	const char *FileNameParentBuf, size_t LenFileNameParent,
	const char *ParentCommandLineBuf, size_t LenParentCommandLine);
int psflsl_runner_fork_build_file_name(
	enum PsflslBitness BitnessCurrent,
	enum PsflslBitness BitnessWantedForReinvoke,
	const char *CurrentExecutableBuf, size_t LenCurrentExecutable,
	char *ioReinvokeBuf, size_t ReinvokeSize, size_t *oLenReinvoke);
int psflsl_runner_fork_build_command_line(
	char *ReinvokeBuf, size_t LenReinvoke,
	char *JvmDllPathBuf, size_t LenJvmDllPath,
	char *ioCommandLineBuf, size_t CommandLineSize, size_t *oLenCommandLine);
int psflsl_runner_run(
	enum PsflslBitness BitnessCurrent,
	enum PsflslBitness BitnessHave,
	char *JvmDllPathBuf, size_t LenJvmDllPath,
	char *HardCodedPathSeparatorBuf, size_t LenHardCodedPathSeparator,
	char *HardCodedClassPathBuf, size_t LenHardCodedClassPath,
	char *HardCodedClassPath2Buf, size_t LenHardCodedClassPath2,
	char *HardCodedJavaOptsBuf, size_t LenHardCodedJavaOpts,
	char *JavaMainClassBuf, size_t LenJavaMainClass);
int psflsl_runner_run_or_fork(
	enum PsflslBitness BitnessCurrent,
	enum PsflslBitness BitnessHave,
	char *JvmDllPathBuf, size_t LenJvmDllPath,
	char *HardCodedPathSeparatorBuf, size_t LenHardCodedPathSeparator,
	char *HardCodedClassPathBuf, size_t LenHardCodedClassPath,
	char *HardCodedClassPath2Buf, size_t LenHardCodedClassPath2,
	char *HardCodedJavaOptsBuf, size_t LenHardCodedJavaOpts,
	char *JavaMainClassBuf, size_t LenJavaMainClass,
	char *JavaFallbackJvmDllBuf, size_t LenJavaFallbackJvmDll,
	char *JavaFallbackJvmDllPreferOverForking, size_t LenJavaFallbackJvmDllPreferOverForking);

#endif /* _PSFLSL_RUNNER_H_ */
