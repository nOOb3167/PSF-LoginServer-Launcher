#ifndef _PSFLSL_CONFIG_H_
#define _PSFLSL_CONFIG_H_

#include <stddef.h>
#include <stdint.h>

#define PSFLSL_CONFIG_DEFAULT_RELATIVE_PATHNAME "."
#define PSFLSL_CONFIG_DEFAULT_RELATIVE_FILENAME "PSF-LoginServer-Launcher-Config.txt"

struct PsflslConfMap;

/** value struct
    manual-init struct
*/
struct PsflslAuxConfigCommonVars
{
	char *HardCodedPathSeparatorBuf; size_t LenHardCodedPathSeparator;
	char *HardCodedClassPathBuf; size_t LenHardCodedClassPath;
	char *HardCodedClassPath2Buf; size_t LenHardCodedClassPath2;
	char *HardCodedJavaOptsBuf; size_t LenHardCodedJavaOpts;
	char *JavaDebugOptsBuf; size_t LenJavaDebugOpts;
	size_t JavaDebugOptsEnabled;
	char *JavaMainClassBuf; size_t LenJavaMainClass;
	char *JavaFallbackJvmDllBuf; size_t LenJavaFallbackJvmDll;
	char *JavaFallbackJvmDllPreferOverForkingBuf; size_t LenJavaFallbackJvmDllPreferOverForking;
};

int psflsl_conf_map_create(PsflslConfMap **oConfMap);
int psflsl_conf_map_destroy(PsflslConfMap *ConfMap);

int psflsl_config_parse_find_next_newline(const char *DataStart, uint32_t DataLength, uint32_t Offset, uint32_t *OffsetNew);
int psflsl_config_parse_skip_newline(const char *DataStart, uint32_t DataLength, uint32_t Offset, uint32_t *OffsetNew);
int psflsl_config_parse(
	const char *BufferBuf, size_t LenBuffer,
	PsflslConfMap **oKeyVal);

const char * psflsl_config_key(const PsflslConfMap *KeyVal, const char *Key);
int psflsl_config_key_uint32(const PsflslConfMap *KeyVal, const char *Key, uint32_t *oVal);

int psflsl_config_read_fullpath(
	const char *PathFullBuf, size_t LenPathFull,
	PsflslConfMap **oKeyVal);
int psflsl_config_read_builtin(PsflslConfMap **oKeyVal);
int psflsl_config_read_builtin_or_relative_current_executable(
	const char *ExpectedLocationBuf, size_t LenExpectedLocation,
	const char *ExpectedNameBuf, size_t LenExpectedName,
	PsflslConfMap **oKeyVal);
int psflsl_config_read_default_everything(PsflslConfMap **oKeyVal);

int psflsl_config_get_common_vars(
	PsflslConfMap *KeyVal,
	PsflslAuxConfigCommonVars *oCommonVars);

#endif /* _PSFLSL_CONFIG_H_ */
