# S10 CCPU386 pending C/H follow-up

This review is limited to `softpc.new/base/ccpu386`. The current provisional
host inventory identifies 19 pending physical source paths below. On this
review each current file's SHA-256 equals the OpenNT hash in
`frozen-file-reconciliation.csv`. This is byte retention, not execution,
completeness or a behavioral recovery percentage. The global ledger is not
changed by this report. Other profiles, guest behavior and task lifecycle are
not established by the observations below.

## New selected-profile preprocessing comparison

The recorded worker graph has `CPU_40_STYLE`, `CCPU`, `C_VID`, `SPC386` and
`PROD`, without `SPC486` or `PIG`. Using flags extracted from that graph and
MSVC x86, two disposable copies of the same source path were preprocessed per
case: baseline, and one with exactly one direct include removed. No real
source was edited. Preprocessed outputs were compared after removal of blank
lines; this removes no other whitespace or tokens. Source, generated graph,
and runner identities are listed below. The commands and driver are retained
under [s10-cpu-profile-evidence](s10-cpu-profile-evidence/).
The first attempt had a newline in the generated command that broke output
redirection and returned 255; it is not counted. After trimming that command,
all six preprocessing invocations exited zero. Existing compiler warnings
were present, and each baseline/removal pair had identical diagnostic output.

| Header and tested TU | Current dependency | Result and bounded meaning |
| --- | --- | --- |
| `gdpvar.h` in `sasCdef.c` | `obj/ccpu/sasCdef.obj`, VALID | With and without the direct include, the raw preprocessed outputs are byte-identical, SHA-256 `0F77971E96F8B7DF876A54C590D6C886578B6F24949A5FAC2AA70B9A8825E8C7`. This direct include adds no emitted preprocessing text in this exact scratch source and selected flags. |
| `univer_c.h` in `sasCdef.c` | same object, VALID | Raw output differs by five blank lines only; nonblank-line SHA-256 is `BC4C47375D039DD0A592BE36D9A23CBCCED42F27B0C1DEAAE081B9E6A69086E3` for both. Supports its already recorded non-PIG physical-role exclusion, not a global generated-schema exclusion. |
| `pigger_c.h` in `c_reg.c` | `obj/ccpu/c_reg.obj`, VALID | Raw output differs by five blank lines only; nonblank-line SHA-256 is `C75F4CE314E868144437C12598FA802B260A37E9EB8DFA1496ADEC3796C9F6A0` for both. Supports the recorded non-PIG physical-role exclusion, not exclusion of CPU register or PIG capability. |

The dependency capture was checked for case-insensitive exact paths: these
were the only observed objects including each of the three physical headers,
and its parsed output reported zero STALE blocks. Its SHA-256 is
`47565CA5D5AE3FCEDD9FE004380F9A1FF912425482C7B3F825167F0C256A8D19`.
The two source identities are `sasCdef.c`
`0EC6AFC6E8FC2086639044D1ADB3460EDFE36B265C6B0CBFEB89391901870BE9` and
`c_reg.c` `D9BBC34913D015C386C354967D82F31ED53D87487EBFC183684EEE338FD4C150`;
graph SHA-256 is
`835D8D25248FFBE2C4B4B147C056A43EF7C2F3624A85E09F141258F45FA0410C`.

The `gdpvar.h` observation supports a **profile-limited physical exclusion**
for this direct include only. The global row should remain pending if the
ledger's `excluded` means all supported profiles: generated-schema ownership,
other profiles and external consumers are not proven by an include-removal
comparison. S11/S13 retain actual GDP, CPU and video behavior. Do not promote
this to a full restored behavior or functional test.

## Nineteen current pending physical paths

| Paths | Evidence and safe disposition | Remaining functional owner |
| --- | --- | --- |
| `bswap.c`, `bswap.h` | Selected source compiles under SPC386 but SPC486 body/decoder is inactive; no 486 execution shown. **Pending** globally. | S11 paired gates, operand widths and #UD/faults. |
| `cmpxchg.c`, `cmpxchg.h` | Same profile boundary. **Pending** globally. | S11 8/16/32-bit, flags, locking, writeback and faults. |
| `invd.c`, `invd.h` | Same profile boundary; original empty leaf presumes its cache model. **Pending** globally. | S11 cache model and privilege/fault order. |
| `invlpg.c`, `invlpg.h` | Same profile boundary. **Pending** globally. | S11 paging/TLB ownership and faults. |
| `wbinvd.c`, `wbinvd.h` | Same profile boundary; original empty leaf is not host-cache proof. **Pending** globally. | S11 cache model and privilege/fault order. |
| `xadd.c`, `xadd.h` | Same profile boundary. **Pending** globally. | S11 both results, aliasing, flags and faults. |
| `c_getset.c`, `ccpupig.c`, `ccpupig.h` | Historical PIG-specific comparison and declarations under inactive `PIG`; a non-PIG physical-role exclusion is supportable only within that profile. **Pending** if disposition is global. | S17 determine whether original PIG diagnostics are required; CPU execution is separate. |
| `evid_c.h`, `evidfunc.h` | Alternate generated CCPU video declarations/names differ from selected C-VID family; no equivalence shown. **Pending**. | S11/S13 actual generated-provider ABI and mode lifecycle. |
| `gdpvar.h` | See new selected-profile preprocessing comparison. **Pending** globally; bounded exclusion candidate only. | S11/S13 generated schema, GDP and actual vector ownership. |
| `ntstubs.c` | Alternate non-PIG aggregate provides CPU, FPU and SAS wrappers/stubs, but is absent from the current graph and conflicts with selected providers. **Pending**. | S11 map applicable exact caller/provider and failure semantics; do not infer feature absence. |

The existing [486 profile audit](shard-3/cpu486-profile-boundary.md),
[CCPU physical-role review](host-ch-scope/ccpu-pending-physical-role-review.md),
and [PIG profile evidence](host-ch-scope/dos-pig-env-physical-owner-review.md)
give the detailed earlier source and graph evidence. No new production, guest,
or release image was changed, and no live 486 or PIG behavior was tested.

## Cache incident

While collecting the dependency output, this reviewer mistakenly invoked the
Visual Studio bundled Ninja 1.12.1 on the worker tree, whose normal Ninja is
1.13.2. It reported `build log version is too new; starting over`; afterward
the existing worker `.ninja_log` was absent. The `.ninja_deps` file and
cached objects remained present. This is an accidental build-log loss, not an
authorized cache clear and not evidence of a source or product failure. The
coordinator was notified immediately and owns any recovery; this reviewer did
not rebuild, delete or restore caches. The captured dependency listing and
the comparison above are bounded observations and do not prove cache integrity.

Coordinator follow-up: the target was rebuilt using project Ninja 1.13.2 and
the existing repository ccache; the regenerated log records 532 build edges,
and the immediate second build reports no work. See the
[NIDDB candidate record](host-ch-scope/niddb-allocation-rollback-review.md)
for exact rebuilt/tested image identities and PE-section comparison. The lost
prior log was not recovered, and the later image is not claimed to be the
same tested file.
