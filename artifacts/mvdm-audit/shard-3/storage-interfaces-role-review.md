# SoftPC storage interfaces — shard 3

Three current pending entries `softpc.new/base/inc/{cdrom_fn,disk_geo,gfirflop}.h` are reviewed together. `storage-interfaces-observer.py` and captured JSON hash these files against both original editions and related selected sources, normalize retained Ninja C dependencies to worker root and record map symbols. All three headers and gfi.c match both references; current nt_rflop.c and nt_fdisk.c differ. No storage device, guest, or product execution occurred.

| file | proposed status | source role and current selection boundary |
|---|---|---|
| `cdrom_fn.h` | pending | Historical CD addressing, TOC and device status declarations. No exact matching dependency in the captured worker C-deps, and no checked direct selected consumer; this does not prove CD-ROM functionality is absent. Original header names are not evidence of the current optical provider. |
| `disk_geo.h` | pending | Historical geometry constants (17 sectors, 4 heads and 512 bytes are formulas/defaults), not a dynamic query contract. `nt_fdisk.c` has independently defined similarly named constants and its current map contains host_fdisk_get_params, but this does not show that it includes this header or that all media have those geometries. |
| `gfirflop.h` | pending | Historical host_gfi_rdiskette declarations. `base/disks/gfi.c:287-325` forwards validate/change/activate operations to same-named host functions; current `host/src/nt_rflop.c:385-405` opens or terminates the drive path, and the observed map associates host_gfi_rdiskette_active, nt_floppy_read and nt_floppy_close with nt_rflop.obj. No exact header edge in retained snapshot, so its declarations must not be equated to the declarations actually used by selected sources without include-resolution proof. |

The original selected `nt_fdisk.c:164-173` bodies named host_fdisk_rd/wt explicitly return zero; identical mapped RVAs for these symbols do not prove working disk I/O. Other storage paths may own actual DOS file/device operations and must be traced separately. Existing genuine floppy, optical and fixed-disk media behavior, media-change/locking, I/O error and teardown are not verified by this review; no claim of total device failure is made.

The normalized deps file, map and source snapshots are independently hashed; absence of an edge applies only to that worker C-dependency snapshot and is not absence of capability. No restored-behavior percentage is inferred.
