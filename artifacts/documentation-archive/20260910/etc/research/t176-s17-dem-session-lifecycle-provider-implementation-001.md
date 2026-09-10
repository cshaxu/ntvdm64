# T176 S17 — DEM Session-Lifecycle Provider Implementation 001

## Question

Does the selected finite profile now handle the reached `50:3C` through the
common host-service structure while preserving the S16 exclusions?

## Change

- `bx_ntvdm_search_sessions_v1_release_pdb` releases only occupied finite
  continuation slots whose stored owner matches the copied 16-bit PSP/PDB.
- `boot_namespace_provider_v1_terminate_pdb` exposes that same-island action;
  no guest/Bochs/CLI ABI was added.
- `bx_ntvdm_dem_session_lifecycle_provider_v1` accepts only exact real-mode
  `50:3C` ingress, source-derived registry selection, and DEM namespace plane
  classification. It invokes the finite owner then performs ordinary `+4`
  resume without a register or flags delta.
- The registry records only `50:3C` as the already-audited
  source-derived-after-blocker disposition. The DEM plane remains the common
  classifier; the runtime reaches the provider through ingress, registry and
  plane records, not a byte-only recognizer.

## Focused verification

The new x86 test creates two private slots for PSP `1234h` and one for
`5678h`. Exact `50:3C` with `EBX=ABCD1234h` releases the two matching slots,
preserves and consumes the foreign slot, resumes from `0x400` to `0x404`, and
writes no GPR or flags delta. A second call proves absent-list success; a
wrong-vector call declines.

It compiled and ran successfully as
`artifacts/build/t176-s17-adapter-test/dem-session-lifecycle-test.exe`,
SHA-256 `3B508A9016579150BC8ABB17DEC5450C8470242126D8BBFAE543098C5BF556D1`.
The changed runtime translation unit also compiled x86 as
`adapter-runtime.o`, SHA-256
`3F10BB932910F123CCF9C8EEE82B9B9FCBB0402149043919C4A088CF2F63E353`.
The compiler emitted only pre-existing-style `wcscpy` deprecation warnings;
there were no new errors. This is adapter source/test closure, not an engine
relink or guest-runtime claim.

## Boundary result

The result restores only the current finite search-continuation lifetime. It
does not invoke VDD callbacks, `HostTerminatePDB`, floppy/fdisk cleanup,
ambient host search, FCB behavior or Bochs machinery. No runtime observation
was attempted, because the next question is link-composition closure rather
than trace-driven feature expansion.

## Follow-up

T176 S18 should establish the narrow current-adapter engine link closure for
this one provider, then admit at most one unchanged-profile observation. It
must retain the same source/owner gates and stop if the link requires Bochs,
device, VDD or host-capability expansion.
