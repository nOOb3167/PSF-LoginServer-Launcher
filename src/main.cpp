#include <cassert>
#include <cstdlib>
#include <cstddef>

#include <windows.h>

#define PSFLSL_ERR_CLEAN(THE_R) do { r = (THE_R); goto clean; } while(0)
#define PSFLSL_GOTO_CLEAN() do { goto clean; } while(0)

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

int psflsl_jvmdll_check()
{
	int r = 0;

	const char JreKeyName[] = "Software\\Wow6432Node\\JavaSoft\\Java Runtime Environment";
	HKEY JreKey = NULL;

	BYTE SubValBuf[512] = {};
	DWORD SubValSize = sizeof SubValBuf;
	DWORD LenSubVal = 0;

	bool HaveValueCurrentVersion = false;

	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, JreKeyName, 0, KEY_READ, &JreKey))
		PSFLSL_ERR_CLEAN(1);

	if (!!(r = psflsl_jvmdll_get_value(
		JreKey,
		"CurrentVersion",
		SubValBuf, SubValSize, &LenSubVal,
		&HaveValueCurrentVersion)))
	{
		PSFLSL_GOTO_CLEAN();
	}

	if (HaveValueCurrentVersion) {
		BYTE LibValBuf[512] = {};
		DWORD LibValSize = sizeof LibValBuf;
		DWORD LenLibVal = 0;
		bool HaveValueRuntimeLib = false;

		if (!!(r = psflsl_jvmdll_get_sub_value_runtime_lib(
			JreKey,
			SubValBuf, LenSubVal,
			LibValBuf, LibValSize, &LenLibVal,
			&HaveValueRuntimeLib)))
		{
			PSFLSL_GOTO_CLEAN();
		}

		if (HaveValueRuntimeLib) {

		}

	}

clean:
	if (JreKey)
		RegCloseKey(JreKey);

	return r;
}

int main(int argc, char **argv)
{
	int r = 0;

	if (!!(r = psflsl_jvmdll_check())) {
		assert(0);
		return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}
