# T186 S1 corrected v5 observation classification 001

Date: 2026-08-12  
Packet: M0 T186 S1  
Disposition: complete one-shot observation; no normal guest completion claim.

## Fixed composition

The observation used the current runner, retained shim, and fixed four-file
Bochs bundle with the fresh T185 engine derivative. The bundle
`ntdos64-bochs.exe` at
`artifacts/build/current/t186-corrected-v5-bundle-r1/ntdos64-bochs.exe` has
SHA-256 `DE750579769772AFF1F5ECCB8AD1C4F8215A0FC4A8D21C438CDECFF8B9AE528A`.
It embeds the corrected engine linked from current adapter/CLI sources; no
Bochs source, object, archive, device, or configuration changed.

The sole controller result is retained at
`artifacts/analysis/t186-s3-corrected-v5-observation-001`. Its
`observation.json` records a 30,409 ms run, wrapper PID 15036, watchdog
termination, successful `taskkill /T`, exit code 1, no launch error, and the
hashes of both retained streams. There was no retry.

## Result

| Marker | Count |
| --- | ---: |
| COMMAND `54:0C` | 258,738 |
| COMMAND `54:01` | 0 |
| committed adapter transactions | 2 |
| selector `FE` / controlled stop / CPU-loop shutdown | 0 / 0 / 0 |
| pass-through and real-mode vector 06 at the loop | 258,737 each |

Thus T185's v5 selection projection repair is genuinely present in the
linked tuple but does not by itself make the first CONFIG.SYS request commit.
The previous projection-only explanation is disproved. The run does not reach
target delivery, `QUIT.COM`, normal DOS return, or CLI result transport.

## Source classification

`bx_ntvdm_boot_namespace_provider_v1_initialize` constructs one finite
provider containing COMMAND.COM, CONFIG.SYS, AUTOEXEC.BAT, TARGET and QUIT
when the v5 selection is valid. The corrected selection supplies the fields
that this initializer requires, and the focused adapter test proves that the
same provider can prepare `54:0C` as a one-write `C:\\CONFIG.SYS` transaction.

At runtime, `bx_ntvdm_adapter_runtime_v2_dispatch` accepts this service only
when all following short-circuit predicates hold: provider installed, COMMAND
plane gate accepts `54:0C`, boot-file preparation accepts the captured
exception/CPU/window, and the host session queues the transaction. The
retained listener records selector/service and several registers, but not DX,
and it does not record which predicate declined. The repeated event is
`54:0C` at real-mode `CS:IP 8DC8:0732`; its logged DS is `8DC8`. The service's
checked destination is `DS:DX`, so this evidence cannot safely select a
failing predicate or claim an address/memory defect.

This is a bounded observability/contract-classification gap, not authority to
change Bochs, broaden the profile, add DOS semantics, or add a fallback
recognizer. A future package may first establish a default-off, copied-value
diagnostic for the existing runtime predicates and then decide whether any
source-derived provider correction is warranted.

