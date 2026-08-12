# T99 S2 boot-namespace consumer runtime checkpoint 001

## Question

After moving the finite boot namespace's normal DEM open, seek, read and
close lifecycle into its single provider state, which reached historical
contract is next on the contained first-profile guest path?

## Inputs

- Local OpenNT source: `base/mvdm/dos/dem/demdisp.c` and
  `base/mvdm/inc/dossvc.h`.
- T98 r5 retained Bochs build island and the current-adapter derivative
  generator.
- The finite v4 BYOB profile used by T99 S2.

## Procedure

The provider state was linked into the current adapter closure, with normal
`50:12`, `50:00`, `50:16`, and `50:02` routed only through that state. Focused
provider and runtime fixtures were run before generating the r2 derivative.
The source-built r2 binary was then run once with a five-second watchdog and
one-million-instruction benchmark budget.

## Observations

The focused fixtures passed the paired CONFIG/AUTOEXEC, normal open/seek/read/
close and pathname lifecycle, and rejected FCB routing. The narrow r2 relink
produced `ntdos64-t98-current-adapter.exe` with SHA-256
`08C20F430575B3A3DAEFF686E69F2F218E129699603845CCB29BFFDCD2ADE533` under
`artifacts/build/bochs-2.6-t99-boot-namespace-msvc-r2/`. It rebuilt adapter
and CLI objects only; no Bochs source, archive, device, firmware, or
configuration was rebuilt.

The sole runtime record is
`artifacts/analysis/t99-s2-boot-namespace-consumer-v4-probe-001/`:

| Ordered marker | Observed result |
| --- | --- |
| `54:0C` | Provider CONFIG transaction committed. |
| `50:12` | Normal DEM open was observed and resumed. |
| `50:00` | Normal DEM seek was observed and resumed. |
| `50:21` | Existing DEM IOCTL path was observed and resumed. |
| `50:42` | Observed, then passed through because no original-unavailable provider is linked to the runtime. |
| Terminal machine symptom | Bochs reported a prefetch direct-read veto at physical `0x00000000000af6af`; watchdog ended the run. |

The record contains 16 BOP observations and three committed transactions. It
does not reach `50:16` or `50:02` in this one real execution; those two normal
operations are source/fixture closed only.

## Interpretation and confidence

The provider is the sole owner of the admitted normal read-only lifecycle and
has reached its real consumer through open and seek. The immediate next
historical edge is not a missing fast-read implementation: `50:42`
`SVC_DEMFASTREAD` is one of the eight `demNotYetImplemented` entries in the
original DEM table. Its source-proven behavior is a no-op that clears CF.
Confidence is high for that owner/disposition and medium for any claim about
execution after it, because the current run terminates in independent Bochs
memory mechanics.

## Follow-up

T100 must compose the existing, whole eight-slot original-unavailable DEM
provider through the common ingress, registry and DEM-plane selection. It may
not implement FASTREAD/FASTWRITE, inspect a file, add a host namespace, or
special-case only `50:42`. A later real-path task will classify the resulting
post-resume machine stop.
