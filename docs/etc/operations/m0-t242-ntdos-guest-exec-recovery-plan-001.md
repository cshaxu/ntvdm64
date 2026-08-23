# M0 T242 NTDOS Guest EXEC Recovery Plan

## Scope

Recover one original guest-owned local DOS EXEC → child → parent-return
profile. The product runs source-built `NTIO.SYS`, `NTDOS.SYS` and
`COMMAND.COM`; it does not translate their DOS process logic into C/C++.

## Ordered S Plan

| S | Package work | Exit criterion |
| --- | --- | --- |
| S1 | Freeze the original source/ABI/failure map and exact first profile. | `BOP-DEPENDENCY-092..101` identify the staged guest bytes, source owners, BOP boundaries and explicit transfers. |
| S2 | Establish a bounded guest-state observation fixture for source-built NTDOS: prove the selected image enters the original INT 21/EXEC route and expose only checked, typed observations of PDB/vector/parent state. | No host-side PSP/arena/JFN representation; source/build identity and pre/post guest-memory facts are reproducible. |
| S3 | Recover the local `AL=0` DOS-binary load/child/exit path by enabling only source-proven machine or existing DEM prerequisites. | Original `$Exec`, `$CREATE_PROCESS_DATA_BLOCK`, `$Exit/$Abort` execute in guest ownership; error exits retain source behavior. The admitted `50:42` fast-read seam may reuse an identical imported source body only because the original fast worker is absent from the admitted tree and its ABI/failure map is recorded. |
| S4 | Prove original parent restoration and ordinary continuation, then run the focused source-built guest regression. | PDB/parent/vector and exit-code facts match the original guest flow; no claim about host 32-bit child, Redirector, WOW, debugger, overlay or HMA branches. |
| S5 | Reconcile the package with tracker/COMMAND endpoints and perform one bounded native integration observation. | Every touched dependency has an owner/disposition; any stop before the declared profile is classified by owner rather than patched from trace. |

## Transfers

- `SVC_CMDCHECKBINARY` unknown/32-bit selection consumes COMMAND `54:07` and
  ultimately the later `cmdExec32` composition package; it is excluded from
  the local DOS-binary profile.
- Remote JFN/redirected file behavior transfers to Redirector.
- WOW/NE binary paths transfer to WOW16.
- `$Abort` symbol freeing transfers to VDD/debugger.
- HMA/A20-off, device/media, and any missing real-mode execution mechanics
  transfer to the selector-blind machine package. Its code must not acquire
  DOS/PSP/BOP semantics.

## Non-Goals

No adapter-owned DOS EXEC implementation, no new leaf BOP because a trace
arrives there, no CCPU/CSR recreation, and no widening from the one declared
local DOS child profile without a new tracker admission.
