# S10 WOW32 pending C/H follow-up

Scope: the seven `src/mvdm/wow32` C/H rows marked pending in the recorded
`host-ch-scope/inventory.csv`; no other directory is adjudicated here. The
coordinator owns the shared ledger. This report recommends **physical-file
roles for the checked NT release profile**, not exclusion of guest features,
and does not silently convert those limited recommendations into global
dispositions. All seven current files match both original editions and the
frozen original identities; no mirror diff or new original-code exception is
involved. Text identity is not functional proof.

Both requested audit catalogues were checked. The DOS16
`management/source-file-catalog.csv` calls each exact path `excluded_win16`
and does not adjudicate its WOW behavior. The WOW32 catalogue has two
`host_boundary` entries per path, one for each original edition; its
`source-coverage.csv` does not certify whole-file restoration. Thus the DOS
label cannot be used to exclude any of these WOW capabilities.

## Physical roles and remaining proof

| File | Recommendation for the checked release profile | Evidence and remaining functional boundary |
| --- | --- | --- |
| `debug.c` | **Overall excluded in this profile**, subject to coordinator confirmation that this non-product debugger extension is outside the active profile. Not a claim about DEBUG or WOWPROFILE builds. | `wow32/sources` does not select it. Lines 66–2063 put the debugger helpers under `DEBUG_OR_WOWPROFILE`; `help`, task/dispatch dumps, filters and cursor alias inspection depend on debugger extension APIs and data symbols, not the normal guest API dispatcher. S17 must separately decide debugger feature applicability and test an actual debug build/provider. |
| `wddeml32.c` | **Overall excluded in this exact selected profile**, not a DDEML capability exclusion. | Neither original `wow32/sources` nor the current graph selects it; selected `wowtbl.c::aw32WOW` has no `wddetbl2.h`. It implements a separate historical DDEML thunk using `DdeGetCallbackInstance`, callback/address maps and `CallBack16`, unlike the separate selected classic WM_DDE layer and the unchanged guest DDEML manager. S14 must establish the loaded guest's actual route and test real peer transactions, callback reentry and teardown before deciding whether this old provider is required. |
| `wddeml32.h` | **Overall excluded in this exact selected profile**, together with that specific unselected thunk implementation. | Repo and both original WOW32 trees name it only from `wddeml32.c`; it declares the private `BIND1632` tables and `WD32Dde*` routines. It is not the selected guest-manager ABI. Alternate builds, external consumers and the guest's actual manager remain separately unverified. |
| `wddetbl2.h` | **Overall excluded in this exact selected profile**, not DDEML itself. | It defines the `MOD_DDEML` ordinal table, but `wowtbl.c` includes the kernel, USER, GDI, keyboard, sound, shell, Winsock, ToolHelp, multimedia and command tables instead; no include of this table occurs in checked original or current MVDM C/H. S14 must prove any other historical/guest dispatcher before importing the table. |
| `wmmalias.c` | **Overall excluded for the exported/executable multimedia-alias behavior in the checked image**; record the surviving static datum and restrict the disposition to that profile. | `wow32/sources:102` and the current Ninja graph compile the original object, but the captured `wow32.dll.map` has no `GetMMedia16`, `GetMMedia32`, `FreeMMedia16`, `FindHMap32`, `FindHMap16`, or `FreeHMap16` function symbols; it does have `FindHMap16`'s `hmDummy` datum at map line 6719. Source-wide macro-use search puts all C uses of `walias.h`'s multimedia alias macros in `wmmedia2.c`, whose substantive body is under its original outer `#if 0`. This does not prove other build profiles or external dynamic references, nor does it prove wave/MIDI operation. S16 owns actual immutable guest → WINMM resolution and callback/data/teardown validation. |
| `wmmalias.h` | **Overall excluded for its historical alias structures/functions in the checked profile**, subject to the same image/profile qualification. | Only `wmmalias.c` includes it in the checked MVDM tree; it describes `HINFO`/`HMAP` and the historical `FindHMap` family. Preserve associated multimedia capability in S16; do not infer it is obsolete across all builds or that the exported WINMM resolver has equivalent semantics. |
| `wmmstruc.h` | **Overall excluded for its conversion macros/prototypes in the checked profile**, despite inclusion through `precomp.h`; do not equate dependency-file inclusion with executed conversion. | A single-pass exact-name scan of all header-defined macros across `src` C/H found consumers only in `wmmedia1.c`, `wmmedia2.c`, and `wmmstruc.c`; their substantive implementations are enclosed by original outer `#if 0`. The declared converter function names occur only in `wmmstruc.c` and this header. S16 must validate the distinct live guest/WINMM route, including wave/MIDI/MCI data, asynchronous callbacks, failure and task retirement. Other source variants and external consumers are not ruled out. |

These are coordinator-review recommendations. If the shared ledger's
`overall excluded` means *all* historical configurations or all related
features rather than a recorded physical-source/profile role, keep the rows
pending until that interpretation is resolved; do not misstate the bounded
evidence. No row has sufficient test evidence to claim full behavior, and no
behavioral recovery percentage can be derived from these static findings.

## Additional S14 source risks in the unselected DDEML alternative

`wddeml32.c::WD32DdeInitialize` maps `p` from the guest parameter, invokes
native `DdeInitialize`, then explicitly notes that guest memory may have moved.
It reacquires the frame and argument pointer but dereferences and writes the
previous `p` before its eventual `FREEMISCPTR`; `wow32.h` shows that
`GETMISCPTR` obtains a guest mapping. This is a **conditional stale-mapping
source risk** if that old alternative is ever selected and guest memory can
move across this call, not a demonstrated current-product fault. A suitable
S14 test must inject that move and verify pointer remapping, identity,
callback reentry and cleanup before selecting a correction.

`WOWDdemlBind` has a fixed 3,200-entry array and returns `void`: a full table
only logs and cannot propagate failure to its caller. `WOWDdemlGetBind16` and
`WOWDdemlGetBind32` fall off the end of a non-void function after a missing
entry. These are original source behaviors in the unselected alternative,
not present-day reproductions. If an eligible guest is proved to require this
provider, S14 needs bounded full-table and missing-mapping tests, plus a
design that preserves observable contract and registers any source exception;
do not import or activate the provider merely to remove a pending row.

## Identity and evidence limits

Current file hashes, identical to both frozen originals: `debug.c`
`0B78331BEE5BD87CA5F786D162F345A46A04C255FDEA1BC6733FB2F171AD02A2`;
`wddeml32.c` `13DB1E133CBB9BE8037FE9A57B1CF637F744EF23C9277697C096486BDE3A8C2C`;
`wddeml32.h` `B74CDA34EC90308AA0212142E7282A878DD7C2267A0305C07147543C0B0A0A52`;
`wddetbl2.h` `AE89A2A82C10AD6FF2B8A41F7E876AF2286B9AD48C8441B7726D901483E30148`;
`wmmalias.c` `D4949206F261D22F80F96E769388AEE94037B27092C69C68EC058619F843F4FB`;
`wmmalias.h` `48BCEE7ABCCA92BAFD6DCEA4BCFD9437BAE10C170BB9BFEE4DB587E00E134CC0`;
`wmmstruc.h` `8C8C764F500C47689F5D97C485E8D81C6361130C6CE744A3DBE8D2D26C1EAF5B`.
Current graph SHA-256 `03FE1C4AEB2CFB7434FA1471C55A3EF62652254615E630BA5861AE7F8729E278`,
cached map `37CE4B735DA1536D2DEE3492169D5A46785C04111E305A378E239B33D552B1F5`,
and source recipe `D24767FF50C4C7017E6FCF5F550E120DB0F580F7C07C77CBE7E99E4432F52B36`
are separate snapshots: they do not independently bind a specific installed
image or prove all compiler profiles. No Ninja command, product code or guest
image was modified in this review. Previous concrete selected WINMM resolver
failure evidence is [already recorded](host-ch-scope/wow-winmm-resolve-object-review.md)
and remains S16; it must not be duplicated as a new finding here.
