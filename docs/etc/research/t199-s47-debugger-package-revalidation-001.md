# T199 S47 — Debugger package revalidation

## Question

What is the real OpenNT Debugger BOP ABI, and can the current bound CLI
composition preserve its boundary without interpreting a following guest byte
as a service or fabricating debugger behaviour?

## Source map and boundary

`src/opennt/base/mvdm/inc/bop.h` defines `BOP_DEBUGGER` as selector `56h`.
`softpc.new/host/src/nt_bop.c` routes `MS_bop_6` directly to `DBGDispatch()`
and explicitly states that debugger parameters are on the VDM stack; it neither
reads nor advances an inline service byte.  `dbg/dbg.c` confirms that its
dispatcher obtains mode and arguments from historical CCPU/SAS stack access.

The macro text in `inc/dbgsvc.h` contains historical mode values `00..0F`, but
the real DPMI callers in `dpmi/486/dxboot.asm` push mode/arguments before the
three-byte `C4 C4 56` instruction and clean the stack afterwards.  Thus the
mode is a guest-stack ABI, not byte four of this BOP.  A four-byte observation
window may contain the following guest instruction and must not consume it.

The original host dispatch combines CCPU/SAS, VDM debug context, host pointers
and Win32 exception/event mechanisms.  It cannot be linked into the modern
non-invasive CLI composition.  The selected profile therefore supplies no
debugger provider and returns a typed controlled stop without reading the
guest stack or changing registers/flags.

## Procedure and observation

Fresh evidence was produced with:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t199-s47-debugger-package-r1 `
  -Fixture boot-namespace -HostArchitecture x64
```

The source-built MSVC x64 `/MT` fixture linked and exited zero.  Its bound
composition exercises both `C4 C4 56` and `C4 C4 56 90`; both produce typed
stop with no resume RIP, GPR delta, or flags delta.  The `90` is preserved as
the following instruction, not classified as debugger service `90h`.

## Interpretation

The complete Debugger package is closed only at the declared deferred CLI
disposition.  It proves no Debugger event delivery, stack-mode decode,
breakpoint, attach, ToolHelp, VDD, GUI or host-debug capability.  Bochs remains
selector-blind and no native guest trace ran.  Any future debugger support must
admit one complete stack ABI and host-event provider package, not add a
trace-discovered selector handler.
