# T198 S18 Native COMMAND Boot-File Witness 001

The existing S17 native fixture is parameterized by
`BX_NTVDM_NATIVE_BOOT_SERVICE`. The default proves `54:0C`; compiling the same
source with `0x0d` produces a distinct MSVC x64 executable whose real-mode
payload is `C4 C4 54 0D; HLT`.

The `0x0d` executable links against the same CPU5 core/mantle and S16 adapter
objects, exits zero, and reaches its post-resume HLT. This proves both existing
COMMAND boot-file services natively with no selector meaning outside `bx-vdm`.
