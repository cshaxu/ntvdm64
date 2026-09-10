# T187 S4 passive-listener observation 001

Date: 2026-08-12  
Packet: M0 T187 S4  
Disposition: one observation consumed; ingress composition is the blocker.

## Inputs and outcome

The sole permitted v5 invocation used the T187 S3 passive-listener engine, retained T184 runner/shim/firmware/BYOB inputs, a 30-second watchdog and no retry. Its complete input manifest is `artifacts/analysis/t187-s4-passive-listener-observation-001/inputs.json`.

| Input | SHA-256 |
| --- | --- |
| runner | `67F50C468F29305A23A11E9BCF0E4CB8B1EBD909E78C251A399D74347ED41661` |
| shim | `4D8750B9D9C6FFFAF39643EFCF4668F8DDF83EC114166DFCE6B6435C07D80B59` |
| passive-listener engine | `EB55EFA8CBBB4E9B640D3AC2BFAF054AE551EB0EB793A597733E660F329CCA57` |
| v5 profile | `09E06FA81225E2308415DABFFC2BBBCAC03E6918185F7A139D7A97960662F129` |
| target | `69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC` |

Controller cleanup reported `Access denied`; no outcome JSON or child exit-code file exists. A later exact-name inspection found no `ntdos64-run`, `ntdos64-bochs`, or `ntdos64-bochs-engine` process. This is not a clean watchdog or guest-completion claim, and the observation must not be repeated.

The retained `stderr.log` is 247,623,357 bytes with SHA-256 `1DF910AA73CA7B2199963A3D8D9F9C1A3C7335370F47D9443F395BC0E56E3C3B`. Focused `rg` inspection found 476,135 `54:0C` listener records. Every sampled record has `cs=8dc8 ds=8dc8 dx=33d9`. Its physical endpoint is `(0x8dc8 << 4) + 0x33d9 = 0x91059`, within the one-megabyte real-mode aperture. Therefore the `DS:DX` endpoint does not explain the loop as an out-of-aperture boot-file preparation refusal.

The same records establish the ordering:

```text
adapter boundary passed through cs=8dc8 eip=00000732 ... bytes=c4c4540c
real-mode vector vector=06 ... target-cs=f000 target-ip=ff53
```

The CPU returns to the same `C4 C4 54 0C` instruction. The listener observes but deliberately does not consume it. The S3 macro set has `BX_NTVDM_ENABLE_STARTUP_TRANSACTION=0`; in `cpu/exception.cc`, both common startup dispatch and the CPU-result bridge are nested beneath that macro. The enabled listener thus precedes the consumer but cannot route the BOP to the host-service plane.

This is an engine ingress/composition blocker, not a COMMAND provider, DOS pointer, IRQ, BIOS, or device requirement. T187 closes its diagnostic purpose; its successor must recover one generic ingress consumer without selector-specific interception or a repeat observation.
