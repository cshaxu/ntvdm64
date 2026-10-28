# Shard 1: shared Win16 guest resource build inputs

This read-only group covers seven pending `.RES` files and one guest executable.
The [observer](guest-resource-components-observer.py) and
[receipt](guest-resource-components-observations.json) verify current file
bytes against both original editions and the two frozen catalogs. Each of the
eight paths has one DOS16 and two WOW32 catalog entries and retains 100% of its
original source-file bytes. They also record the exact seven recipe hashes and
point-in-time ZIP and direct installation-path image identities. No guest,
product, installed file or build tree was modified.

Original `makefile` recipes link or apply each `.RES` to its corresponding
16-bit image. The shell, system, WINOLDAP, WINSPOOL and mouse recipes refer to
shared `wow16/inc/common.ver` for version-resource dependencies; MMSYSTEM uses
it through its `.rcv` recipe. OLE server uses its own `olesvr.rcv` version
definition. MMSYSTEM additionally includes `mci.rc`, so its resource scope is
not limited to the MMSYSTEM top-level RC file. These are original build edges,
not proof of source-to-image provenance or runtime loading.

| Shard-1 path | Original output relationship | Proposed role and remaining boundary |
| --- | --- | --- |
| `wow16/mmsystem/MMSYSTEM.RES` | Applied to MMSYSTEM.DLL; source image matches ZIP. | Candidate historical resource-build input exclusion only. The direct installed DLL path was absent during this observation; locate its actual runtime provider and validate multimedia independently. |
| `wow16/shell/SHELL.RES` | Applied to SHELL.DLL; source, ZIP and direct installed image hashes match. | Candidate historical resource-build input exclusion only; shell resources and guest behavior still need proof. |
| `wow16/system/SYSTEM.RES` | Applied to system.exe before renaming to SYSTEM.DRV; all three image hashes match. | Candidate historical input exclusion only; system driver resources, timers and teardown remain open. |
| `wow16/winoldap/WINOLDAP.RES` | Applied to WINOLDAP.MOD. | **Pending**: this direct MOD path and matching ZIP member were absent; trace its actual packaging/consumer before deciding the input's role. |
| `wow16/winspool/WINSPOOL.RES` | Applied to WINSPOOL.EXE; source, ZIP and direct installed image hashes match. | Candidate historical resource-build input exclusion only; printing/spooler behavior and resources remain separately open. |
| `wow16/winspool/WINSPOOL.EXE` | Exact bytes match both original editions, ZIP and direct installed file. | **Partial candidate** as an immutable selected guest image; load, printer execution, failures and cleanup are not proven. |
| `wow16/ole/server/SRVR.RES` | Applied to OLESVR.DLL; ZIP contains that name. | Candidate historical resource-build input exclusion only; source-image and direct installed paths were absent, so OLE server packaging and behavior need separate trace. |
| `wow16/drivers/mouse/MOUSE.RES` | Applied to MOUSE.EXE, then copied to MOUSE.DRV; source, ZIP and direct installed image hashes match. | Candidate historical resource-build input exclusion only; mouse loading, capture, input and device behavior remain open. |

The direct-path absence statements are observations at this run, not proof a
file is absent elsewhere or that any feature fails. Image identity does not
prove that the retained `.RES` produced the image; the original makefile is
the recipe authority for that build relationship. Candidate exclusions refer
only to individual intermediate-file roles and require coordinator review;
they must not exclude multimedia, shell, timer, printer, OLE or mouse
capabilities. No numeric behavioral-restoration percentage is supported.
