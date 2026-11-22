# T484 S2 P6 native host family

Native CMake now owns NTVWM, NTCON, NTMON, Run16 and NTSRV. Their component
lists preserve source profiles, archive members and ordered link inputs.
The old PowerShell generator and x64 configuration's x86 reference graph
dependency are removed. Native MIDL, font generation and live extraction of
the original VDM configuration functions remain explicit dependencies. NXVM
membership, source hashes and license identity still come from its import
manifest. No mirror or original guest changes are included.

## Evidence

- `test/build/test-native-family-migration.py` compares actual Ninja commands
  against the retained prior graphs. Worker/frontend/monitor/launcher/service
  preserve respectively75/85/29/70/71 C compilation contexts,20/12/6/19/16
  archives and4/17/7/6/7 links. All330 compilation contexts,73 archives and41
  links match in source/profile and ordered membership. CMake object names
  have stable output aliases for existing archive and test consumers.
- Native initialization PID6312 compiled all five products. The first full
  check PID30312 passed but its historical runner unnecessarily invoked the
  old reference tree afterward. That runner call was removed. Final cached
  eleven-image build PID2756 passed17:43:28Z--17:44:06Z on2026-11-22.
- Publication PID21192 passed17:44:30Z--17:44:47Z. Both assets/release and
  O:/winnt/system32 carry package972c2293-4961-4c9e-b1eb-bd690b0ad3a4;
  all22 published hashes match. Four raw images are byte-identical to P5;
  seven differ only in PE timestamps. No diagnostic path difference appears
  in this batch. Staged package identity is separate from raw input identity.
- Post-publication PID10444 built all41 links and passed nine native checks:
  worker control, frontend arguments/window library, monitor layout/management,
  native service layout/idle/reservation and application search.
- Its direct execution-lifetime invocation failed because the build directory
  lacked the required Hook/Run16 runtime companions. This was an incorrect
  invocation, not a passing test. The existing private-desktop fixture runner
  supplies those actual published companions. PID12520 passed presentation612,
  input-return605 and lifetime1084 assertions, zero failures; the lifetime
  check reports zero remaining handles. Presentation fixtures explicitly do
  not claim full production activation coverage.
- The same run passed final command comparison, frontend/launcher ownership
  (68/60 sources and five negative controls), linked C-VID, eight live graph
  A/B boundary checks (326 objects),544 authored/generated macro-profile
  commands and22 published hashes. The former reference graph is no longer
  counted. The macro count drops by one because the native worker's extracted
  original configuration source now has the existing original-source path
  classification; no compilation unit was removed.

[Compact receipts](../../../artifacts/build-migration/t484/s2-native-family/profiles.json)
and adjacent baseline graphs retain the comparison inputs. Those graphs are
audit evidence only, never build inputs. The initial missing generated-config
and unreferenced test-target mistakes were corrected before final validation.

No new Full runtime pass or interpreter-free clean bootstrap is claimed.
Documentation, relative-link and diff gates passed in PID3844.
P5's additional Clipboard baseline failure remains recorded in its evidence.
S2 stays active: the original x86 worker graph generator remains, and S3-S5
still own addon/test producers, publication migration and final acceptance.
The containing P6 commit is the delivery identity. Mirror delta is zero.
