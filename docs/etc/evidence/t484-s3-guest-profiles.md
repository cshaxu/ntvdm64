# T484 S3 P188: fixed DOS regression profiles

Five live focused regressions replace direct assembler calls with the existing
CMD fixture graph. All13 previously selected source/define combinations are
explicitly declared; there is no arbitrary macro or source-input interface.

| Caller | Preserved producer profiles and purpose |
| --- | --- |
| DEM pressure | NoDelete/NoPressure/Deleted: deletion and resource-pressure controls |
| DPMI debug context | WATCH with DELIVERY_GROUPS; LIFE without defines: debug delivery/removal and lifetime |
| DPMI INT21 state | low/high with HIGH_BUFFER=1 only for high: direct/nested state preservation |
| XMS block | Boundaries; Existing with DEFAULT_INT15; InvalidRange; InvalidDescriptor with INVALID_DESCRIPTOR |
| Video7 flags | default/all with VIDEO7_ALL_MODES=1 only for all |

The existing XMS fault predicate and expected termination remain. The original
DOSX status-query exclusion in the debug test is retained: delivery coverage
does not claim status-query acceptance. Guest sources, runtime assertions,
cleanup and selected destination names are unchanged. No original guest is
modified or rebuilt. These are focused tests, not a new Full result.

## Evidence

`build/t484-s3-guest-profiles/run.ps1`, PID15532, exit0:

- All13 actual outputs exactly match the previous source/define invocations;
  compared inputs and hashes are recorded in images.json.
- Repeated native build preserves all13 output modification times.
- All five changed callers parse and no longer invoke NASM.

The old assembler calls exist only in this delivery's comparison oracle.
Cached full and11 raw/22 published hash checks are recorded by
`delivery-result.json` in the same directory; unchanged products retain the
existing dual publication. Governance and final parser checks are recorded by
`closure-result.json`. No fresh guest execution is claimed by byte identity.

Fixed census remains316/355 disposed,39 pending. P183's18 extra NASM callers
have14 producer sites handled and four remaining. Fifteen supplemental caller
chains still need shared configuration/staging reconciliation; production of
their binaries alone does not close those chains or S3.
