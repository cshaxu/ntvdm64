# opennt-guest

Original OpenNT DOS and WOW16 guest source, guest artifacts and their
source-build inputs. Original prebuilt OpenNT artifacts remain the default
packaging inputs; source-built outputs are fallback/reproducibility inputs.

The retained original source layout is `dos-v86/` and `wow16/`.  These trees
were moved intact in T260 S4 with no content edit.  Historical documents may
refer to their former `src/opennt/base/mvdm/...` provenance paths; those are
source identities, not live product input paths.

The complete source-identical `base/mvdm/bin86/` guest companion package is
retained below `bin86/`. Its ten-file source identity and per-file role are
recorded in `docs/etc/evidence/m0-t263-s2-bin86-guest-input-admission-001.md`.
It is an image/configuration input package, not an in-process host provider
or an automatic runtime admission.
The exact original system-font staging input is retained below `wow16/fonts/`.
It preserves the source-relative OpenNT system font files used by the guest
bundle; T266/S3 records their selected source paths and hashes.

Original prebuilt DOS/V86 programs, system images, build libraries, objects
and MAP/SYM companions are retained at the same relative paths below
`dos-v86/`. They are byte-identical package/build inputs, not generated local
artifacts. Their per-file OpenNT/OpenNT-4.5 source paths and hashes are recorded in
`docs/etc/evidence/m0-t262-s3-opennt-guest-binary-import-001.md`.

T270/S2 independently rechecked the complete production component: 676
`dos-v86` files, 2 WOW16 include files, 42 system-font files and 10 `bin86`
files are all byte-identical to their selected original roots. There is no
guest overlay and no locally authored production source. The identity and
current 113-artifact staging result are recorded in
`docs/etc/evidence/m0-t270-s2-opennt-guest-mirror-closure-001.md`.

This component is guest content, never an in-process host provider library.
It may communicate with host behavior only through the guest's original
instruction/BOP/interrupt contracts.

## Local intrusion register

Every imported guest-source modification is registered here with original path,
source identity, exception identifier, necessity and focused verification. The
expected normal count is zero. The future public surface is the declared guest
image/artifact manifest, not C/C++ host headers.
