# T198 S25 P3 DEM GSET DTA Native 001

## Question

Does the source-built NTIO/NTDOS continuation complete its first real
`SVC_DEMSETDTALOCATION` (`50:1B`) transaction through the current CPU5 native
composition, and what is the next unhandled boundary?

## Inputs

- Locked source-built `NTDOS.SYS`: 27,858 bytes, SHA-256
  `957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84`.
- The existing selector-blind CPU5 `bx-core` / `bx-mantle` finite-run fixture
  and the MSVC x64 `/MT` `bx-vdm` composition closure.
- OpenNT source: `base/mvdm/inc/dossvc.h` names `1Bh` as
  `SVC_DEMSETDTALOCATION`; `base/mvdm/dos/dem/demdisp.c` dispatches it to
  `demSetDTALocation`; and `base/mvdm/dos/dem/demgset.c` establishes the four
  guest locations, including the two-byte `DOSWOWDATA.lpSftAddr` read.

## Procedure

1. Rebuild the full `bx-vdm` closure and its focused composition test with
   MSVC x64 `/MT`.
2. Correct the composition gate to accept `50:1B` only after the existing DEM
   classifier has identified it as the original `GSET` component.  Its
   registration remains a prerequisite consumed by the selected namespace
   provider; it does not change selector routing or add a Bochs capability.
3. Rebuild, relink, and run the exact NTIO/NTDOS CPU5 fixture.  The observer
   accepts only the expected typed `RIP+4` completion and stops at the first
   later unimplemented BOP.

## Observation

The focused composition executable exits zero.  The full native run exits
zero and reports:

```
t198-s23 status=4 observed-5011=1 observed-503b-resume=1 observed-dta-resume=1 observed-stop=1 next=50:32 plane=0/0/1 terminal=1:0000:0ad2
```

`plane=0/0/1` means no pending read/action remains and the DTA registration is
present.  The observer's stop is therefore the first later boundary, not a
failed `50:1B` transaction.  `50:32` is original
`SVC_DEMSETHARDERRORINFO`, dispatched by OpenNT `demdisp.c` to
`demSetHardErrorInfo`.

## Interpretation

Confidence is high that the previous `50:1B` stop was an adapter
component-gating defect: the global DEM classifier already correctly called
the service GSET, while the namespace entry mistakenly rejected every
non-namespace component before reaching its existing DTA prerequisite.  The
correction preserves the original component identity and a fixed-width,
checked guest-RAM transaction.  No Bochs source, mantle lifecycle, BOP
selector decoding, guest image, host drive capability, or `50:32` provider
was enabled.

## Follow-up

Map `50:32` against its OpenNT implementation and provider disposition before
enabling anything further.  A future component-organization pass may extract
the retained DTA prerequisite into a dedicated GSET plane; that refactoring is
not required to establish this native transaction or to classify `50:32`.
