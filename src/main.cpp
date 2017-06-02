#include <cassert>
#include <cstdlib>
#include <cstddef>

#include <psflsl/registry.h>

#include <jni.h>
#include <windows.h>

/* http://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/jniTOC.html */

typedef jint (JNICALL *CreateJVMFunc)(JavaVM **pvm, void **penv, void *args);

int main(int argc, char **argv)
{
	int r = 0;

	char JvmDllPathBuf[512] = {};
	size_t JvmDllPathSize = sizeof JvmDllPathBuf;
	size_t LenJvmDllPath = 0;

	bool HaveJvmDllPath = 0;

	enum PsflslBitness BitnessCheckOrder[2] = { PSFLSL_BITNESS_64, PSFLSL_BITNESS_32 };
	enum PsflslBitness BitnessHave = PSFLSL_BITNESS_NONE;

	if (!!(r = psflsl_jvmdll_check(
		sizeof BitnessCheckOrder / sizeof *BitnessCheckOrder, BitnessCheckOrder,
		JvmDllPathBuf, JvmDllPathSize, &LenJvmDllPath,
		&BitnessHave)))
	{
		assert(0);
	}

	HMODULE jvmDll = LoadLibrary(EXTERNAL_PSFLSL_HARDCODED_JVMDLL_FILEPATH);
	assert(jvmDll);
	CreateJVMFunc CreateJVM = (CreateJVMFunc) GetProcAddress(jvmDll, "JNI_CreateJavaVM");
	assert(CreateJVM);

	/* http://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/invocation.html
	     see about extraInfo hooks (exit, abort) */
	// FIXME: https://docs.oracle.com/javase/tutorial/essential/environment/sysprop.html java.class.path platform-specific separator
	JavaVMOption vmOpts[] = {
		{ "-Djava.class.path=" EXTERNAL_PSFLSL_HARDCODED_CLASS_PATH, NULL },
	};

	JavaVM *jvm = NULL;
	JNIEnv *env = NULL;
	JavaVMInitArgs vmArgs = {};
	vmArgs.version = JNI_VERSION_1_8;
	vmArgs.nOptions = sizeof vmOpts / sizeof *vmOpts;
	vmArgs.options = vmOpts;
	vmArgs.ignoreUnrecognized = false;

	if (JNI_OK != CreateJVM(&jvm, (void **)&env, &vmArgs))
		assert(0);

	jclass mainClass = env->functions->FindClass(env, "Main");
	assert(mainClass);
	jmethodID mainMethod = env->functions->GetStaticMethodID(env, mainClass, "main", "([Ljava/lang/String;)V");
	env->functions->CallStaticVoidMethod(env, mainClass, mainMethod, NULL);
	assert(! env->functions->ExceptionCheck(env));

clean:
	if (jvm)
		if (JNI_OK != jvm->functions->DestroyJavaVM(jvm))
			assert(0);
	if (jvmDll)
		FreeLibrary(jvmDll);

    return EXIT_SUCCESS;
}
