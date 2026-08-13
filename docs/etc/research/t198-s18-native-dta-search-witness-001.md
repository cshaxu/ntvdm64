# T198 S18 Native DTA/Search Witness 001

## Question

Can the existing OpenNT-derived DEM DTA registration and pathname-search
branches complete as one native CPU5/Pentium-MMX real-mode sequence, without
injecting a register result or calling a provider directly from the fixture?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demgset.c`, `demSetDTALocation`: the source
  contract for `50:1B`.  In particular, `DS:AX` identifies the *variable
  holding* a DTA far pointer; it is not the DTA buffer itself.
- `src/bx-vdm/bx_ntvdm_dem_dta_service.c` and
  `src/bx-vdm/bx_ntvdm_dem_path_search_service_v1.c`: the admitted bounded
  adapter contract.
- `tests/bx-vdm/t198_s18_native_dta_search_fixture.cc`: the guest payload.
- The fresh S18 adapter closure in
  `artifacts/build/t198-s18-dta-x64-r1` and the existing CPU5 finite-machine
  source closure rebuilt below `artifacts/build/t198-s18-native-dta-search-r1`.

## Procedure

The fixture copies one opaque payload to physical `0x1000` and enters it at
`0100:0000`.  It sets `DS=0100`, then executes the genuine BOP byte forms:

1. `C4 C4 50 1B`, with `AX=0080`, `DX=0090`, `CX=00A0`, and `SI=0020`.
   Guest RAM at `DS:SI+32` supplies SFT offset `0050`.
2. `C4 C4 50 09`, with `DS:DX` pointing at the 128-byte `C:\\*.*` pathname.
   `DS:0080` contains the far DTA value `0100:00C0`; `DS:0090` contains the
   current PDB word `1234`.
3. `C4 C4 50 0B`, followed by `HLT`.

The adapter is bound once through its existing global composition session.
Each request crosses the selector-blind Bochs `#UD` record, the adapter
ingress/plane, and the existing checked mechanical RAM read/write executor.
The fixture does not supply a BOP outcome or call a plane/provider API itself.

The initial full build exceeded the command timeout only after it had created
the reusable native-core objects.  The remainder was compiled and linked in
that same build root with the recorded response file
`native-dta-search.rsp`; this did not change the source closure.

## Observations

`t198-s18-native-dta-search.exe` links as MSVC x64 `/MT` and exits zero.  Its
run log records CPU reset, CPU5 CPUID setup, and one-MiB RAM allocation before
the bounded guest reaches its terminal `HLT`.

## Interpretation And Confidence

The terminal `HLT` is reachable only after all three BOP forms receive typed
resume outcomes.  Thus the existing `50:1B` read/registration, `50:09`
gather-read/guest-DTA write, and `50:0B` DTA/PDB gather-read/result path each
completed in native execution.  This is high-confidence proof of the selected
boot-namespace plane's transaction wiring and guest-RAM preconditions.

It is not evidence of a DOS boot, a host-directory backend, arbitrary search
compatibility, interrupt/device operation, or continued execution beyond this
bounded terminal fixture.

## Follow-up

S18's implemented endpoints now all have either a native fixture or an
explicit non-runtime classification.  Any additional BOP family must be
admitted as a new global plane/package, not added as a trace-specific handler.
