# Part 1 MVDM: bin86 guest binaries, configuration and help family

All 17 assigned `bin86/*` paths have fresh per-file source/reference hashes, actual ZIP basename matches, and exact O:/winnt/system32 observations in `bin86-evidence.json`; replay `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/nine-way/part-1/bin86-observer.py`. The original `bin86/makefile:12-26` binplaces most runtime binaries and default configuration. Hash comparisons do not establish build lineage or functionality; installed search is limited to the exact named paths and no guest or provider was run.

| Assigned path | Role and bounded disposition |
| --- | --- |
| autoexec.nt | Default DOS startup script, equal in both primary originals and ZIP; exact installed system32 file differs. Treat installed file as user configuration, do not overwrite or treat hash difference as a defect. |
| config.nt | Default DOS startup configuration, equal in both originals and ZIP; installed system32 differs. Preserve user configuration and trace actual launch selection. |
| country.sys | Original bin86 country data matches OpenNT-4.5; ZIP and installed system32 instead match `dos/v86/dev/country/COUNTRY.SYS`, a different byte identity. Keep variant and locale behavior pending; do not claim same hash or replace installed file. |
| ctl3dv2.dll | Original special WOW control library, matches OpenNT-4.5 and ZIP; exact installed path absent. See `shard-1/guest-ctl3d-review.md`; actual app load and control cleanup untested. |
| ctl3dv2.map | Historical symbol map, matches both references; no matching basename in ZIP/system32. Not runtime and not proof of ZIP DLL provenance. |
| i386/win87em.dll | Historical x86 Win16 x87 emulator library, matches OpenNT-4.5 and ZIP; exact installed system32 path absent. See `shard-1/guest-win87-variants-review.md`; selection and numeric behavior need proof. |
| lanman.drv | Historical Win16 network provider, matches both references and ZIP; exact installed system32 path absent. WFWNET source resolves LANMAN entrypoints; actual provider and job cleanup untested. |
| lzexpand.dll | Historical Win16 decompression library, matches OpenNT-4.5 and ZIP; installed exact path absent. Loading and compressed-file boundaries untested. |
| netapi.dll | Win16 network API library, matches OpenNT-4.5 and ZIP; installed exact path absent. Do not confuse with native NETAPI32; provider and callback behavior untested. |
| olecli.dll | Win16 OLE client library, matches OpenNT-4.5, ZIP and exact installed system32 file. Activation, ownership and teardown still untested. |
| pifedit.hlp | PIF editor help data, matches both primary references; no matching ZIP/system32 basename. Historical PIFEDIT GUI is not identical to the project PIF format tool. |
| pmspl.dll | Win16 print/spool helper, matches OpenNT-4.5 and ZIP; exact installed path absent. Actual provider/print lifecycle unverified. |
| regedit.hlp | Registry editor help data, matches both references; no ZIP/system32 basename. Original regedit RC names it, help execution unverified. |
| system.ini | Historical guest system defaults, matches both original editions. ZIP `WINNT/SYSTEM.INI` and installed O:/winnt/system.ini match each other but differ from historical bin86 file. Preserve installed configuration; do not infer a failure. |
| ver.dll | Historical Win16 version/installation helper, matches OpenNT-4.5 and ZIP; exact installed path absent. Version API and failure handling untested. |
| winhelp.exe | Historical Win16 help executable, matches OpenNT-4.5 and ZIP; exact installed system32 absent. Guest invocation, reuse and exit untested. |
| write.hlp | Historical WRITE help data matches both references; ZIP `WINNT/WRITE.HLP` and installed O:/winnt/write.hlp match each other but differ from bin86 reference. Keep variant provenance and actual help behavior pending. |

Exact absence is not an exhaustive search of other guest disks, application directories or alternate configurations. All keys retain specific runtime/variant obligations; image equality and original recipe selection are evidence, not completion. No configuration, guest, product or shared ledger was modified.
