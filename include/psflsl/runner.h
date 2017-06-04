#ifndef _PSFLSL_RUNNER_H_
#define _PSFLSL_RUNNER_H_

int psflsl_file_exist_ensure(const char *FileNameBuf, size_t LenFileName);
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
	char *JvmDllPathBuf, size_t LenJvmDllPath);
int psflsl_runner_run_or_fork(
	enum PsflslBitness BitnessCurrent,
	enum PsflslBitness BitnessHave,
	char *JvmDllPathBuf, size_t LenJvmDllPath);

#endif /* _PSFLSL_RUNNER_H_ */
