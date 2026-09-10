# M0 T310 S25 P2 — C-VID generated-dispatch formal assembly

## Procedure

Outside the sandbox, generated a fresh graph at
`build/M0-T310/S25/formal-cvidc-x86` using
`New-T310OriginalSoftpcNinja.ps1 -Architecture x86` and Node 22.  Ran Ninja
for `original-softpc-cvidc.lib`, then ran the complete
`original-softpc-forced-closure.dll` ownership audit.

## Observations

- The direct original C-VID archive compiled all 41 C-VID object edges,
  including original `vglfunc.obj` and `evidfunc.obj`; it contains neither
  `cvidc_typed_video_vector.obj` nor `cvidc_typed_evid_tables.obj`.
- Completing the forced closure finished the full 369-edge selected Win32/x86
  CCPU40 graph.
- The forced-link log contains no unresolved selected C-VID table symbols:
  `C_Video`, `simple_evid`, `gricvid_evid`, `dith_evid`, `chain2_evid`,
  `unchained_evid`, `chain4_evid`, `read_mode0_evid`, or `simple_mark_evid`.
- The compiler retains historical generated-source diagnostics.  They are
  visible and are not suppressed by this packet; their non-x64 policy
  disposition is separate from the restored table ownership.

## Result

The selected original C-VID generated table contract is formally
source-assembled on x86.  This is not runtime evidence for a video device or
display.  Its mandatory integrated behavior verification remains S49.
