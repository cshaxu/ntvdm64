# M0 T349 S1 — Original COMMAND package-path capacity audit

Date: 2026-09-01

## Question

What capacity contract governs the app-selected executable-relative `mvdm`
root, and can it be resolved without rewriting original COMMAND configuration
processing?

## Inputs

- selected OpenNT and OpenNT-4.5 `cmdconf.c` and `nt_pif.c`;
- current mirrors `src/mvdm-host/dos/command/cmdconf.c` and
  `src/mvdm-host/softpc.new/host/src/nt_pif.c`;
- current app package layout, session root storage and the source-shaped
  `mvdm_softpc_system_*` media adapter.

## Findings

- Original `ExpandConfigFiles` has a genuine 64-byte `achSysRoot` contract:
  `GetShortPathNameA` must produce 1 through 63 bytes.  Its zero or oversized
  output is already routed to `ED_BADSYSFILE` followed by `TerminateVDM`.
- The selected product changed only root acquisition.  It preserves the
  original PIF override, short-path normalization, configuration expansion,
  generated COUNTRY/SHELL lines and later failure route.
- Three additional callers require the same selected root or one selected
  media path to fit their original caller-owned buffers: `GetPIFConfigFiles`,
  `DemInit`, and SoftPC NTIO lookup.  They cannot be left to fail at different
  startup times merely because session can store 1,024 bytes.
- No mapping manager is applicable: these are synchronous host path bytes,
  not an opaque host identity, handle or guest pointer.

## Decision for S2

The only source-shaped product intervention is one app-owned admission check
after it derives the `mvdm` root and before original host startup.  It will
use the public path APIs to require the original short-root capacity plus the
existing default `config.nt`, `autoexec.nt`, `ntio.sys` and directory caller
capacity.  It deliberately will not preflight file existence: a PIF can
override the two configuration names, and the original callers own those
errors.  It will not synthesize an 8.3 path, widen a mirror buffer, use the
host Windows directory, copy guest media, or alter original COMMAND text.

The exact rows and failure routes are retained in
`m0-t349-s1-mvdm-package-path-capacity-ledger.tsv`.
