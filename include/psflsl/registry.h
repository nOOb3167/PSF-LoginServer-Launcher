#ifndef _PSFLSL_REGISTRY_H_
#define _PSFLSL_REGISTRY_H_

#include <stddef.h>

#define PSFLSL_JREKEY_NAME_STR "Software\\JavaSoft\\Java Runtime Environment"

enum PsflslBitness
{
	PSFLSL_BITNESS_NONE = 0x7FFFFFFF,
	PSFLSL_BITNESS_32 = 32,
	PSFLSL_BITNESS_64 = 64,
};

enum PsflslBitness psflsl_bitness_current();
enum PsflslBitness psflsl_bitness_other(enum PsflslBitness Bitness);
int psflsl_bitness_suffix_compose(
	enum PsflslBitness Bitness,
	const char *PreBuf, size_t LenPre,
	const char *PostBuf, size_t LenPost,
	char *ioSufBuf, size_t SufSize, size_t *oLenSuf);

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
