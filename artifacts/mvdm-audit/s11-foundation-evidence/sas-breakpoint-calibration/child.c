#include <intrin.h>
__declspec(dllexport) __declspec(noinline) void probe_break(void) { __debugbreak(); }
int main(void) { probe_break();return 0; }
