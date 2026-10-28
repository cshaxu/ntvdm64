# Host consumers of guest-tree C/H: bounded build-graph observations

Replay `python artifacts/mvdm-audit/host-ch-scope/guest-tree-host-consumer-observe.py`
from the repository root and compare its JSON with
[`guest-tree-host-consumer-evidence.json`](guest-tree-host-consumer-evidence.json).
The recorded result was independently replayed without a mismatch; later
builds can legitimately change graph and dependency snapshot hashes.

The `host-ch-scope/inventory.csv` snapshot SHA-256 is
`BA07C656C6FE3691E0A4558BA6B74DC37EC822EED048EEC6CE19057CE74D831D`.
Its 473 `role-pending` rows under `wow16/` and `dos/v86/` were compared by
case-insensitive exact relative path, not basename, against each selected
Ninja graph and its recorded `ninja -t deps` output. No exact candidate path
was found in these four observed configurations:

| Tree | `build.ninja` SHA-256 | Recorded dependency groups (valid) |
| --- | --- | ---: |
| `msvc-x86-release/worker` | `835D8D25248FFBE2C4B4B147C056A43EF7C2F3624A85E09F141258F45FA0410C` | 639 (639) |
| `msvc-x86-release/wow32` | `03FE1C4AEB2CFB7434FA1471C55A3EF62652254615E630BA5861AE7F8729E278` | 229 (229) |
| `msvc-x64-release/worker` | `732526CCA6BAE92E860B6ED415EE96F0912C8CD3331ED87846EFF5BDAEFC44C5` | 53 (53) |
| `pkg-uuid-x86/worker` | `B1BC7B95C2A9DBC1A9DD8CC88931D86941EE146017C37A5ECCB6843FA80AC470` | 497 (497) |

All four read-only Ninja commands returned zero. Records establish only the
currently recorded dependency groups and explicitly enumerated graphs. They
do not establish coverage of unbuilt targets, other configurations, omitted
system headers, historical profiles or conditional branches. Zero matches is
not proof that the 473 files are unnecessary or a decision to exclude them.

There are at least 38 guest-header basenames also spelled in host C/H include
directives. These are ambiguity candidates, not evidence of actual sharing.
For example, the worker dependencies resolve host `bios.h` and `debug.h` to
`softpc.new/base/inc/`, rather than `dos/v86/h/bios.h` or
`wow16/inc/debug.h`. `wow32/precomp.h` spells `commdlg.h` and `dde.h`, but
the exact guest-tree paths are absent from the recorded WOW32 dependencies;
this observation alone does not identify the selected SDK copies.

One original source contains an explicit guest path:
`src/mvdm/inc/wowfax.h:76-79` includes `wow16/inc/print.h` only in the
`_WOWFAX16_` branch, which its own comment identifies as the 16-bit WOWFAX
case. The four graphs and recorded dependencies do not name that guest path;
the original conditional is nevertheless retained as evidence of dual-use
source and requires separate review if that profile is in scope.

No guest or host source was changed or compiled. These observations do not
prove runtime behavior or decide the per-file ledger classifications.
