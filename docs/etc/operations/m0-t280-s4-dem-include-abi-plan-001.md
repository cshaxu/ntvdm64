# M0 T280 S4 — complete DEM include and ABI boundary ledger

S4 reads the original `dos/dem/sources` selection and all 16 selected DEM
bodies directly. It records each direct include with one final owner:

- original local DEM headers stay in `opennt-mvdm-host/dos/dem`;
- existing selected MVDM headers stay in `opennt-mvdm-support/inc`;
- public `nt_vdd.h` stays in `opennt-platform-abi`;
- compiler CRT and public SDK headers remain external platform headers; and
- historical private `winbasep.h` is an explicit `adapter-win32` include-only
  carrier gap: no DEM-reached declaration may be invented;
- historical private `vdm.h` is an explicit `adapter-win32 -> session` gap:
  `demsrch.c` reaches `VDMQUERYDIRINFO`, `VdmQueryDir`, and `NtVdmControl`.

The latter gap is not permission to copy a private header or rewrite a DEM
body. S5 must derive exactly the reached declarations from OpenNT source and
provide same-shaped adapter headers. This ledger also distinguishes guest
numeric addresses, which later use the session guest-memory mapping manager,
from normal host-local C pointers, which never do.
