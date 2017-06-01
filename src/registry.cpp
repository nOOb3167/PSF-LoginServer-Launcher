#include <cstddef>
#include <cstring>

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include <windows.h>

#include <psflsl/registry.h>

/*
= FIXME: windows registry virtualization =
https://stackoverflow.com/questions/252297/why-is-regopenkeyex-returning-error-code-2-on-vista-64bit/291067#291067
  Note that you should not rely on the key being called "Wow6432Node"
  Access the other registry view using the flags to RegOpenKeyEx instead
*/

static int psflsl_strtol_prefix(const char *Str, size_t LenStr, size_t *oVal);
static int psflsl_jvmdll_get_value(
	HKEY JreKey,
	const char *ValueNameStr,
	BYTE *ioJreValBuf,
	DWORD JreValSize,
	DWORD *oLenJreVal,
	bool *oHaveValue);
static int psflsl_jvmdll_get_key_hklm_opt(
	const char *JreKeyName,
	HKEY *oJreKeyOpt);
static int psflsl_jvmdll_get_sub_value_runtime_lib(
	HKEY JreKey,
	BYTE *SubValBuf,
	DWORD LenSubVal,
	BYTE *LibValBuf,
	DWORD LibValSize,
	DWORD *oLenLibVal,
	bool *oHaveValue);
static int psflsl_jvmdll_check_using_current_version(
	HKEY JreKey,
	char *JvmDllPathBuf,
	size_t JvmDllPathSize,
	size_t *oLenJvmDllPath,
	bool *oHaveValue);
int psflsl_jvmdll_check_using_enumerate_and_guess(
	HKEY JreKey,
	char *JvmDllPathBuf,
	size_t JvmDllPathSize,
	size_t *oLenJvmDllPath,
	bool *oHaveValue);

struct PsflslVersionComparator
{
	static void getsplit_uscore_possibly(std::string Lump, std::vector<size_t> *vec)
	{
		/* expect ".*(_number)?", accumulate number if present */
		const char *Uscore = NULL;
		size_t Number = 0;

		if ((Uscore = strchr(Lump.data(), '_')))
			if (0 == psflsl_strtol_prefix(Uscore + 1, (Lump.data() + Lump.size()) - (Uscore + 1), &Number))
				vec->push_back(Number);
	}

	static int getsplit(std::stringstream *ss, std::vector<size_t> *vec)
	{
		std::string Lump;

		vec->clear();
		while (std::getline(*ss, Lump, '.')) {
			size_t Number = 0;
			if (!!psflsl_strtol_prefix(Lump.data(), Lump.size(), &Number))
				return 1;
			vec->push_back(Number);
		}
		/* see if we can parse an extra number out of the last Lump ever getline-d */
		if (! vec->empty())
			getsplit_uscore_possibly(Lump, vec);
		
		return 0;
	}

	bool operator()(const std::string &a, const std::string &b) const
	{
		// FIXME: java version format "number(.number)*(_number)?" - _number not parsed yet
		/* the operation is 'greater than' */
		std::stringstream ssA(a), ssB(b);
		std::vector<size_t> verA, verB;
		/* get numeric components - refuse to order on failure */
		if (!!getsplit(&ssA, &verA))
			return false;
		if (!!getsplit(&ssB, &verB))
			return false;
		/* compare by component - while component present in both */
		for (size_t i = 0; i < verA.size() && i < verB.size(); i++) {
			if (verA[i] == verB[i])
				continue;
			return verA[i] > verB[i];
		}
		/* break ties by length */
		return verA.size() > verB.size();
	}
};

int psflsl_strtol_prefix(const char *Str, size_t LenStr, size_t *oVal)
{
	char *end = NULL;
	unsigned long long valLL = 0;
	errno = 0;
	valLL = strtoull(Str, &end, 10);
	if (errno == ERANGE && (valLL == ULLONG_MAX))
		return 1;
	if (errno == EINVAL)
		return 1;
	/* avoid erroring out if not all input consumed */
	//if (end != Str + LenStr)
	//	return 1;
	if (oVal)
		*oVal = valLL;
	return 0;
}

int psflsl_jvmdll_get_value(
	HKEY JreKey,
	const char *ValueNameStr,
	BYTE *ioJreValBuf,
	DWORD JreValSize,
	DWORD *oLenJreVal,
	bool *oHaveValue)
{
	int r = 0;

	bool HaveValue = true;
	LONG Ret = 0;

	DWORD Tmp = JreValSize;

	Ret = RegQueryValueEx(JreKey, ValueNameStr, NULL, NULL, ioJreValBuf, &Tmp);
	if (Ret == ERROR_FILE_NOT_FOUND)
		HaveValue = false;
	else if (Ret != ERROR_SUCCESS)
		PSFLSL_ERR_CLEAN(1);

	if (!(Tmp < JreValSize))
		PSFLSL_ERR_CLEAN(1);
	ioJreValBuf[Tmp] = '\0';

	if (oLenJreVal)
		*oLenJreVal = Tmp;

	if (oHaveValue)
		*oHaveValue = HaveValue;

clean:

	return r;
}

int psflsl_jvmdll_get_key_hklm_opt(
	const char *JreKeyName,
	HKEY *oJreKeyOpt)
{
	int r = 0;

	LONG Ret = 0;
	HKEY JreKey = NULL;

	Ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, JreKeyName, 0, KEY_READ, &JreKey);
	if (Ret == ERROR_FILE_NOT_FOUND)
		PSFLSL_ERR_NO_CLEAN(0);
	else if (Ret != ERROR_SUCCESS)
		PSFLSL_ERR_CLEAN(1);

noclean:
	if (oJreKeyOpt)
		*oJreKeyOpt = JreKey;

clean:

	return r;
}

int psflsl_jvmdll_get_sub_value_runtime_lib(
	HKEY JreKey,
	BYTE *SubValBuf,
	DWORD LenSubVal,
	BYTE *LibValBuf,
	DWORD LibValSize,
	DWORD *oLenLibVal,
	bool *oHaveValue)
{
	int r = 0;

	const char *SubValBufStr = (const char *) SubValBuf;

	HKEY JreKeySub = NULL;

	if (ERROR_SUCCESS != RegOpenKeyEx(JreKey, SubValBufStr, 0, KEY_READ, &JreKeySub))
		PSFLSL_ERR_CLEAN(1);

	if (!!(r = psflsl_jvmdll_get_value(
		JreKeySub,
		"RuntimeLib",
		LibValBuf, LibValSize, oLenLibVal, oHaveValue)))
	{
		PSFLSL_GOTO_CLEAN();
	}

clean:
	if (JreKeySub)
		RegCloseKey(JreKeySub);

	return r;
}

int psflsl_jvmdll_check_using_current_version(
	HKEY JreKey,
	char *JvmDllPathBuf,
	size_t JvmDllPathSize,
	size_t *oLenJvmDllPath,
	bool *oHaveValue)
{
	int r = 0;

	BYTE SubValBuf[512] = {};
	DWORD SubValSize = sizeof SubValBuf;
	DWORD LenSubVal = 0;

	bool HaveValueCurrentVersion = false;
	bool HaveValueRuntimeLib = false;

	if (!!(r = psflsl_jvmdll_get_value(
		JreKey,
		"CurrentVersion",
		SubValBuf, SubValSize, &LenSubVal,
		&HaveValueCurrentVersion)))
	{
		PSFLSL_GOTO_CLEAN();
	}

	if (HaveValueCurrentVersion) {
		if (!!(r = psflsl_jvmdll_get_sub_value_runtime_lib(
			JreKey,
			SubValBuf, LenSubVal,
			(PBYTE)JvmDllPathBuf, JvmDllPathSize, (PDWORD)&oLenJvmDllPath,
			&HaveValueRuntimeLib)))
		{
			PSFLSL_GOTO_CLEAN();
		}
	}

	if (oHaveValue)
		*oHaveValue = HaveValueRuntimeLib;

clean:

	return r;
}

int psflsl_jvmdll_check_using_enumerate_and_guess(
	HKEY JreKey,
	char *JvmDllPathBuf,
	size_t JvmDllPathSize,
	size_t *oLenJvmDllPath,
	bool *oHaveValue)
{
	int r = 0;

	std::vector<std::string> Names;

	DWORD Idx = 0;
	DWORD Ret = ERROR_SUCCESS;

	char NameBuf[512];
	DWORD NameBufSpecialLen = 0;

	while (true) {
		NameBufSpecialLen = sizeof NameBuf;
		Ret = RegEnumKeyEx(
			JreKey,
			Idx++,
			NameBuf,
			&NameBufSpecialLen,
			NULL,
			NULL,
			NULL,
			NULL);
		if (Ret == ERROR_NO_MORE_ITEMS)
			break;
		else if (Ret != ERROR_SUCCESS)
			PSFLSL_ERR_CLEAN(1);
		Names.push_back(std::string(NameBuf, NameBufSpecialLen));
	}

	std::sort(Names.begin(), Names.end(), PsflslVersionComparator());

clean:

	return r;
}

int psflsl_jvmdll_check_jrekeyname(
	const char *JreKeyName,
	char *JvmDllPathBuf,
	size_t JvmDllPathSize,
	size_t *oLenJvmDllPath,
	bool *oHaveValue)
{
	int r = 0;

	HKEY JreKey = NULL;

	bool HaveValue = false;

	if (!!(r = psflsl_jvmdll_get_key_hklm_opt(JreKeyName, &JreKey)))
		PSFLSL_GOTO_CLEAN();

	if (!JreKey)
		PSFLSL_ERR_NO_CLEAN(0);

	if (!!(r = psflsl_jvmdll_check_using_current_version(
		JreKey,
		JvmDllPathBuf, JvmDllPathSize, oLenJvmDllPath,
		&HaveValue)))
	{
		PSFLSL_GOTO_CLEAN();
	}

	if (HaveValue)
		PSFLSL_ERR_NO_CLEAN(0);

	if (!!(r = psflsl_jvmdll_check_using_enumerate_and_guess(
		JreKey,
		JvmDllPathBuf, JvmDllPathSize, oLenJvmDllPath,
		&HaveValue)))
	{
		PSFLSL_GOTO_CLEAN();
	}

noclean:
	if (oHaveValue)
		*oHaveValue = HaveValue;

clean:
	if (JreKey)
		RegCloseKey(JreKey);

	return r;
}

int psflsl_jvmdll_check(
	char *JvmDllPathBuf,
	size_t JvmDllPathSize,
	size_t *LenJvmDllPath,
	bool *oHaveValue)
{
	int r = 0;

	const char JreKeyNameRegular[] = PSFLSL_JREKEY_NAME_REGULAR_STR;
	const char JreKeyNameWow[] = PSFLSL_JREKEY_NAME_WOW_STR;

	bool HaveValue = false;

	if (!!(r = psflsl_jvmdll_check_jrekeyname(
		JreKeyNameRegular,
		JvmDllPathBuf,
		JvmDllPathSize,
		LenJvmDllPath,
		&HaveValue)))
	{
		PSFLSL_GOTO_CLEAN();
	}

	if (! HaveValue) {
		if (!!(r = psflsl_jvmdll_check_jrekeyname(
			JreKeyNameWow,
			JvmDllPathBuf,
			JvmDllPathSize,
			LenJvmDllPath,
			&HaveValue)))
		{
			PSFLSL_GOTO_CLEAN();
		}
	}

	if (oHaveValue)
		*oHaveValue = HaveValue;

clean:

	return r;
}
