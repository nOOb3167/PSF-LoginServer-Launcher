#ifndef _PSFLSL_REGISTRY_H_
#define _PSFLSL_REGISTRY_H_

#include <stddef.h>

#define PSFLSL_ERR_NO_CLEAN(THE_R) do { r = (THE_R); goto noclean; } while(0)
#define PSFLSL_ERR_CLEAN(THE_R) do { r = (THE_R); goto clean; } while(0)
#define PSFLSL_GOTO_CLEAN() do { goto clean; } while(0)

#define PSFLSL_JREKEY_NAME_REGULAR_STR "Software\\JavaSoft\\Java Runtime Environment"
#define PSFLSL_JREKEY_NAME_WOW_STR     "Software\\Wow6432Node\\JavaSoft\\Java Runtime Environment"

int psflsl_jvmdll_check_jrekeyname(
	const char *JreKeyName,
	char *JvmDllPathBuf,
	size_t JvmDllPathSize,
	size_t *oLenJvmDllPath,
	bool *oHaveValue);
int psflsl_jvmdll_check(
	char *JvmDllPathBuf,
	size_t JvmDllPathSize,
	size_t *LenJvmDllPath,
	bool *oHaveValue);

#endif /* _PSFLSL_REGISTRY_H_ */
