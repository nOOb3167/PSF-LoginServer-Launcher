#include <cassert>
#include <cstdlib>
#include <cstddef>
#include <cstring>

#include <psflsl/config.h>
#include <psflsl/misc.h>
#include <psflsl/registry.h>
#include <psflsl/runner.h>

int main(int argc, char **argv)
{
	int r = 0;

	enum PsflslBitness BitnessCheckOrder[2] = { psflsl_bitness_current(), psflsl_bitness_other(psflsl_bitness_current()) };
	enum PsflslBitness BitnessHave = PSFLSL_BITNESS_NONE;

	struct PsflslConfMap *ConfMap = {};
	struct PsflslAuxConfigCommonVars CommonVars = {};

	if (!!(r = psflsl_config_read_default_everything(&ConfMap)))
		PSFLSL_GOTO_CLEAN();

	if (!!(r = psflsl_config_get_common_vars(ConfMap, &CommonVars)))
		PSFLSL_GOTO_CLEAN();

	if (argc > 1 && strcmp("--forked", argv[1]) == 0) {

		if (! (argc > 2))
			PSFLSL_GOTO_CLEAN();

		if (!!(r = psflsl_runner_run(
			psflsl_bitness_current(),
			psflsl_bitness_current(),
			argv[2], strlen(argv[2]),
			CommonVars.HardCodedClassPathBuf, CommonVars.LenHardCodedClassPath,
			CommonVars.HardCodedJavaOptsBuf, CommonVars.LenHardCodedJavaOpts,
			CommonVars.JavaMainClassBuf, CommonVars.LenJavaMainClass)))
		{
			PSFLSL_GOTO_CLEAN();
		}
	}
	else {
		char JvmDllPathBuf[512] = {};
		size_t JvmDllPathSize = sizeof JvmDllPathBuf;
		size_t LenJvmDllPath = 0;

		if (!!(r = psflsl_jvmdll_check(
			sizeof BitnessCheckOrder / sizeof *BitnessCheckOrder, BitnessCheckOrder,
			JvmDllPathBuf, JvmDllPathSize, &LenJvmDllPath,
			&BitnessHave)))
		{
			PSFLSL_GOTO_CLEAN();
		}

		if (!!(r = psflsl_runner_run_or_fork(
			psflsl_bitness_current(),
			BitnessHave,
			JvmDllPathBuf, LenJvmDllPath,
			CommonVars.HardCodedClassPathBuf, CommonVars.LenHardCodedClassPath,
			CommonVars.HardCodedJavaOptsBuf, CommonVars.LenHardCodedJavaOpts,
			CommonVars.JavaMainClassBuf, CommonVars.LenJavaMainClass)))
		{
			PSFLSL_GOTO_CLEAN();
		}
	}

clean:
	PSFLSL_DELETE_F(&ConfMap, psflsl_conf_map_destroy);

	if (!!r)
		assert(0);

    return !!r ? EXIT_FAILURE : EXIT_SUCCESS;
}
