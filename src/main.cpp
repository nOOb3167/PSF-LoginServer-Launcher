#include <cassert>
#include <cstdlib>
#include <cstddef>

#include <psflsl/registry.h>

int main(int argc, char **argv)
{
	int r = 0;

	char JvmDllPathBuf[512] = {};
	size_t JvmDllPathSize = sizeof JvmDllPathBuf;
	size_t LenJvmDllPath = 0;

	bool HaveJvmDllPath = 0;

	if (!!(r = psflsl_jvmdll_check(JvmDllPathBuf, JvmDllPathSize, &LenJvmDllPath, &HaveJvmDllPath)))
		assert(0);

    return EXIT_SUCCESS;
}
