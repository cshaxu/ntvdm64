# T95 S6 minimal-port compile-only observation 001

## Scope

This is the one admitted CPU3 compile-only observation for
`src/bochs/iodev/minimal_port_space.cc`. It does not compile `devices.cc` or
any batch, and it performs no link, execution, configuration, source, adapter
or OpenNT change.

The fresh analysis root is
`artifacts/analysis/t95-s6-msvc-x86-minimal-port-compile-001-20260811-001`.
The pinned source `src/bochs/config.h` remains
`A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8`.
Its copied root `bochs/config.h` is the accepted v2 CPU3 projection,
`C47030F79880C894A346F56E3AB3242924308983E00B5845EDCE849450C6C19D`.
The copied `minimal_port_space.cc` hashes to
`1EB43E4D4E66B9AAF4B4B14AF3353BF9D17EEB0EC18F7F6549812A125E310DB8`.

## Invocation and result

One preliminary directory-creation attempt used unsupported PowerShell
`New-Item -LiteralPath`; it failed before creating the root or starting a
compiler. The corrected setup used `New-Item -Path`, then copied the pinned
tree and replaced only the copied root config. It did not change the compiler
input or result count.

Exactly one BuildTools x86 compiler invocation then ran through
`VsDevCmd.bat -arch=x86 -host_arch=x64`:

```text
cl.exe /nologo /c /Gy /MT /DWIN32 /DBX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=0 /Fo"...\objects\minimal_port_space.obj" /I"...\bochs" /I"...\bochs\iodev" /I"...\bochs\instrument\stubs" "...\bochs\iodev\minimal_port_space.cc"
```

The compiler exited `0` and produced only the known projected-config
`FMT_LL` C4005 warning. The retained compile log hashes to
`02BE05A0D0E7F92F31AB697BD45DB24BB87489CFD6CBD713FBDEA2A007C439E1`.
The object hashes to
`0E397EF5B0029296142B3B6AB372A3B770DAEB2B7BC6774AD8F0D078C90858A8`.

Because that object exists, the permitted `dumpbin /symbols` observation was
captured at `logs/undefined-symbols.txt`
(`566CC68E17DEA2E4650859E3BA121464D45F1D7C28E888EB32FA99FA9DE44EDA`).
It contains five undefined entries: the CRT `operator new[]`,
`__security_check_cookie`, `__security_cookie`, and the original
`bx_devices_c::register_default_io_read_handler` and
`bx_devices_c::register_default_io_write_handler` members. These are an
object-local provider list, not an authorization to add a provider or a link
closure result.

## Non-claims

This result proves only that the isolated source compiles with the accepted
CPU3 projected configuration and the listed original header context. It does
not prove the new source is in any build recipe, that `devices.cc` is absent,
that any COMDAT is discarded, or that a link, reset, runtime, firmware, device
or OpenNT target works. No retry, link, executable, or run occurred.
