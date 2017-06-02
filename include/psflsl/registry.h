#ifndef _PSFLSL_REGISTRY_H_
#define _PSFLSL_REGISTRY_H_

#include <stddef.h>

#define PSFLSL_ERR_NO_CLEAN(THE_R) do { r = (THE_R); goto noclean; } while(0)
#define PSFLSL_ERR_CLEAN(THE_R) do { r = (THE_R); goto clean; } while(0)
#define PSFLSL_GOTO_CLEAN() do { goto clean; } while(0)

#define PSFLSL_JREKEY_NAME_STR "Software\\JavaSoft\\Java Runtime Environment"

enum PsflslBitness
{
	PSFLSL_BITNESS_NONE = 0x7FFFFFFF,
	PSFLSL_BITNESS_32 = 32,
	PSFLSL_BITNESS_64 = 64,
};

int psflsl_jvmdll_check_jrekeyname(
	enum PsflslBitness Bitness,
	const char *JreKeyName,
	char *JvmDllPathBuf,
	size_t JvmDllPathSize,
	size_t *oLenJvmDllPath,
	bool *oHaveValue);
int psflsl_jvmdll_check(
	size_t NumBitnessCheckOrder,
	enum PsflslBitness *BitnessCheckOrder,
	char *JvmDllPathBuf,
	size_t JvmDllPathSize,
	size_t *oLenJvmDllPath,
	enum PsflslBitness *oHaveBitness);

#endif /* _PSFLSL_REGISTRY_H_ */
