# T221 S3 search owner-package admission

## Owner and service family

`src/opennt/base/mvdm/dos/dem/demsrch.c` owns pathname and FCB searches as
one stateful package:

| Services | Historical entry points | Buffer contract |
| --- | --- | --- |
| `50:09/0B` | `demFindFirst`, `demFindNext` | DTA, 43-byte result record, process/PDB continuation identity |
| `50:0A/0C` | `demFindFirstFCB`, `demFindNextFCB` | 53-byte SRCHBUF, 128-byte copied path, process/PDB continuation identity |

## Current state

The bx-vdm package already has one checked gather/write transaction and a
stateful search session. It preserves the distinct pathname and FCB input
layouts. However, first-search inputs currently come from the host namespace
or a declared immutable snapshot; there is no Overlay-visible enumeration
input, and Virtual is unavailable.

## Required recovery

The next implementation must not add per-service profile branches. It must
introduce one selector-blind search-entry source selected once by the provider
view:

- Direct: admitted host enumeration;
- Readonly: same read-only enumeration (search itself is non-mutating);
- Overlay: `overlay_namespace_view_v1_enumerate` output, preserving private
  tombstones, COW attributes and relocation;
- Virtual: explicit unavailable until an admitted virtual namespace exists.

Then both pathname and FCB first/next transactions consume the same selected
entry source while retaining their separate DTA/SRCHBUF encoders and original
no-more/failure results. Native trace remains out of scope until this complete
four-service regression closes.
