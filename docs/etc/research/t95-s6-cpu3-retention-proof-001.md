# T95 S6 CPU3 retention proof 001

## Decision and scope

The coordinator selected CPU3 solely for the first bare-machine mechanics
profile, applying the owner's earlier minimal/no-stock-firmware intent. CPU5
stock-ROM/floppy remains a separately deferred profile. This does not select
an eventual OpenNT runtime profile or narrow the project objective.

The immutable analysis root is
`artifacts/analysis/t95-s6-msvc-x86-cpu3-retention-001-20260811-001`.

| Input | SHA-256 | Observation |
| --- | --- | --- |
| Copied `src/bochs/config.h` before replacement | `A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8` | Pinned baseline. |
| Accepted v2 CPU3 projected config | `C47030F79880C894A346F56E3AB3242924308983E00B5845EDCE849450C6C19D` | Artifact root config only. |
| Non-product fixture | `22EA4D39D76E38EF57309DBD4DE54C121EB306274A1834FCF631BB6E0578EC85` | Roots the private composition without execution. |

The fixture includes only `bochs.h` and the private minimal-machine header,
constructs the composition, and references its existing `initialize` and
`cleanup` expressions. It was never run and contains no loop, reset, firmware,
adapter, OpenNT, CLI, DOS, guest, device, plugin, or GUI product call.

## `/Gy` object observation

Confirmed BuildTools MSVC 14.43.34808 HostX64/x86 compiled one batch with
`/nologo /c /Gy /MT /EHs-c- /DWIN32 /DBX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=0`
and only copied Bochs root, `cpu`, `memory`, `iodev`, `gui`, `instrument`, and
`instrument/stubs` include directories. It exited 0 and made 72 objects.
`logs/compile-direct.log` retains complete output; `logs/object-hashes.json`
retains every object name and SHA-256.

Inputs were the fixture; composition/minimal-SIM
(`bx_ntvdm_minimal_machine.cc`, `bx_ntvdm_minimal_sim.cc`); original non-CPU
core (`gui/siminterface.cc`, `gui/paramtree.cc`, `logio.cc`, `pc_system.cc`,
`memory/memory.cc`, `memory/misc_mem.cc`, `iodev/devices.cc`); and this exact
CPU3 ledger family:

```
init fetchdecode icache resolver access access32 arith8 arith16 arith32
mult8 mult16 mult32 logical8 logical16 logical32 shift8 shift16 shift32 bit
bit16 bit32 bcd data_xfer8 data_xfer16 data_xfer32 load ctrl_xfer16
ctrl_xfer32 ctrl_xfer_pro call_far jmp_far ret_far iret stack stack16 stack32
segment_ctrl segment_ctrl_pro protect_ctrl tasking vm8086 soft_int flag_ctrl
flag_ctrl_pro exception crregs paging smm io string proc_ctrl debugstuff crc32
fpu_emu mmx sse sse_move sse_pfp sse_rcp sse_string xsave aes
```

There are no compiler errors. The log has only the known repeated `FMT_LL`
C4005 warnings from the accepted projection. Before that batch, a
`Start-Process` launcher failed before `cl.exe` because `Path`/`PATH` collided
in its environment dictionary; an initial `cmd.exe` wrapper then failed before
`cl.exe` with `The command line is too long`. Both are retained as pre-compiler
facts; the direct invocation above is the sole compilation observation.

## Single link observation and stop boundary

One MSVC x86 `link.exe` call was made against the 72 objects with
`/SUBSYSTEM:CONSOLE /INCREMENTAL:NO /OPT:REF /MAP /VERBOSE:LIB
/VERBOSE:UNUSEDLIBS`. The exact argument display and output are retained in
`logs/link-actual-command.txt` and `logs/link-actual.log`.

`link.exe` started once and exited `1146` before reading an object:

```
LINK : fatal error LNK1146: no argument specified with option '/OUT:'
```

The retained display proves that the PowerShell construction split `/OUT:` and
`/MAP:` from their paths. There is no `retention-fixture.exe`, map, unresolved
symbol list, verbose import/library result, PE import table, or retained-COMDAT
result. This is a command-shape/tooling boundary, not a CPU3 source/object
closure result. The one-link admission prohibits a retry.

## Parameter-vector repair and validation

`tools/New-T95S6LinkArgumentVector.ps1`
(`6B3BAC407392C5F9CA327E3FF1177B8A465DD2F669A5AD5DDF9290CEAD90EE31`) is
a deterministic future-link vector generator and validator, not a build
wrapper. Given an absolute output path, absolute map path, and existing
absolute `.obj` paths, it produces only these non-object tokens:

```
/nologo /OUT:<absolute-path> /SUBSYSTEM:CONSOLE /INCREMENTAL:NO /OPT:REF
/MAP:<absolute-path> /VERBOSE:LIB
```

It validates the exact expected output/map tokens once each, both required
retention options once each, every object token's existence and uniqueness,
and rejects every other option. Thus split `/OUT:` or `/MAP:`, an empty value,
duplicate `/OPT:REF`, a missing/duplicate object, and product/run options such
as `/DLL` fail before any toolchain call.

`tests/bx-ntvdm-adapter/Test-T95S6LinkArgumentVector.ps1`
(`9BBCC82B9757A8C0013385B575909808816C9D01A140F1C383FABBF3D343C623`)
creates only two empty synthetic `.obj` fixture inputs under a temporary test
directory. Its positive and malformed-negative tests pass without calling
`cl.exe` or `link.exe`. The retained CPU3 inputs remain unchanged: fixture
`22EA4D39D76E38EF57309DBD4DE54C121EB306274A1834FCF631BB6E0578EC85`,
72-object manifest `BFF16CBFDF2F3104B4E0732A94FB787FF1504204D5718B6BA4B5A3D91485760F`,
and failed argument display `E9D5DD241DB636EBBB8D406AE7CE2B8EE2C288F6365A9C6CDB2FF29FD0CEACC6`.
The corrected vector has deliberately not been generated against the 72-object
artifact or passed to `link.exe`; that needs a separately admitted fresh
observation.

## Observation 002 pre-link stop

The separately admitted fresh root
`artifacts/analysis/t95-s6-msvc-x86-cpu3-link-retention-002-20260811-001`
revalidated the frozen source manifest hash
`BFF16CBFDF2F3104B4E0732A94FB787FF1504204D5718B6BA4B5A3D91485760F` and
all 72 source-object hashes before creating its explicit input manifest
(`2AB84B556A542D61345FE59514EB1439C54E16CED58EB3447A087CE3CFE7DF6F`).
The pre-invocation stage then failed before vector generation because the
shell session dot-sourced `New-T95S6LinkArgumentVector.ps1` without the
required execution-policy bypass:

```
File ...\New-T95S6LinkArgumentVector.ps1 cannot be loaded because running
scripts is disabled on this system.
```

The generator was therefore not loaded and `New-T95S6LinkArgumentVector` was
not recognized. The retained `pre-invocation-validation.json`
(`13A8AEFBFEDBE24AAFCB8B67E166F43869FBFACBF3E36150ECEFC4090C49A88D`)
records null vector/hash/tokens and false retention flags. No vector, link
command, map, executable, undefined-symbol list, or import evidence exists.
Under the first-error stop rule, the root is preserved and no alternate
execution-policy retry, link call, object change, or run was made.

## Classification and next gate

| Question | Result |
| --- | --- |
| CPU3 `/Gy` object creation | Available: 72 objects, hashes retained in artifact log. |
| COMDAT/map/unresolved/import evidence | Unobserved: only link call failed before object processing. |
| Required or rejected retained owner | Unclassified: map/undefined/import evidence does not exist. |
| Runtime/reset/bounded-stop/firmware/guest behavior | Not attempted and not claimed. |

The next gate is coordinator review of this pre-link execution-policy failure.
Any later observation needs a fresh explicit admission and must establish the
approved PowerShell policy before creating its fresh root; it may not infer a
linkable target, add inputs, or enable a denied feature.

## Explicit PowerShell entrypoint proof

`tools/Invoke-T95S6LinkVectorEntrypoint.ps1`
(`5C993833318843641A1B955D5C2E5E92589F9EDAA704B7F3AA38FB970A90E06B`) is
the sole documented policy-safe entrypoint for the vector generator. It is
intended only for this form:

```
powershell.exe -ExecutionPolicy Bypass -File Invoke-T95S6LinkVectorEntrypoint.ps1 ...
```

It takes an absolute output path, map path, output-vector path, and an
absolute JSON string-list path for existing objects. Within that explicit
Bypass process it dot-sources only the accepted generator
`New-T95S6LinkArgumentVector.ps1`, verifies that both required exported
functions exist, generates and validates the vector, then writes JSON. It has
no `cl.exe`, `link.exe`, response-file, or build-tool invocation. A missing
generator is a fail-closed error before vector output.

`tests/bx-ntvdm-adapter/Test-T95S6LinkVectorEntrypoint.ps1`
(`527870EA69FA57EE8649878A33F4000435C5AB9E58341F2EC2314662234A9802`)
launches exactly that `powershell.exe -NoProfile -ExecutionPolicy Bypass -File`
form against two temporary existing synthetic `.obj` files. It proves emitted
single-token output/map paths and both retention options, then proves that an
explicit nonexistent generator exits nonzero without creating a vector. The
accepted generator remains hash
`6B3BAC407392C5F9CA327E3FF1177B8A465DD2F669A5AD5DDF9290CEAD90EE31`.

Therefore dot-sourcing is policy-safe only through this explicit Bypass
entrypoint, not through an arbitrary ambient PowerShell session. This proves
the entry mechanism alone: it generated no CPU3 vector and did not invoke a
link, compiler, executable, or other build tool.

## Observation 003: one validated-vector link result

Fresh root `artifacts/analysis/t95-s6-msvc-x86-cpu3-link-retention-003-20260811-001`
revalidated all 72 frozen object paths/hashes against the original manifest
`BFF16CBFDF2F3104B4E0732A94FB787FF1504204D5718B6BA4B5A3D91485760F`.
Its explicit `object-paths.json` and input manifest hash to
`D575B4813DD0ABEA46E8C814A7C854D1316AFD7532727A2C892FB4E2B9EA1E70` and
`BFB2829688D102578358EB92842307D545F4064E105F1AED70E6D7437847B0DF`.

Only the explicit-policy entrypoint emitted `link-argument-vector.json`
(`BBEA0874E957568DF2F406A033D2CACB0532A540759584524D3EC44B7D13C928`).
It contains the original order of 79 tokens: the seven validated fixed tokens
(`/nologo`, fresh `/OUT:`, `/SUBSYSTEM:CONSOLE`, `/INCREMENTAL:NO`,
`/OPT:REF`, fresh `/MAP:`, `/VERBOSE:LIB`) followed by exactly the frozen 72
absolute object paths. The subsequent direct call used that persisted JSON
array without token construction, addition, deletion, or reordering.

The one invocation used BuildTools x86
`link.exe` 14.43.34809.0
(`B56E474990A7042E3DA861AFE958BC592D22EA4DF00DBE1FD453617C9C840244`);
`link-invocation.json` records the executable, vector, and exact token list.
The retained `link.log`
(`25406CF6D88DFAED1785EF42AC1350B4A2BDD7B86C2D945F02675DC6351817EC`)
contains `/VERBOSE:LIB` search evidence and ends with exit `1120`:

```
retention-fixture.exe : fatal error LNK1120: 72 unresolved externals
```

The log has 49 `LNK2019` reference lines and 34 additional `LNK2001` reference
lines for those 72 unresolved external identities. It demonstrates both
required-closure gaps (for example CPU `inhibit_interrupts`, `set_INTR`,
`prefetch`, `repeat`, and `repeat_ZF`) and retained forbidden owners: plugin
control and product device plugin initializers; CMOS/DMA/PIC/PIT/VGA/floppy;
virtual/slowdown timers; GUI state; and SIM configuration/UI methods. It also
contains unresolved fetch/decode handlers from VMX/SVM, MSR, GETSEC and ADCX/
ADOX families. These are measured unresolved edges, not authorization to add
objects, enable features, alter configuration, or move any semantics to the
adapter.

`retention-fixture.map` was produced but is zero bytes. No executable/PE was
produced, so no import table exists; map retention and PE-import conclusions
are unavailable. The link is a negative closure result: the declared CPU3
source/object family is not link-closed and retains rejected product owners.
No retry or post-failure object/source/configuration change occurred.

## Next gate after observation 003

Coordinator review must first distinguish the required native CPU providers
from rejected product-retention edges using this fixed log. Any further action
needs a new bounded admission; it must not automatically add the unresolved
objects, enable a denied device/feature, or convert this link evidence into a
runtime or OpenNT claim.

## Non-claims

No Bochs, adapter, OpenNT, guest, configuration, exception-register, build
recipe, or VS project source changed. No `bochs.exe`, `all`, `configure`,
firmware/device/plugin enablement, executable invocation, reset, or CPU loop
occurred. Successful object compilation is not a link, executable, runtime,
or product claim.
