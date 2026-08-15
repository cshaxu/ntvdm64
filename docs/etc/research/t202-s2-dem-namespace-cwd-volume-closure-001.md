# T202 S2 closure: DEM namespace/CWD/volume

## Question

Has the direct-host namespace, current-directory/default-drive, and immutable
drive/volume observation subpackage reached its stated boundary without
claiming DEM file/handle/FCB/search recovery or an implicit virtual C:?

## Inputs

- OpenNT `base/mvdm/dos/dem/demdir.c` and `demgset.c`, plus
  `demerror.c` for the `demClientError` result rule.
- T200 S30 selected-host root capability and the T201 mutation-profile ABI.
- The T202 S2 source/ABI map and its CWD, boot-drive, and GSET evidence.

## Procedure

1. Reconciled `50:13`, `50:18`, and `50:1A` as one CWD/default-drive family
   through session-owned state and checked guest reads/writes.
2. Reconciled `50:0D`, `50:0E`, `50:0F`, `50:10`, `50:25`, and `50:46`
   against one admitted drive/volume snapshot; repaired the excluded-drive
   failure group to `AX=ERROR_INVALID_DRIVE (15), CF=1`.
3. Deleted the displaced root-only CWD/default-drive fallbacks and their
   composition inputs.
4. Ran the retained source-built x64 `/MT` fixtures and documentation gate.

## Observations

- `Invoke-T202S2HostNamespaceProbe.ps1` proves selected real-root validation
  without exposing host paths or handles.
- `Invoke-T202S2DemCwdServiceProbe.ps1` at
  `artifacts/build/t202-s2-dem-cwd-service-r5` passes direct, readonly,
  overlay, and explicit virtual CWD outcomes. It includes OpenNT's stale-CWD
  query-time reset to root.
- The complete composition closure at
  `artifacts/build/t202-s2-cwd-fallback-removal-r1` source-built and its
  generated `t198-s16-bx-vdm-boot-namespace.exe` returned zero after removal
  of the legacy fallbacks.
- The complete DEM package fixture at
  `artifacts/build/t202-s2-volume-failure-r1` source-built and its generated
  executable returned zero; excluded media/DPB destination bytes remained
  untouched.
- `Verify-DocumentationGovernance.ps1` and `git diff --check` passed for the
  accepted deliveries.

## Interpretation and confidence

S2 is closed for its admitted owner boundary. Direct mode uses selected,
actual host roots and immutable drive/volume observations; readonly, overlay,
and virtual are handled only where their shared-profile CWD disposition is
admitted. There is no ambient process CWD mutation, raw handle crossing,
implicit synthetic C:, or remaining root-only CWD fallback.

The explicit virtual boot-volume capability is deliberately not a result of
S2. It remains a default-off, separately admitted whole-package feature; no
virtual volume, DPB, free-space value, or boot-file materialization is claimed.

## Follow-up

T202 S3 admits the next complete DEM owner subpackage: file/handle/FCB/search
and its direct/readonly/overlay dispositions. It must first map
`demfile.c`, `demhndl.c`, `demfcb.c`, `demsrch.c`, and related dispatcher
entries as one source/ABI/failure set. Trace is integration evidence only.
