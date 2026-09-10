# M0 T280 S4 — DEM private-header gap evidence

Only `demsrch.c` includes private historical headers in the selected DEM
package.

- `winbasep.h`, at line 16, has no referenced declaration in the complete DEM
  source body. It requires only a same-name, behavior-free `adapter-win32`
  include carrier to preserve the original include shape. It is not a reason
  to import the old private Base product surface.
- `vdm.h`, at line 17, is substantively reached at lines 1268–1283:
  `VDMQUERYDIRINFO`, `VdmQueryDir`, and `NtVdmControl`. The later adapter must
  preserve those declarations and route the directory-query request through
  session-owned capability state. It cannot expose a kernel VDM control or a
  raw guest/host pointer.

The repeated `GetVDMAddr` calls elsewhere in `demsrch.c` are not `vdm.h`
controls. They are existing historical SoftPC calls; once the provider body is
admitted, their numerical DOS address inputs must resolve via the session
guest-memory mapping manager before `adapter-softpc -> adapter-bochs` performs
a checked copy/lease operation.
