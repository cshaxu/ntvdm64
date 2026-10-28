# DOS command source-input reconciliation

## Question, identity and procedure

This S10 checkpoint follows `bba17e229`. It reviews all **85** remaining
`.asm`, `.inc`, `.h` and `.c` paths under the original `dos/v86/cmd` tree.
The [observations](dos-command-input-observations.json) bind every current /
primary-OpenNT source pair and all read recipe/include inputs by SHA-256.
The [dispositions](dos-command-input-role-dispositions.csv) account for the
85 resolved physical file roles.

The [observer](dos-command-input-observer.py) starts from the actual COMMAND,
DEBUG, EDLIN, EXE2BIN, GRAPHICS, KEYB and SHARE link responses, and the
reviewed inline link16 roots for APPEND, LOADFIX, MEM, NLSFUNC and SETVER.
`dos/v86/make.inc` supplies explicit `.asm.obj` and `.c.obj` inference using
`$*.asm` and `$*.c`, MASM and cl16. This is the source-to-object rule, not an
assumption from matching stems. In particular GRAPHICS' grcolprt.obj does
not list grcolprt.asm in its explicit dependencies; the real inference rule
still compiles it, and its actual INCLUDE reaches grcommon.asm.

The collector follows physical source INCLUDE/#include directives and
retains their original branch conditions. It ignores C block comments,
assembler semicolon comments and MASM COMMENT blocks, and preserves physical
line numbers. It does not execute NMAKE, evaluate every conditional or
compile a guest. A conditional historical input relationship justifies its
host-input file role; it does not prove a branch is active in a distributed
image. Search paths follow the retained utility working directory / include
order, rather than a global basename search.

## Reviewed conclusions

| Group | Reconciled paths | Evidence and retained boundary |
| --- | ---: | --- |
| COMMAND | 16 | Actual command.lnk objects and their include chains, including COMEQU, ENVDATA, IFEQU and TRANMSG. |
| DEBUG | 7 | Actual DEBUG response roots and 16-bit source inference. |
| EDLIN | 5 | Actual response roots and EDLIN's EDLEQU include. |
| EXE2BIN | 2 | E2BINIT and DISPLAY are the real response roots. |
| GRAPHICS | 14 | Thirteen linked modules plus shared GRCOMMON include; no directory-based inference. |
| KEYB | 12 | Actual KB16 response, nine object roots and selected declaration includes. |
| LOADFIX | 1 | loadfix.obj -> inline link16 loadfix.exc -> original COM conversion. |
| MEM | 10 | Eight linked object roots plus actual local declarations; original XMM ABI and host memory reporting remain runtime obligations. |
| NLSFUNC | 6 | Two inline-linked source modules and their four actual includes. |
| SETVER | 4 | Three pending C/header inputs plus the COUNTRY=usa-ms copy source. The copied local message.h and usa-ms/message.h are byte-identical. This is a conditional locale edge, not universal locale activation. |
| SHARE | 1 | Actual SHARE response and source inference. |
| Historical message-build tool | 2 | command/makefil0 uses cl16 with `-I.\$(COUNTRY)` then forcedos link to COUNTRY/buildmsg.exe. buildmsg.c includes bldmsg.h, selected from usa-ms when that COUNTRY is used. These files build the message tool; they are not linked into COMMAND.COM. |
| Obsolete DIR implementation | 1 | tcmd1a.asm explicitly states obsolete; readme.txt confirms it is no longer used; dir.asm names itself the replacement and actual command.lnk selects dir.obj. DIR functionality is retained. |
| Replaced message bindings | 3 | APPEND's eighteen old message/length exports are absent from its active root, which uses MSG_SERVICES/SYSDISPMSG. KEYB's eighteen old data/import names are either now locally defined IDs/data or unreferenced; its active ID-based message path is retained. |
| Source change-history note | 1 | e2bchng.inc contains comments plus only .xlist/.list controls and DOS EOF. It defines no executable code/data; the real EXE2BIN modules remain retained. |

The first eleven groups contain **78** source/include inputs. Together with
the two message-tool inputs, four obsolete modules/bindings and history note,
they resolve all 85 paths. Each is excluded only as its named historical file role in the
modern host implementation input denominator. Nothing is deleted or changed.
Every source remains byte-identical to the primary original; mirror diff is
zero. There is no meaningful restored-runtime-code percentage for these
excluded input roles, and no newly restored function is claimed.

The exact-delta checker preserves the 4,684-path inventory, 52 existing full
rows and all unrelated shard rows. Totals become **1,583 pending, 417 partial,
52 full and 2,632 excluded file roles**. Every mirror difference stays partial.

## Legacy message bindings: source-based retirement

Initial traversal left `append/appendm.asm`, `keyb/kbmsg.inc` and
`keyb/keybmsg.inc` unbound. Further source review establishes replacement,
rather than using nonselection as the reason for exclusion. APPEND's old
eighteen exported message/length symbols have no active references in its
single linked root; its old external references are commented out and the
retained MSG_SERVICES APPEND tables/SYSDISPMSG calls supply the real messages.
KEYB's old data and external-byte declarations name the same eighteen symbols.
The active KEYBCMD defines referenced names locally (for example ACT_KEYB=2
and INV_KEYB_Q=10), passes IDs in AX to SYSDISPMSG, and instantiates the
KEYB.CL1/CL2/CLA services. Old names not locally defined have no active use.
Its real KB16 response omits the older KEYBMSG object mentioned in comments.
These incompatible legacy data/import bindings are therefore retired file
roles, while APPEND/keyboard message capability is retained.

## Retained capability owners and limits

S12 retains COMMAND execution/search/environment, utility startup, memory
reporting and return/exit contracts. S13 retains the keyboard, codepage,
graphics/printing and device boundaries used by these utilities; S17 retains
applicable external redirector/VDD services. Immutable guest-image provenance
and actual host BOP/service behavior are separate from historical build-input
roles. Existing support, failure and lifecycle tests remain required. Original
utility no-op semantics are not silently upgraded to fully implemented host
features. The [recovery worklist](recovery-worklist.md) retains these owners.

No product or guest code changed and no guest was rebuilt. Host-input absence
is separately checked against actual worker commands/dependencies and the
retained WOW graph snapshots, not inferred from a DOS directory. The shared
ledger helper now applies exact reviewed cohorts with original CSV framing;
the checker independently replays source observation, original hashes,
delta/shard consistency and unchanged full/drift records.

The first collector run rejected uppercase `.OBJ` parsing in SHARE; this was
a collector error, corrected before the successful observation. It is not a
DOS runtime failure. An intermediate 77-row source-chain checkpoint was
expanded to 82 after direct source review established five additional roles,
then to 85 after the legacy message bindings were traced to their retained
replacement. Only the final 85-row result is the current classification checkpoint.
Owned process commands, errors and successful final results are retained in
`dos-command-input-runs/`. Documentation and preserved-package gates are
separate from runtime qualification. The held runtime candidate's SOL failures
are unaffected; this source audit does not qualify it or close S10/T473.
