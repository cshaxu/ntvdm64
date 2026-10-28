# Shard 3: historical CTL3D reference inputs

The [read-only observer](ctl3d-source-observer.py) and
[observations](ctl3d-source-observations.json) verify the four files against
both original editions and the DOS/WOW source catalogs. The original retained
`readme.txt` explicitly describes this source tree as debugging material and
states CTL3DV2.DLL was binplaced from `bin86`. Original `ctl3d.rc` includes
`version.h` and declares `3dcheck.bmp` as a bitmap resource.

| File | Proposed | Bounded role |
| --- | --- | --- |
| `3dcheck.bmp` | excluded historical debug/resource-input role | Direct bitmap input of the retained CTL3D resource script. |
| `version.h` | excluded historical debug/build-input role | Direct version resource input of that script; special WOW and non-WOW branches differ. |
| `new3d.doc` | excluded historical documentation role | Original document containing CTL3D version notes; preserved for reference. |
| `scomp.dif` | excluded historical comparison-data role | Original recorded source comparison, not a compiled provider. |

These proposals exclude only file roles from the product build; they do not
exclude 3D control functionality or authorize rebuilding the guest. The
`src/mvdm/bin86/ctl3dv2.dll` SHA-256 is identical to the archived
`WINNT/SYSTEM32/CTL3DV2.DLL`. As observed, its direct
`O:/winnt/system32/CTL3DV2.DLL` path is absent; this does not establish an
actual loader failure or rule out application-local/other paths. Source to
archived binary lineage, use of the 3D bitmap, subclassing, input and cleanup
remain unproved. All four original input bytes are retained at 100%; a
whole-function behavior percentage is unmeasurable.
