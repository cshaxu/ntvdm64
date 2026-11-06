# T478 S30: CPU diagnostic semantic-neutrality disposition

Parent: 051ad1086. Finding 01 is already repaired in the current product.
This S changes test maintenance and evidence only; no production, mirror or
guest bytes change and no extra tolerance or diagnostic capability is added.

## Current source reconciliation

The six callers from the historical diagnostic-access inventory were reviewed
against the current source, not its old line numbers or old unfixed hashes.

| Caller | Current behavior and disposition |
| --- | --- |
| nt_bop.c / MS_bop_9 | Reports existing registers; original direct-access handler retained. No diagnostic stack reads. Original/current source witness both zero reads, one handler call and AX 5a31. |
| tape_io.c / cassette_io | Register-only enter/leave observations. PM/NTVDM AH=91 source witness preserves AH=91, AL=25, CF=7 and zero reads in both versions. Other original INT15 services retain their functional memory/device operations. |
| keybd_io.c / keyboard_io | Register-only AH/AL reports, original four-way dispatch; no effective_addr or SAS read in the function. Current selected 180-byte object body matches released NTVDM. Historical controlled keyboard witness remains historical; no new physical keyboard claim. |
| illegalp.c / illegal_op_int | Uses the four originally decoded frame bytes and registers for the report. The selected original false define() branch also reads one opcode for its existing panel selection: original/current source witnesses both five reads, two writes, one panel/dispatch, IP 1235. The old four-read witness modeled a different branch. The original formatter is stubbed, so five is the tested body count, not every read in a complete guest fault. |
| c_xfer.c / validate_far_dest | Reports the existing access-right byte and scalar metadata, without the old extra descriptor reads or translation. Actual selected CCPU tests retain #GP/error 8 and unchanged CR2 for the invalid cross-page descriptor, with trace off/matching/unmatched; valid control has no exception. |
| dpmiint.c / DpmiSwIntHandler | Target report uses handler metadata without mapping code. Controlled source witness covers missing handler, V86 reflection and rejected protected frame; actual frame construction and arbitrary nested DPMI exceptions are outside that witness. |

The project scalar reporter no longer calls the surveyed SAS/read/translation
interfaces. The existing actual-source reporter fixture aborts at translation
or SAS access and passes disabled, unmatched and three enabled records, with
LastError preserved. Adjacent CCPU caller arguments use existing registers,
addresses or values already supplied by functional operations. No new
observation-only memory read was identified in this bounded review.

The old source inventories explicitly excluded operational XMS fetches,
mode-transition frames and other original handler reads. They remain intact.
Removing them to reduce read counts would change original behavior.

## Executed verification

- Current CCPU far-exception fixture rebuilt from the selected cache and passed
  all four controls. Link maps establish original descriptor, paging, SAS and
  CPU owners and the test-only exception hook.
- Current selected CALLF/RETF fixture passed trace off, unmatched and matched;
  CPU/stack/result registers agree and only the matching scalar record appears.
- The observer error suite passed 53 checks in each of two processes, including
  enabled illegal-op output and failed output-path handling.
- The scalar fixture passed its disabled/unmatched/enabled no-guest-read and
  LastError checks.
- Six complete relocation-masked function bodies uniquely match the existing
  S29 package: far validation, three scalar reporters, keyboard BOP and illegal
  opcode handler. This proves selection separately from test execution.

Commands and frozen results are in build/t478-s30. The reusable entrypoints are
verify-diagnostic-stack-source.py --build-root, verify-dpmi-swint-diagnostic-source.py
--build-root, verify-ccpu-far-exception.py --build-root,
verify-ccpu-callf-trace.ps1, verify-softpc-observer-error.ps1 and
verify-softpc-trace-no-guest-reads.ps1. The PowerShell runners use the existing
x86 compiler driver and fresh build-owned output roots. All runs were owned
hidden background processes: 25088 (old fixture configuration failed), 5140
(source comparisons passed, old DPMI extraction failed), 26184 (remaining
checks passed), 33312 (image binding passed).

The first two failures were test maintenance: stale src/mvdm paths/historical
output roots, PM-off/define()-true configurations now excluded by current
#error markers, and a DPMI matcher unaware of #line plus the selected width-aware
stack helper. The scripts now use current selected configurations and fresh
outputs, preserving original/current comparisons. INT15 services not exercised
by AH=91 abort at controlled provider boundaries rather than silently succeed.
No #error marker or production branch was bypassed. GCC body witnesses remain
controlled tests; MSVC selected-object and image checks are separate.

Admission initially failed because the preceding S29 closure heading did not
match the governance format; its heading and link were corrected before
implementation. Final documentation governance and diff checks are required
for this P. The large CURRENT exception remains explicit until T478 final
reconciliation.

## Closure and limits

The durable [receipt](../../../artifacts/mvdm-audit/shard-3/diagnostic-s30-results.json)
records current source hashes/calls, object/image binding, outputs and ten
release hashes. No product edit requires new Full/publication: exact S29 package
7434fc0b-10e5-4081-a37f-b33031436b9e remains in assets/release, with its 16-gate
Full and real guest evidence unchanged. Test rebuilds are not release artifacts.
No O:/winnt write or push occurred.

S30 closes the known diagnostic regression finding as already resolved. This
is not proof of every nested callback or arbitrary enabled-diagnostic guest
exception; those coverage limits alone do not justify new product behavior.
Reopen for a concrete selected caller that adds observational memory access or
changes original exception/result ordering. Broader T478 inventory reconciliation
and subsequent findings remain open. Earlier unrelated startup diagnostics and
owner-excluded DOS4GW/NESticle remain outside this disposition.
