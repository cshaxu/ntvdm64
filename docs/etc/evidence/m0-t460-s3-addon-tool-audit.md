# M0 T460 S3 — independent add-on and launch-tool audit

## Scope and boundary

This record audits the concurrent independent add-ons and their publication
tools.  They are not MVDM/WOW production providers, and no original guest
media, guest build, host binary, or installed Windows tree was modified by the
audit.

## Findings and disposition

| Item | Audit result | Disposition |
| --- | --- | --- |
| Win1.01 mouse source rename | `mouse101.asm` was renamed to `mouse.asm`; normalized source comparison found no semantic line change. | Retain rename and update the manifest/source path. |
| Win3.1 mouse source rename | The only source change is the callback register save from 16-bit `pusha/popf` to 386 `pushad/pushfd`, matching the documented 386 asynchronous USER callback contract. | Retain; fresh NE structure build matches released `MOUSE31.DRV`. |
| Win3.1 launch repeat Apply | A pre-existing adjacent backup allowed an unknown active `MOUSE.DRV` to be overwritten. | Repair: repeated Apply now accepts only the exact released driver; it refuses unknown bytes while retaining the original adjacent backup. |
| Launch recovery policy | The authority text still described `PATCH`-resident original copies, while the accepted tool and recovery evidence use adjacent `.BAK` files. | Reconcile the policy to the tested adjacent-backup contract; `PATCH` remains profile-only. |
| Win95 compat provenance | The source referred to a missing `provenance.json`; the offline extractor declares an adapted MIT DS decoder. | Record exact driver source hashes separately from the offline-only patcher9x derivation, revision, license and helper hash. No third-party decoder enters the driver/runtime. |
| LNK utility | The offline writer validates bounds, refuses lossy encoding/unknown writable blocks, and uses atomic replacement. | Build and publish its AMD64 release artifact through the shared add-on manifest. |
| Win95 compatibility installer | Checks the released driver hash and one existing driver configuration entry, preserves the selected files and attributes, and restores them on a failed installation. | Retain explicit opt-in installation and fresh backup requirement. |
| Win95 menu preparation/application | Preparation writes a fresh build-owned plan; application checks original/candidate hashes, exact destinations, relative paths and duplicate destinations before replacing files, with saved originals and failure rollback. | Retain separate preparation/application; existing nonempty APPS95 content is refused. |

## Noninteractive evidence

Final review found that both Win95 installers wrote `install.json` after
setting their success flag, outside failure rollback. A journal write failure
could therefore leave installed files without a usable completed record.
Both now write that record inside the replacement transaction and mark
success only afterwards. Compatibility backups must also be outside PATCH.
The actual file operations are tested by
`tests/component-integration/win95_launch_install_test.ps1 -OutputRoot
<fresh-repository-build-directory>` using independently authored fixtures,
not a real guest installation. The successful `addon-install-r3` case covers
compatibility installation, two backup refusals, menu installation, preservation
of user-modified files, explicit restoration, and two injected journal-write
failures with actual filesystem rollback. Disk/process loss during rollback
is not a tested crash-recovery guarantee.

- `win31_launch_apply_recovery_test.ps1` passed against the immutable retail
  Win3.1 fixture.  It covers first Apply, repeat Apply, explicit refusal of
  unknown mouse bytes, legacy migration, profile integrity, and Unapply.
- Both mouse add-ons assembled and passed their NE ABI/structure checks.  The
  emitted hashes equal the corresponding release artifacts.
- `compat95.sys` rebuilt from NASM source with the manifest hash
  `BFD8E8D3AF62D4398BE414087B36E30EFF9798308387B5454C4B109ED4490D17`.
  Full and no-logo control builds passed the prefix-layout check.
- `LNK.EXE` passed CP936 round-trip, metadata preservation, malformed-input
  rejection, opaque-extra-data read-only handling, and atomic no-overwrite
  tests.  Its published SHA-256 is
  `B27F657281EB00BCA30314014AF20FE5D7E3ADB2E190934CB292A20F523E0B60`.
  The current release was rechecked with the actual executable in
  `build/t460-s3/addon-audit-lnk-r2`; all six test groups passed. Win3.1
  installation/recovery was rerun in `build/a2` and passed all assertions.
- Fresh mouse structure builds in `addon-audit-mouse31-r2` and
  `addon-audit-mouse101-r2` match the released drivers. Fresh full and
  compatibility-only Win95 builds reproduce the published 4856-byte driver
  and the retained 2435-byte control respectively. These are source/build
  identity checks, not new interactive guest acceptance.

## Submission boundary

The add-on source/tool submission is a bounded progress delivery within active
S3, not closure of its independent WOW32/PIC/keyboard runtime work. Add-on
release assets already match their fresh producers; this review changes no
original guest or default host-component dependency. The newly added menu
and logo observation/probe tools retain their explicit experimental status.
Their earlier owner-run guest evidence does not establish general Win95
compatibility. In particular the retained menu program-launch error 8 is not
resolved by offline LNK or installer tests.

## Remaining runtime boundary

The mouse build checks are structural only; their existing real guest runtime
evidence remains their respective T435/T436 records.  This audit neither
claims new guest runtime acceptance nor deploys any add-on to an installation.
