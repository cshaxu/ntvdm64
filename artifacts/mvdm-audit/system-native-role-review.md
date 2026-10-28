# Original NT system-provider reconciliation

This S10 checkpoint follows `eb218c706`. It resolves the eight remaining
`base/system` file roles: **one full leaf, five partial providers and two
alternative/revision file exclusions**. The [per-file ledger](system-native-role-dispositions.csv)
retains original identities, exact scope, measurement basis and follow-up.
No original guest or production source/image is modified.

## Original selection and actual linked code

The original byte-identical `base/system/sources` selects at_dma, cmosnt,
ica, idetect, illegalp, qevnt, rom, timer, timestrb, dummy_nt and unexp_nt.
The [observer](system-image-observer.py) verifies the six still-pending
selected source pairs, actual compile/archive edges, recorded VALID dependency
blocks and **77 mapped functions**. Every non-relocated byte matches at its
real mapped address in the candidate; complete executable sections match the
cached map-producing worker. Call relocation targets are not all validated by
this function-byte comparison. [Observations](system-image-observations.json)
bind the exact source, object, library, map, image and recipe hashes.

| File | Disposition | Actual owner and retained boundary |
| --- | --- | --- |
| at_dma.c | Partial | Original two-controller/page-port registration and memory-transfer state. A guest status-port read does not prove masked/request transfers, byte/word channels, auto-init, decrement/page wrap, LIM interaction or failure/reset ownership. |
| cmosnt.c | Partial | Original NT CMOS/RTC ports, pending cause/IRQ state and EOI callback. Fresh valid-RAM/periodic-publication and native callback tests are bounded; calendar/alarm/format/time-change, asynchronous register-C consumption and full IRQ8/BOP70 completion remain open. |
| dummy_nt.c | Full selected leaf | The complete original dummy_int function has an empty body and no state, resources, callbacks or failure branches. Its real selected object/image contains exactly three bytes `C2 00 00` (near RET 0), with no relocations. BOP01 resolves to that owner and real guest positive/negative witnesses exercise it. |
| timer.c | Partial | Original NT PIT/counter/gate/latch and timer EOI provider. Real BIOS tick advance and native PIC cancellation do not prove every mode, reprogramming, coalescing, time discontinuity or lifecycle overlap. |
| timestrb.c | Partial | Original NT video blank/refresh strobe remains selected. Non-NT time_tick/rtc_tick work is explicitly gated out in the original source; NT host timing calls RtcTick separately. Verify exact nineteen-strobe clear, enable refresh/reset and teardown, rather than wiring duplicate clock owners. |
| unexp_nt.c | Partial | Original unexpected-interrupt ISR/mask/EOI and BIOS cause-byte handler. Hardware/software, master/slave, cascade preservation, repeats and actual callback paths remain untested here and must accompany the D11 investigation. |
| cmos.c | Excluded provider file | Original NT recipe explicitly selects cmosnt.c. That provider directly registers the ports and uses static initialization/post instead of the generic central CMOS file. Port/RTC capability is retained; selecting both would create competing owners, not restore a missing service. |
| ica.old | Excluded earlier revision file | The retained revision is 1.31; the original NT recipe selects ica.c revision 1.38. Current PIC behavior and its registered host correction remain selected and partial. No PIC capability is excluded with this older physical file. |

The generic CMOS helper `cmos_io_attach` is folded directly into NT
`cmos_init`; the NT source implements shutdown-byte clearing too. The
non-NT `rtc_tick` call in time_strobe is guarded out, while nt_timer calls
the NT RtcTick. Absence of those generic names from the NT map is therefore
not proof of a missing CMOS device. The NT source intentionally makes central
CMOS file pickup/update no-ops; this is original behavior, not an invented
project fallback.

All eight current files match the primary original byte-for-byte. Text
retention is 100 percent. For the five partial device files, code/image
selection and bounded tests do not yield a whole-device behavior/branch or
lifecycle percentage; that unmeasurable basis is explicit. For dummy_nt.c,
**100 percent** applies only to its complete selected no-state leaf contract:
one unconditional return, no other runtime body or owned lifetime. The
unchanged prior 26 executable-leaf and 26 declaration rows remain intact;
the leaf index gains this one entry. No whole interrupt subsystem, firmware
vector installation, invalid caller state or other device is accepted by it.

## Real guest and native verification

All tests use the existing medium-integrity modern-pwsh private-desktop runner,
short physical runtime stage and package isolation. There is no Computer Use
or UI automation, and no original guest is rebuilt.

```powershell
tests/observation/verify-ccpu-sign-flags-guest.ps1 `
  -Workload SystemDevices -PackageRoot <pinned-package> `
  -Observer build/tools/console-startup-observer.exe `
  -LogonHelper build/t449-s2-message-construction-r3/logon-env.dll `
  -BuildRoot <fresh-repo-build-directory>
```

`-Workload DummyInt` selects the independent BOP01 witness; `-NegativeOracle`
selects the corresponding deliberate wrong expectation. Exact commands,
parent integrity/PIDs, source/tool/runtime hashes and raw results are retained
under `system-runs/`.

* SystemDevices reruns the retained [system probe](../../tests/observation/system_capability.asm):
  BIOS tick advance, CMOS valid-RAM bit, actual RTC periodic cause publication,
  PIC mask round-trip, selected DMA status read and readable reset ROM.
  Before enabling PIE it masks IRQ8 and clears an older register-C cause, so
  the BIOS cannot consume the newly observed flag first. It restores register
  B and the slave mask; other CMOS fields are not used to alter host time.
  This deliberately proves publication, **not** IRQ8 delivery/BOP70 completion.
  Success text is split into short lines without removing assertions.
* DummyInt uses the [register/flag witness](../../tests/observation/bios_dispatch_probe.asm)
  in a separate compile-time mode. Sixteen BOP01 calls per package cover four
  defined flag patterns and check EAX's upper word, all tested 32-bit general
  registers, SP and DS/ES. The actual leaf's exact RET 0/no-relocation code
  establishes its no-state return for the canonical caller ABI; the guest
  confirms the selected dispatch edge rather than only an unused helper.
* Both workloads pass on the accepted and candidate packages. Wrong CMOS
  valid-RAM expectations fail at S24_CMOS_FAIL after the tick check; wrong
  dummy AX expectations fail at phase D. Both packages reject both oracles.
* Two additional accepted-package BIOS-dispatch positive/wrong-oracle runs
  pass. Its normal COM bytes are identical to the earlier BIOS checkpoint
  (`F6E667ABFE8FE83EE9CA31B31C66D7460D662B644C4153724000172D17CAFEE3`),
  so the new conditional dummy mode does not replace or weaken the default
  836-case workload.

The cached PIC fixture initially needed three Ninja steps: fixture-root
archive, unchanged fixture source and final fixture link. It was rebuilt
through the existing MSVC driver, Ninja and repository-routed ccache, without
clearing caches. The second incremental check reports no work. Production
system/BIOS archives, six source objects and cached NTVDM/map hashes are
unchanged. The fresh fixture reports `PIC_RESET_EOI assertions=24 failures=0`,
including actual timer/RTC/mouse callbacks behind its declared host boundary.
Its seeded states are not proof of all asynchronous hardware races.

## Corrections found in the audit harness

The initial observer incorrectly assumed a one-byte C3 return. Actual COFF
bytes showed RET 0 instead; the guard now checks exactly C20000. That was an
observer assumption failure, not a runtime defect. A supervisor array-capture
error was corrected before fixture execution.

The native fixture writes results to stderr. Review found that an earlier
stdout-only assertion could miss an empty-output condition. The final run
checks combined output; the guest runner now reads explicit strings and
rejects missing/blank report or console output. All ten guest/oracle contracts
and the native fixture were rerun under these stronger guards in
`system-integration-r5`. Earlier runs are retained as diagnostics, not final
acceptance. No assertion was relaxed to make a failing product pass.

The captured manifest retains all final R5 metadata and the earlier supervisor,
build and failure diagnostics; duplicate provisional R4 guest subdirectories
are omitted. Binary test media stays in disposable build storage. The final
[delivery result](system-delivery/stdout.txt) verifies the captured byte hashes,
all ten final outcomes and input identities, native fixture and clean increment,
unchanged published packages, exact eight-row delta and documentation gates.

## Classification and remaining work

The exact eight-row checker proves one pending->full, five pending->partial
and two pending->excluded transitions, prior full-index preservation, source
identity and all unrelated shard rows. Totals become **1,568 pending, 429
partial, 53 full and 2,634 excluded physical file roles**. Mirror differences
remain partial; the selected PIC's registered delta is unaffected.

S13 retains complete DMA, CMOS/RTC/PIT, video-strobe and unexpected/slave-IRQ
lifecycle/failure validation in the [worklist](recovery-worklist.md). No new
device correction or semantic concession follows merely from source concerns.
The known D11 port issue is still not proved by these tests. Full DOS/WOW/
Win3.x compatibility and the held candidate's earlier SOL failures remain
outside this bounded witness. The source audit changes no product image and
does not authorize a new runtime release or S10/T473 closure.
