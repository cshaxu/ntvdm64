# M0 T420 S12 — SoftPC host package recovery

The original `softpc.new/host/src/sources` manifest declares 47 C entries
(46 distinct files; `nt_msscs.c` is repeated).  The selected product also
composes the original CCPU/DPMI bridge `nt_cprgs.c` and original worker entry
carrier `ntvdm_entry.c`; neither is a new mirror file.  This S compared every
same-named local/OpenNT host source (56 files) bytewise and after newline
normalization.  The closing ledger is 27 byte-exact files and 29 semantic
differences.  The latter are pre-existing, registered finite CCPU40,
Console, worker-lifecycle, guest-lease or unavailable-NT4 boundary records;
this S found no duplicate host policy that could be removed safely.

Six selected source files had no normalized-content difference and were
restored to the pinned OpenNT bytes: `fprt.c`, `nt_cga.c`, `nt_lpt.c`,
`nt_sec.c`, `nt_umb.c`, and `x86_emm.c`.  `stubs.c` also regained its
upstream CRLF/blank-line formatting while retaining only the prior
`MVDM-HOST-DIV-VDM-TIB-STORAGE` binding: the real monitor-side
`VDM_TIB` remains the sole storage owner, rather than the original
non-MONITOR four-byte placeholder.  No mirror algorithm, overlay, adapter
provider, or autonomous behavior was added.

The owner also supplied one concurrent product change:
`BASESRV_EMPTY_GRACE_MS` is now 10,000 ms in `basesrv-exe/main.c`.  It is not
an OpenNT-mirror change; it was rebuilt into the final broker and included in
the regression package.

The fresh S12 graph is `build/M0-T420/S12/formal-x86-001`.  The graph
generation completed under Win32/x86.  Its local Ninja runner again failed to
reap its first parallel compiler children, so the generated commands were
executed in the same VS x86 environment: all seven format-recovered host
units, the final `stubs.c`, and the changed BaseSrv entry were compiled;
`original-softpc-host-roots.lib` and all four products were relinked.
`Verify-VdmTibStorage.mjs` passed with `mvdm_vdm_tib.obj` as the only
4,208-byte owner.

The deployed `O:\winnt` hashes are:

- `run16.exe`: `5CAAA606651FDD0E3F2003BEA87ECCC621EE837BFA8F1D62C08E277FA1D7927F`
- `basesrv.exe`: `E0F09C36F5C8F7721E2097D9EF21987AA9AFF4B1F31CA3B5ABD9405DB2E07CF2`
- `dtmgr.exe`: `73546A1EF25DAE632116D3F15BE2EF4C0CC846BAB34BAD9376327E6D8E004CED`
- `ntvdm.exe`: `E4DB74A167D4DB69439CFB6BE8B9A2B726A2DA559A739101804BC36799D92347`

Final deployed checks passed direct MEM, repeat MEM, nested COMMAND→MEM,
`COMMAND /C`, and EDIT's scripted return (`exit=1`, the established EDIT
fixture result).  The separate batch EDIT instance was interrupted during
test cleanup and returned `FFFFFFFF`; it is not used as evidence because the
immediately preceding final-package standalone EDIT run passed.  The final
C-VID matrix passed 80x25, 45x34, 60x50 and 120x30, resize, mouse, and all
five short-window EDIT→MEM geometry observations.  Raw observations remain
under `O:\winnt\logs`.

S13 owns the next whole package, `dos/dem`; it must consume this worker-host
closure and may not reopen a settled host formatting or lifecycle boundary
without new evidence.
