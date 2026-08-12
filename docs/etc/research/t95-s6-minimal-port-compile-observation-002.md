# T95 S6 minimal-port compile-only observation 002

## Scope

This is the one admitted CPU3/MSVC x86 compile-only observation of the
post-`BX-IO-030` five-member
`src/bochs/iodev/minimal_port_space.cc`. It does not compile `devices.cc` or
any other Bochs source, and it performs no link, executable, execution,
configuration, project or feature action.

The fresh analysis root is
`artifacts/analysis/t95-s6-msvc-x86-minimal-port-compile-002-20260811-001`.
The pinned source `src/bochs/config.h` remains
`A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8`. Its
copied root configuration is the accepted CPU3 projection,
`C47030F79880C894A346F56E3AB3242924308983E00B5845EDCE849450C6C19D`.
`logs/hashes.json` retains hashes for these inputs, the copied source and the
resulting object.

## Procedure and observation

Exactly one BuildTools x86 compiler invocation ran through
`VsDevCmd.bat -arch=x86 -host_arch=x64`:

```text
cl.exe /nologo /c /Gy /MT /DWIN32 /DBX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=0
  /Fo"...\\objects\\minimal_port_space.obj"
  /I"...\\bochs" /I"...\\bochs\\iodev" /I"...\\bochs\\instrument\\stubs"
  "...\\bochs\\iodev\\minimal_port_space.cc"
```

It exited `0` and emitted only the previously known projected-config `FMT_LL`
C4005 warning. Because the object exists, one permitted `dumpbin /symbols`
capture was made at `logs/undefined-symbols.txt`.

The two `register_default_io_*_handler` members now occur as local `SECT`
definitions in that object. The undefined list contains only MSVC CRT/runtime
support:

- `operator new[]` and `operator delete[]`;
- `strlen` and `strcpy`; and
- `__security_check_cookie` and `__security_cookie`.

There is no undefined Bochs, device, plugin, SIM, GUI, adapter or OpenNT
identity. This is the expected object-level effect of `BX-IO-030`.

## Interpretation and limit

The five-member port-space implementation is now demonstrably independent of
the full `devices.cc` object at the object-import level. This removes exactly
one documented retained-owner root from a future declared object model. It
does not select a CRT/link model, prove that the source is in a build recipe,
or establish COMDAT garbage collection, an executable link, reset, device
absence or runtime behavior.

The next gate is a coordinator review of the remaining separate retention
roots (SIM virtual surface, PC-system lifecycle, logging) and the full CPU
static opcode-map family before any new object model or link observation. No
provider may be added merely because this object now has only CRT imports.
