# M0 T470 S2 — WOW ToolHelp query disposition

## Executable witness

`tests/observation/win16_toolhelp_probe.asm` is an independently authored NE
program.  It imports the immutable original `TOOLHELP.DLL`, reports KERNEL
version/mode, then calls these original exports:

1. `TaskFirst` with packed `sizeof(TASKENTRY)`;
2. `ModuleFirst` first with a zero size (required rejection), then with packed
   `sizeof(MODULEENTRY)`;
3. `ClassFirst` with packed `sizeof(CLASSENTRY)`.

The companion `verify-win16-toolhelp-probe.ps1` stages a supplied unchanged
ToolHelp image beside the probe, uses the established isolated `run16 --wait`
observer route, and writes a fixed binary result.  It has two deliberate
contracts: normal retained-query acceptance for a compatible provider, and a
separate `-ExpectOriginalUnavailable` negative contract.  The latter does not
claim that ToolHelp functionality works.

The current sealed T467 package run used the immutable
`O:\winnt\tests\w31p\SYSTEM\TOOLHELP.DLL`, stable observer
`build/.ninja/observations-x64/console-startup-observer.exe`, and retained
`build/t470-s2/toolhelp-r4`.  It completed normally and returned:

| Field | Observed value | Meaning |
| --- | ---: | --- |
| KERNEL version | `0x0a03` | The guest identifies as Win3.1. |
| WinFlags | `0x4825` | Protected-mode bit is set. |
| `TaskFirst` | 0 | Original ToolHelp declines the query. |
| bad-size `ModuleFirst` | 0 | Expected rejection, but not independently sufficient. |
| valid-size `ModuleFirst` | 0 | Original ToolHelp also declines the valid query. |
| `ClassFirst` | 0 | Expected original WOW `UNIMPLEMENTEDAPI` result. |

The probe therefore reached the guest cleanly, wrote its report, and retired
through the ordinary observer path; it is not a launch, rendering, or worker
lifetime failure.  The same result occurred with the pinned prebuilt source
`toolhelp.dll`, so it is not a choice between the two examined immutable DLL
copies.

## Source disposition

Each retained query first tests `wLibInstalled`.  `ToolHelpLibMain` obtains
that state through `KernelType`, which reads the original private KERNEL
`THHOOK`/global-heap and task/module-list layout (`hGlobalHeap`, `hExeHead`,
`headTDB`, and adjacent fields).  The supplied 16-bit ToolHelp image and the
current `KRNL386.EXE` are not the same immutable Win3.1 image pair; their
hashes differ.  The executable result proves the private-layout initialization
did not establish a usable ToolHelp query provider.  It does **not** prove a
particular private field is wrong, and this audit does not reinterpret modern
objects as that layout.

Repairing the result would require changing/rebuilding a guest KERNEL or
ToolHelp pair, or manufacturing a host ToolHelp/task/module provider.  Both
are outside the immutable-guest and one-owner rules; no matching original host
correction was found in the selected WOW sources.  The explicit class failure
is independently original even with a compatible pair.

Accordingly, ToolHelp query exports are retained as an immutable original
guest compatibility limitation.  There is no project-owned production change,
no release asset change, and no candidate host adapter in this stage.  The
negative witness remains to prevent a later launcher/worker failure from being
misreported as a working ToolHelp query route.
