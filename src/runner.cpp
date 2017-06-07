#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif /* _MSC_VER */

#include <cassert>
#include <cstdlib>
#include <cstddef>

#include <jni.h>
#include <windows.h>

#include <psflsl/misc.h>
#include <psflsl/filesys.h>
#include <psflsl/registry.h>
#include <psflsl/runner.h>

/* http://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/jniTOC.html */

typedef jint(JNICALL *CreateJVMFunc)(JavaVM **pvm, void **penv, void *args);

void psflsl_close_handle(HANDLE handle);

void psflsl_close_handle(HANDLE handle)
{
	if (handle)
		if (!CloseHandle(handle))
			assert(0);
}

int psflsl_process_start(
	const char *FileNameParentBuf, size_t LenFileNameParent,
	const char *ParentCommandLineBuf, size_t LenParentCommandLine)
{
	/* create a process and discard all the handles (process and thread handles) */
	int r = 0;

	STARTUPINFO si = {};
	PROCESS_INFORMATION pi = {};
	HANDLE hChildProcess = NULL;
	HANDLE hChildThread = NULL;

	/* https://msdn.microsoft.com/en-us/library/windows/desktop/ms682425(v=vs.85).aspx
	*    32768 actually */
	const size_t MagicCommandLineLenghtLimit = 32767;
	const size_t ReasonableCommandLineLengthLimit = 1024;
	char CommandLineCopyBuf[ReasonableCommandLineLengthLimit];

	BOOL Ok = 0;

	if (LenParentCommandLine >= ReasonableCommandLineLengthLimit)
		PSFLSL_ERR_CLEAN(1);

	memcpy(CommandLineCopyBuf, ParentCommandLineBuf, LenParentCommandLine);
	memset(CommandLineCopyBuf + LenParentCommandLine, '\0', 1);

	if (!!(r = psflsl_win_file_exist_ensure(FileNameParentBuf, LenFileNameParent)))
		PSFLSL_GOTO_CLEAN();

	ZeroMemory(&si, sizeof si);
	si.cb = sizeof si;
	ZeroMemory(&pi, sizeof pi);

	if (!(Ok = CreateProcess(
		FileNameParentBuf,
		CommandLineCopyBuf,
		NULL,
		NULL,
		TRUE,
		0, /* CREATE_NEW_CONSOLE - meh it closes on quit */
		NULL,
		NULL,
		&si,
		&pi)))
	{
		PSFLSL_ERR_CLEAN(1);
	}
	hChildProcess = pi.hProcess;
	hChildThread = pi.hThread;

clean:
	psflsl_close_handle(hChildThread);

	psflsl_close_handle(hChildProcess);

	return r;
}

int psflsl_runner_fork_build_file_name(
	enum PsflslBitness BitnessCurrent,
	enum PsflslBitness BitnessWantedForReinvoke,
	const char *CurrentExecutableBuf, size_t LenCurrentExecutable,
	char *ioReinvokeBuf, size_t ReinvokeSize, size_t *oLenReinvoke)
{
	int r = 0;

	// FIXME: kind-of hardcoded
	const char PreBuf[] = "_x";
	size_t LenPre = (sizeof PreBuf) - 1;
	const char PostBuf[] = ".exe";
	size_t LenPost = (sizeof PostBuf) - 1;

	char SufCheckBuf[512] = {};
	size_t SufCheckSize = sizeof SufCheckBuf;
	size_t LenSufCheck = 0;

	char SufBuf[512] = {};
	size_t SufSize = sizeof SufBuf;
	size_t LenSuf = 0;

	if (!!(r = psflsl_bitness_suffix_compose(
		BitnessCurrent,
		PreBuf, LenPre,
		PostBuf, LenPost,
		SufCheckBuf, SufCheckSize, &LenSufCheck)))
	{
		PSFLSL_GOTO_CLEAN();
	}

	if (!!(r = psflsl_bitness_suffix_compose(
		BitnessWantedForReinvoke,
		PreBuf, LenPre,
		PostBuf, LenPost,
		SufBuf, SufSize, &LenSuf)))
	{
		PSFLSL_GOTO_CLEAN();
	}

	if (!!(r = psflsl_build_modified_filename(
		CurrentExecutableBuf, LenCurrentExecutable,
		SufCheckBuf, LenSufCheck,
		SufCheckBuf, LenSufCheck,
		"", 0,
		SufBuf, LenSuf,
		ioReinvokeBuf, ReinvokeSize, oLenReinvoke)))
	{
		PSFLSL_GOTO_CLEAN();
	}

clean:

	return r;
}

int psflsl_runner_fork_build_command_line(
	char *ReinvokeBuf, size_t LenReinvoke,
	char *JvmDllPathBuf, size_t LenJvmDllPath,
	char *ioCommandLineBuf, size_t CommandLineSize, size_t *oLenCommandLine)
{
	int r = 0;

	const char Arg1[] = "--forked";
	size_t LenArg1 = (sizeof Arg1) - 1;

	size_t LenAll = LenReinvoke + 1 /*space*/ +
		LenArg1                 + 1 /*space*/ + 1 /*quote*/ +
		LenJvmDllPath           + 1 /*quote*/ + 1 /*zero*/;

	if (CommandLineSize < LenAll)
		PSFLSL_ERR_CLEAN(1);

	{
		size_t O0 = 0;
		size_t O1 = O0 + LenReinvoke;
		size_t O2 = O1 + 1;
		size_t O3 = O2 + LenArg1;
		size_t O4 = O3 + 1;
		size_t O5 = O4 + 1;
		size_t O6 = O5 + LenJvmDllPath;
		size_t O7 = O6 + 1;
		size_t O8 = O7 + 1;

		assert(LenAll == O8);

		memcpy(ioCommandLineBuf + O0, ReinvokeBuf, LenReinvoke);
		memset(ioCommandLineBuf + O1, ' ', 1);
		memcpy(ioCommandLineBuf + O2, Arg1, LenArg1);
		memset(ioCommandLineBuf + O3, ' ', 1);
		memset(ioCommandLineBuf + O4, '\"', 1);
		memcpy(ioCommandLineBuf + O5, JvmDllPathBuf, LenJvmDllPath);
		memset(ioCommandLineBuf + O6, '\"', 1);
		memset(ioCommandLineBuf + O7, '\0', 1);
	}

	if (oLenCommandLine)
		*oLenCommandLine = LenAll - 1; /* exclude the zero null-terminator */

clean:

	return r;
}

int psflsl_runner_run(
	enum PsflslBitness BitnessCurrent,
	enum PsflslBitness BitnessHave,
	char *JvmDllPathBuf, size_t LenJvmDllPath,
	char *HardCodedClassPathBuf, size_t LenHardCodedClassPath)
{
	int r = 0;

	/* http://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/invocation.html
	   see about extraInfo hooks (exit, abort) */
	/* NOTE: https://docs.oracle.com/javase/tutorial/essential/environment/sysprop.html
	         java.class.path platform-specific separator */

	char StrJavaClassPath[] = "-Djava.class.path=";
	char VmOptJavaClassPathBuf[4096] = {};

	HMODULE jvmDll = 0;
	CreateJVMFunc CreateJVM = 0;

	JavaVM *jvm = NULL;
	JNIEnv *env = NULL;
	JavaVMInitArgs vmArgs = {};
	JavaVMOption vmOpts[1] = {};

	jclass mainClass = NULL;
	jmethodID mainMethod = NULL;

	if (sizeof StrJavaClassPath + LenHardCodedClassPath > sizeof VmOptJavaClassPathBuf)
		PSFLSL_ERR_CLEAN(1);

	if (!!(r = psflsl_buf_ensure_haszero(HardCodedClassPathBuf, LenHardCodedClassPath + 1)))
		PSFLSL_GOTO_CLEAN();

	strncat(VmOptJavaClassPathBuf, StrJavaClassPath, sizeof StrJavaClassPath);
	strncat(VmOptJavaClassPathBuf, HardCodedClassPathBuf, LenHardCodedClassPath);

	if (!(jvmDll = LoadLibrary(JvmDllPathBuf)))
		PSFLSL_ERR_CLEAN(1);

	if (!(CreateJVM = (CreateJVMFunc)GetProcAddress(jvmDll, "JNI_CreateJavaVM")))
		PSFLSL_ERR_CLEAN(1);

	vmOpts[0].optionString = VmOptJavaClassPathBuf;
	vmOpts[0].extraInfo = NULL;

	vmArgs.version = JNI_VERSION_1_8;
	vmArgs.nOptions = sizeof vmOpts / sizeof *vmOpts;
	vmArgs.options = vmOpts;
	vmArgs.ignoreUnrecognized = false;

	if (JNI_OK != CreateJVM(&jvm, (void **)&env, &vmArgs))
		assert(0);

	if (!(mainClass = env->functions->FindClass(env, "Main")))
		PSFLSL_ERR_CLEAN(1);
	
	if (!(mainMethod = env->functions->GetStaticMethodID(env, mainClass, "main", "([Ljava/lang/String;)V")))
		PSFLSL_ERR_CLEAN(1);

	env->functions->CallStaticVoidMethod(env, mainClass, mainMethod, NULL);

	if (env->functions->ExceptionCheck(env))
		PSFLSL_ERR_CLEAN(1);

clean:
	if (jvm)
		if (JNI_OK != jvm->functions->DestroyJavaVM(jvm))
			assert(0);
	if (jvmDll)
		FreeLibrary(jvmDll);

	return r;
}

int psflsl_runner_fork(
	enum PsflslBitness BitnessCurrent,
	enum PsflslBitness BitnessHave,
	char *JvmDllPathBuf, size_t LenJvmDllPath)
{
	int r = 0;

	char CurrentExecutableBuf[512] = {};
	size_t CurrentExecutableSize = sizeof CurrentExecutableBuf;
	size_t LenCurrentExecutable = 0;

	char ReinvokeBuf[512] = {};
	size_t ReinvokeSize = sizeof ReinvokeBuf;
	size_t LenReinvoke = 0;

	char CommandLineBuf[512] = {};
	size_t CommandLineSize = sizeof CommandLineBuf;
	size_t LenCommandLine = 0;

	if (!!(r = psflsl_get_current_executable_filename(
		CurrentExecutableBuf,
		CurrentExecutableSize,
		&LenCurrentExecutable)))
	{
		PSFLSL_GOTO_CLEAN();
	}

	if (!!(r = psflsl_runner_fork_build_file_name(
		BitnessCurrent,
		BitnessHave,
		CurrentExecutableBuf, LenCurrentExecutable,
		ReinvokeBuf, ReinvokeSize, &LenReinvoke)))
	{
		PSFLSL_GOTO_CLEAN();
	}

	if (!!(r = psflsl_runner_fork_build_command_line(
		ReinvokeBuf, LenReinvoke,
		JvmDllPathBuf, LenJvmDllPath,
		CommandLineBuf, CommandLineSize, &LenCommandLine)))
	{
		PSFLSL_GOTO_CLEAN();
	}

	if (!!(r = psflsl_process_start(
		ReinvokeBuf, LenReinvoke,
		CommandLineBuf, LenCommandLine)))
	{
		PSFLSL_GOTO_CLEAN();
	}

clean:

	return r;
}

int psflsl_runner_run_or_fork(
	enum PsflslBitness BitnessCurrent,
	enum PsflslBitness BitnessHave,
	char *JvmDllPathBuf, size_t LenJvmDllPath,
	char *HardCodedClassPathBuf, size_t LenHardCodedClassPath)
{
	int r = 0;

	if (BitnessCurrent == BitnessHave) {
		if (!!(r = psflsl_runner_run(
			BitnessCurrent,
			BitnessHave,
			JvmDllPathBuf, LenJvmDllPath,
			HardCodedClassPathBuf, LenHardCodedClassPath)))
		{
			PSFLSL_GOTO_CLEAN();
		}
	}
	else {
		if (!!(r = psflsl_runner_fork(
			BitnessCurrent,
			BitnessHave,
			JvmDllPathBuf, LenJvmDllPath)))
			PSFLSL_GOTO_CLEAN();
	}

clean:

	return r;
}
