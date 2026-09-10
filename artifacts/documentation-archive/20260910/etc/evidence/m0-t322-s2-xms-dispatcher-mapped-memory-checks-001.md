# M0 T322 S2 P2 — XMS dispatcher and mapped-memory checks

## Scope

The selected XMS owner is the original `src/mvdm-host/xms.486` package. This
delivery verifies only its retained dispatcher and the one approved
guest-address binding. It does not make an XMS-private allocator, UMB
provider, BIOS, PIC, or INT15 completion path.

## Source review

- `xmsdisp.c` remains the original `apfnXMSSvc` table and `XMSDispatch`
  implementation. Its twelve entries remain, in order: A20, move, allocate,
  free, system page size, extended-memory query, UMB init/request/release,
  INT15 hook, free-memory query, and realloc.
- `xms.c` retains the original `SAInitialize` call, allocator order and
  callback slots. The selected `MVDM_XMS_SESSION_BACKEND` branch changes only
  the historic raw-process-pointer backend selection; it still passes
  `xmsCommitBlock`, `xmsDecommitBlock`, and `xmsMoveMemory` in the original
  callback order.
- `i386/xmsmem86.c` is retained as source evidence only. Its direct casts from
  `ULONG` to `PVOID` and process-wide `NtAllocateVirtualMemory` cannot compose
  with the selected SoftPC backing or x64. The same-shaped adapter is therefore
  the first viable source-recovery rung.

## Binding and focused proof

`adapter-mvdm-host-out/softpc/mvdm_xms_memory.c` now explicitly preserves the
historical `RtlMoveMemory(destination, source, count)` overlap result even
when a transfer crosses its 4 KiB bounded lease chunks. It retains no lease or
native pointer after each synchronous operation.

The dedicated fixture was generated and run on both host-width rows:

```text
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T322XmsMappedMemoryFixtureNinja.ps1 -Architecture x86 -RepositoryRoot O:\repos.hobby\ntvdm64 -BuildRoot O:\repos.hobby\ntvdm64\build\M0-T322\S2\xms-mapped-memory
cmd.exe /d /s /c build\M0-T322\S2\xms-mapped-memory\x86\msvc-mt.cmd ninja -C build\M0-T322\S2\xms-mapped-memory\x86
build\M0-T322\S2\xms-mapped-memory\x86\xms-mapped-memory-fixture.exe

powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T322XmsMappedMemoryFixtureNinja.ps1 -Architecture x64 -RepositoryRoot O:\repos.hobby\ntvdm64 -BuildRoot O:\repos.hobby\ntvdm64\build\M0-T322\S2\xms-mapped-memory
cmd.exe /d /s /c build\M0-T322\S2\xms-mapped-memory\x64\msvc-mt.cmd ninja -C build\M0-T322\S2\xms-mapped-memory\x64
build\M0-T322\S2\xms-mapped-memory\x64\xms-mapped-memory-fixture.exe
```

Both rows compiled five source objects, linked `xms-mapped-memory-fixture.exe`,
and exited successfully. The fixture proves:

- commit and decommit zero their checked guest span;
- overflow commit is rejected without a host-pointer conversion;
- a 5,000-byte overlapping move crossing the 4 KiB lease boundary has the
  same result as `memmove`/historical `RtlMoveMemory`;
- the original `SS:BP-12` descriptor layout reaches the mapped move binding;
- an invalid descriptor produces the selected controlled session cancellation,
  rather than a fabricated XMS success.

The existing formal selected product graphs were then incrementally rebuilt:

```text
build\M0-T321\S2\x86\run-ninja-parallel.cmd original-softpc-process.exe
build\M0-T321\S2\x64\run-ninja-parallel.cmd original-softpc-process.exe
```

Each row recompiled `mvdm_xms_memory.c`, rebuilt `softpc-bindings.lib`, and
linked `original-softpc-process.exe` successfully.

## Deliberate limit and transfer

This is local XMS source/ABI closure, not a claim that a guest has completed
all XMS services. `xmsumb.c` continues to own UMB policy and its original
no-space behavior; `xmsNotifyHookI15` continues to own XMS-side INT15 state.
Actual hardware delivery remains the named CPU40/SAS/ICA machine prerequisite
from the fixed vertical-slice evidence. No synthetic UMB or INT15 success was
introduced here.
