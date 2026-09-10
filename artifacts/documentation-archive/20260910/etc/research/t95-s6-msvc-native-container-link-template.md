# T95 S6 MSVC native-container original link template

## Question

What original metadata names the native Bochs executable's actual library
ordering, without mistaking VS2008 solution order for a linker response file?

## Observation

The pinned generated MSVC `refs/bochs/Makefile:175-186` contains the original
`bochs.exe` target and `link` invocation. It supplies the following template,
in order:

```text
BX_OBJS + SIMX86_OBJS
  iodev/libiodev.a
  iodev/hdimage/libhdimage.a
  iodev/usb/libusb.a
  iodev/network/libnetwork.a
  iodev/sound/libsound.a
  cpu/libcpu.a
  cpu/cpudb/libcpudb.a
  memory/libmemory.a
  gui/libgui.a
  DISASM_LIB
  FPU_LIB
  GUI_LINK_OPTS + MCH_LINK_FLAGS + SIMX86_LINK_FLAGS + READLINE_LIB
  EXTRA_LINK_OPTS + LIBS
```

`NONINLINE_OBJS` names the original executable-owned lifecycle sources:
`logio`, `main`, `config`, `load32bitOShack`, `pc_system`, `osdep`, `plugin`,
`crc` and `win32res`.

The retained UCRT r4 native-container build log independently shows that the
same project families were built into archives (for example full iodev,
93-unit CPU, CPU database, memory and GUI archives). It is provenance for
membership only: its GCC artifacts are never MSVC inputs.

## Interpretation

This closes the former link-order ambiguity. It also confirms the central
architecture result: optional device archives can remain a **compiled
container support** family while runtime admission is controlled by the copied
CPU3/nogui profile and original plugin gates. A build graph is not a feature
enablement statement.

## Required future manifest fields

Before one MSVC container build, a generated fresh-root manifest must expand
every template variable to absolute copied inputs and record:

- source/configuration hashes and BuildTools x86 identity;
- each lifecycle object and original archive in the above order;
- CRT mode and system-library inputs;
- projected feature status from the feature manifest;
- profile text/hash, including original `nogui` and optional-plugin denial;
- an explicit exclusion of adapter/CLI/OpenNT objects; and
- reset/controlled-stop positive and negative predicates.

The manifest is not yet generated and no link command is authorized here.
