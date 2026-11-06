# T478 S15 — codepage mismatch belongs to Console ownership

Baseline: 2e836121b. Finding 30 remains a project defect, not a newly discovered
missing original mvdm-host implementation. This S closes the planned ownership
investigation, not the defect. No production code changes.

## Current trace

Original cmdexec.c ordinary native launch uses CREATE_SUSPENDED with
CREATE_DEFAULT_ERROR_MODE, without CREATE_NEW_CONSOLE. The child shares the
Console's input/output code pages. Current NTSRV worker_registry.c:721 creates
the text NTVWM carrier with CREATE_NEW_CONSOLE; its ordinary native targets
inherit that hidden Console. NTCON owns a separate Console.

NTVWM console_state.c:426-429 samples input/output CP, but repository searches
find no further consumer of these fields beyond their declaration. Its
presentation.c native_publication has screen/cursor/title/cells, no codepage
handoff. The protocol exposes CONSOLE_IO_CODE_PAGE as a query, not a setter.
NTCON console_frontend.c:331 returns its own GetConsoleCP/GetConsoleOutputCP;
frontend_session.c:311 decodes DOS text cells using its own output CP.
Thus successful native setters cannot update the separate frontend state.

The mvdm-host getter connection exists: compatibility headers map the API to
MvdmGetConsoleCP/MvdmGetConsoleOutputCP, the actual worker map contains both
console_client.obj providers, and the bound client sends the query over the
existing IO channel. cmdkeyb.c uses the input query. The original nt_fulsc.c
loadNativeBIOSfont output getter belongs to the excluded X86GFX branch
(DIV-510), not the current selected presentation path. Restoring that branch
would neither unify the two Consoles nor repair the demonstrated DOS cell
conversion. There is no missing standalone GRAFTABL algorithm needed to
explain the defect: a normal native Win32 CP setter already demonstrates it.

## Evidence boundary and disposition

The [archived isolated witness](../../../artifacts/mvdm-audit/shard-1/standalone-tool-chain-audit.md)
recorded a successful native output CP 850 setter while NTCON stayed 437, and
DOS byte 9B displayed as U+00A2 instead of CP850 U+00F8. This is historical
runtime evidence; the present stage rechecks current source/linked ownership,
not a fresh reproduction. It does not assert all DOS country/DBCS semantics.

Register the remaining defect under NTCON/NTVWM Console ownership and NTSRV
handoff. The [earlier architecture analysis](../../../artifacts/mvdm-audit/shard-1/codepage-concurrency-audit.md)
explains why transition-only copying is not full shared-Console parity, and
why shared Console buffers also affect input, CONOUT$ and control events.
No new hook, polling, CP authority, fallback policy or topology is introduced
under this mirror-recovery packet. A separately admitted owner task must choose
and verify the original Console contract; a working getter is not a fix.

All ten release hashes remain those verified at S14, package
3e2e62b9-6d16-442a-9286-e169566a75e7. No source/package bytes changed, no new
build or Full run is claimed. No guest change, O:/winnt write or push.
