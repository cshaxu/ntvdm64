# T217 S10 — SYSINIT WFP Shape Observation

## Scope

This is a default-off extension of the existing DEM namespace-identity
observer. It records no pathname byte, guest address, host path, handle,
pointer, hash, routing choice, or provider result change. For only the second
normal `50:12` Open, it records four source-map predicates from the bounded
path already copied by the existing transaction: NUL-inclusive byte count,
drive-prefix class, root-separator class, and nonempty component count.

## Verification

The focused full DEM provider probe was rebuilt with MSVC x64 `/MT`:

```text
tools\Invoke-T202S4DemWholeProviderProbe.ps1
  -BuildRoot artifacts\build\t217-s10-dem-wfp-shape-provider-r1
```

It passed. Its declared `C:\\COMMAND.COM` input proves the default-off record
reports 15 bytes, C-drive prefix, backslash root separator and one component,
without changing the pre-existing declared-image Open outcome.

One fresh full-current-source native composition then ran at
`artifacts/build/t217-s10-dem-wfp-shape-native-r1`. It retained the existing
controlled `50:3D` terminal and printed only:

```text
t217-s10 wfp-shape bytes=32 drive-prefix=1 root-separator=1 components=3
```

The same run retained S6's C: admitted, namespace-attached,
`OTHER_ADMITTED` classification and the pre-existing DOS file-not-found
result. No pathname bytes were emitted.

## Interpretation

S9 proves the original bare SYSINIT literal is `\\COMMAND.COM`; original
`TransPath` resets a leading-root request to the selected drive root. Its
expected WFP shape is consequently 15 NUL-inclusive bytes, C prefix,
backslash root, and one component. The observed C-prefix/backslash facts
eliminate a drive-selection error and a slash-form incompatibility. The
32-byte/three-component result proves that the live `TransPath` result has
incorporated two additional directory components.

This does **not** disclose those components and does not prove their cause.
The source algorithm produces precisely that shape when it does not see a
leading root separator on the input after its drive-prefix processing. The
remaining candidates are therefore guest SYSINIT's live `DS:DX` command-name
source, its relocation/move lifetime, or an earlier guest write; the DEM
resolver and readonly namespace must not be modified to compensate.

## Recovery Disposition

| Rung | Disposition |
| --- | --- |
| Original OpenNT source | Retained as the normative `commnd` and `TransPath` owner. |
| Smallest seam | The observer is a temporary, default-off extension of the existing copied request; no semantic seam repair is selected. |
| External intrusion | Rejected. |
| Newly authored behavior | Rejected: manufacturing a declared-image match would conceal a guest-side source-realization fault. |

The next work is a static SYSINIT relocation/segment/data-lifetime map. It
must identify the original moves and the current boot-image placement that
can affect live `DS:DX` before any bounded guest-memory evidence or behavior
change is considered.
