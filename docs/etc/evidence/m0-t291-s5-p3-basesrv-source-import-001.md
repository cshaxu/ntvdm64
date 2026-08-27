# M0 T291 S5 P3 — BaseSrv source import

## Question

Was the selected original BaseSrv/client VDM package imported without changing
its source identity, and did the shared host tracker receive every selected
file and boundary?

## Inputs and procedure

The P2 selected files were copied from the canonical OpenNT tree under their
upstream-relative `base/win32` paths. SHA-256 values were compared source to
destination after copy. The OpenNT-4.5 candidate was checked for each file;
all selected files are byte-identical across editions except `client/vdm.c`,
where the three-line difference is unrelated `_X86_`/`_MIPS_` macro spelling.
The canonical OpenNT file was selected for that coherent package.

## Imported source

- `src/opennt-host/base/win32/inc/{base.h,basevdm.h,basemsg.h}`;
- `src/opennt-host/base/win32/server/{basesrv.h,srvvdm.h,srvvdm.c,srvinit.c}`;
- `src/opennt-host/base/win32/client/{basedll.h,vdm.c}`; and
- `src/mvdm-platform-abi/source/public/sdk/inc/ntcsrsrv.h` as the exact shared
  CSR-server declaration carrier.

All ten paths have `byte-exact-verified` mirror identity in the shared file
recovery ledger. The source package has five explicit interface rows, one
package dependency row and one dual-architecture profile row. No source body
is linked, no selector is enabled and no `adapter-opennt-host` body exists.

## Result

The original BaseSrv/client VDM package is now a real `opennt-host` mirror,
not a missing-source assumption. The next work is its interface-by-interface
composition audit: preserve the original CSR message and wait/wake shapes,
then bind only specific unavailable CSR/private-NT calls through the smallest
package-private adapter or an explicit original failure path.

## Confidence

High for source identity and ownership; not a build or runtime claim.
