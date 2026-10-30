# Installed guest package and WIN87EM control

The owner requires immutable guest binaries. A read-only run of
`python tests/observation/verify-guest-media.py O:/winnt assets/winnt-guest-manifest.json`
reported 15 missing paths in the existing installation. The
[missing-file plan](missing-original-files.json) lists those paths and SHA-256
values. Each entry was independently read from `assets/winnt.zip`, whose hash
matches the guest manifest, and matched its manifest hash. Existing installed
files and the archive were not changed.

The previously passing Calculator arithmetic probe used the complete staged
package, which contains the original `system32/WIN87EM.DLL` with SHA-256
`30D6B965D45EDDF306268C9526B01494DF4841B8B530BAB2654C379E1CF49DB2`.
The unchanged original Calculator produced `0.5`, opened and closed About,
and exited; [receipt](complete-package-calc.txt).

Two controls copied that package into fresh build-owned directories while
omitting guest files from the copies. With all 15 missing installed paths
omitted, Calculator did not reach its main window; the worker showed a `File
Error` dialog and the probe returned 4. With only `WIN87EM.DLL` omitted, the
same outcome occurred: [15-file control](guest-absence-calc-probe.txt),
[single-file control](win87em-absence-calc-probe.txt). A full recursive file
hash comparison of the single-file control against the complete source stage
found exactly one missing file, `system32/WIN87EM.DLL`, with no extra or
changed files. All ten host-image hashes in the test manifests match the
published release. These are separate executions, and the dialog body and
guest module identity were not captured. The controlled difference supports
that WIN87EM's presence is required for this isolated Calculator startup;
it is not an exhaustive analysis of the other 14 missing files, all search
paths, or an observed run from the actual installed package.

The recorded static-import list also contains dependencies among the absent
files: TIMER and MCIOLE16 import MMSYSTEM; LANMAN imports NETAPI and PMSPL;
PMSPL imports NETAPI; VER imports LZEXPAND. The currently present files in
that manifest have no static import of these 15 module names. This is a
manifest-level observation only: it does not cover dynamic loading, guest
applications outside the manifest, alias resolution or behavioral acceptance.
The separately inspected original Calculator outside that manifest imports
WIN87EM and provides the runtime control described above.

A subsequent same-package single-file absence run extended only the test
observer's owned-dialog reporting. It captured the dialog body
`Cannot find WIN87EM.DLL` and again observed no Calculator main window; the
[negative receipt](win87em-absence-error-details.txt) and its ten-image
[manifest](win87em-absence-error-details-manifest.json) are retained. All ten
host hashes match the published release. This identifies the missing module
reported in the isolated control, not the precise file chosen by the loader
in the complete-package run or the behavior of the other 14 missing files.
With the same observer change, the [complete-package control](complete-package-error-report-control.txt)
still observed `1/2 = 0.5`, About closure and parent recovery, and the runner
reported normal launcher completion. Only the test observer changed; no product
or guest binary was rebuilt or modified.

An attempted deployment of the 15 original files was rejected by automatic
approval review because adding them would alter the existing guest installation.
The command was not executed. A build-owned, hash-verified preparation exists
only under `build/t473-s11/missing-guest-prepared`; it is not an installation
or a reason to bypass the review. User approval specific to the installation
would be needed before proposing that action again. Existing guest binaries
must not be changed or rebuilt.
