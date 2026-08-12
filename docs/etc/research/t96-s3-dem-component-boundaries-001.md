# M0 T96 S3 DEM Component Boundaries 001

## Method

Starting from the complete DEM provider map, pending handlers are grouped by
the source file that defines the original OpenNT handler. This is a component
boundary map, not a claim that a group is independently linkable.

## Original components and CLI disposition direction

| Original module | Services | Current direction |
| --- | ---: | --- |
| demfile.c, demhndl.c, demdir.c, demsrch.c | 24 | DOS namespace/file/dir/search plane. Preserve OpenNT ordering and error contracts, but defer provider admission until contained read-only namespace, file-token, pathname/FCB search contracts are complete. Host mutation is out of profile. |
| demfcb.c | 8 | FCB compatibility plane. It shares DOS file structures and guest-memory contracts with file/search; defer as one group, not as FCB one-offs. |
| demgset.c | 13 | Drive/time/system-information plane. Some operations may become contained CLI capability substitutions, but registry/current-directory mutation and drive/DPB assumptions require individual source-contract review within this group. |
| demdasd.c | 2 | Absolute disk I/O. `t96-s3-demdasd-source-contract-001.md` closes it as one raw block-media provider family shared with IOCTL AL=0D/11; defer it whole because BDS, geometry, INT 13h/SAS and physical media are outside the read-only CLI namespace. |
| demioctl.c | 1 | Device/drive IOCTL. The full source contract is recorded in `t96-s3-demioctl-source-contract-001.md`: AL=08/09 form one immutable projected-drive metadata slice; AL=0D/11 form one raw block-media slice and remain deferred. |
| demerror.c, demlock.c | 3 | DOS error, retry, and locking state. Defer until their guest structure and process-state ownership are closed. |
| demmisc.c | 13 | Startup, symbols, VDM/WOW, console, debug, output and termination miscellany. Split only by original dependency evidence; do not import VDD/WOW/monitor semantics into the adapter. |
| demdisp.c | 1 | Set-V86-kernel-address historic monitor bridge. Defer to the native monitor/CPU owner; adapter may not synthesize it. |
| demNotYetImplemented | 8 | Source-proven no-implementation contract. The single DEM provider slice owns resume plus CF-clear only. |
| dispatcher sentinel | 1 | Not callable. |

## Resulting hard boundaries

- The adapter can own fixed-width selection, checked CPU result, and contained
  CLI capability adaptation. It cannot own DOS kernel structures, CCPU/SAS,
  raw disks, VDD/WOW, V86 switching, or host-wide current-directory mutation.
- Bochs owns CPU and machine mechanics. It does not gain DEM file, error, or
  drive policy.
- The CLI capability layer may provide an immutable projected namespace and
  declared drive set, but only after the relevant original handler group is
  admitted.

## Next provider order

The first functional DEM group is not selected by trace frequency. It must be
the first group whose complete original contract can be bounded by the
read-only CLI profile. Current evidence points to the namespace/file/search
family, but its existing T95 file/search limitations remain a stop condition
until the group-level ABI review is complete.
