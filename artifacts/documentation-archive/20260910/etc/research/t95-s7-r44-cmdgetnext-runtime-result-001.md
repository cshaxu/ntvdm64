# T95 S7 r44 BOP_CMD:01 Native Runtime Result

## Inputs and Procedure

Fresh r44 root was generated from the pinned r4 container and built once with
MSVC x86. `ntdos64-s7-runtime-trace.exe` is 2,560,000 bytes with SHA-256
`1E0788C512F24CAAC818BFCC3E2C883BC70AC0A698CBA32F9B25BD8D67EB60D8`.

The first v3 observation, 036, intentionally remains a pre-guest negative:
`cmd.exe` deletes an empty `set NAME=` variable, so strict launch-tail
presence correctly rejected installation. Observation 037 used the same
binary, ROMs and v3 profile, with source-built three-byte `QUIT.COM` copied as
identity-pinned `TARGET.COM` and the nonempty ignored tail `/q`. It ran with
benchmark 3 and a 30-second watchdog; the watchdog ended the process.

## Result

The observation contains 6,901 BOP identities and 11 committed transactions.
Its decisive sequence is:

| Log tick | Evidence | Meaning |
| --- | --- | --- |
| 12,369,76 | `selector=54`, `service=01`, `cs=95ab`, `rip=3c1` | COMMAND reached `SVC_CMDGETNEXTCMD`. |
| same tick | `multi-write transaction committed writes=12 bytes=2f` | The adapter committed the source-derived first-command transaction. |
| 12,373,89 | `selector=54`, `service=0e`, `rip=5d9` | COMMAND next queried `SVC_GETKBDLAYOUT`. |
| 12,373,98 onward | selector `06` at `073b:0740` | Unimplemented 54:0E fell through the historical invalid-op path; its repeated real-mode frames depleted the stack. |

Thus the trace proves 54:01's actual gather/commit/resume route. The later
selector-06 loop follows 54:0E and must not be attributed to 54:01.

## Boundary Conclusion

The next reachable requirement is a source-led audit of `SVC_GETKBDLAYOUT`
(`54:0E`), including its DS:SI receiver and DX result convention. OpenNT's
fixed EN-US first profile may return the documented no-KEYB result if the
source proves that outcome. It must not enable keyboard devices, load
KEYB.COM, expose host locale state, or add a generic BOP dispatcher. Empty
launch-tail transport is separately unresolved: the runner/observer must
eventually represent an intentionally empty value without weakening absent
value rejection.
