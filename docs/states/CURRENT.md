# Project Status

**No active M/T/S packet.**

## Current Work

The owner accepts T445 S11 and authorizes final T445 closure audit, commit
and push. T445 is closed with its recorded bounded results; no next S or T
is admitted. See the [closure](../history/m0-t445-window-style-closure.md),
[queue](QUEUE.md) and [retained debt](TODO.md). Queue order is unchanged.

## Current Technical Baseline

The coherent13-image T445 package is published to assets/release and
O:/winnt/system32. release-manifest.json records exact identities; S11 r024
verifies tested/copy/deployed hashes and the deployed style boundary.
S9 P1 is266067264 and P2 is4f8c181487; S10 P1 is e046ee3d7;
owner-accepted S11 P1 is b39ceb7e6, acceptance record cbd0f7832.
Final closure changes documentation only, without rebuild or redeployment.
Guest media and ordinary runtime configuration remain unchanged.

| Architecture | Images |
| --- | --- |
| AMD64 | run16.exe, ntsrv.exe, ntcon.exe, ntvwm.exe, ntmon.exe, nthook64.dll, ntw16s64.dll, W16Style.exe |
| I386 | ntvdm.exe, WOW32.DLL, VDMREDIR.DLL, nthook32.dll, ntw16s32.dll |

--win16-style and --win16-style-all retain their propagated policy. WOW32
owns Win16 attachment; native Hooks use their matching-width NTW16S DLL.
S11 changes only NTW16S32/64 and W16Style among the13 images: generic dialog
background/link typography, flat separators and multiline EDIT attachment.
The other ten binaries retain the S10 identities. W16Style adds page28 for
dialog/link coverage; native branding and application-owned painting remain.
Title/menu/standard-control System bitmap type is20px. The demo layout
matches20px proportions. Generated caption prefixes are retired; application
titles remain unchanged. NTCON links RPC core without launch-style state.
See [S11 evidence](../etc/evidence/m0-t445-s11-dialog-coverage.md) for actual
verification and limits. SOL/WRITE retain their known initialization failures,
assigned to the existing basic-WOW queue rather than claimed repaired here.

## Recent M0 Closures

| Task | Outcome |
| --- | --- |
| T445 | Modular Win3.x appearance, propagation and integration,20px demo/caption cleanup, default-button and dialog/control coverage. [Closure](../history/m0-t445-window-style-closure.md) |
| T444 | Owner-accepted indexed palette repair, audit-only capability disposition and measured host VT/API fidelity. [Closure](../history/m0-t444-indexed16-text-closure.md) |
| T441 | Repaired mixed NTVDM/NTVWM release that stalled COMMAND/ver; bounded variable I/O retained. [Closure](../history/m0-t441-variable-worker-io-release-integrity-closure.md) |
| T440 | Verified graphics/transport/idle/build improvements retained; active root cause deferred. [Closure](../history/m0-t440-text-response-performance-closure.md) |
| T438 | Reversible Win3.1 path repair and CMD/PIF/GRP ownership. [Closure](../history/m0-t438-win31-path-recovery-closure.md) |
| T436 | Limited Win3.1 driver/setup delivery; ordinary Setup transition deferred. [Closure](../history/m0-t436-win31-setup-limited-closure.md) |
| T437 | Installed-tree portable launch and path repair. [Closure](../history/m0-t437-installed-win31-portable-launch-closure.md) |

## Recent Governance

Final owner-directed closure includes S10/S11 after the earlier S9 closure.
Earlier stages closed without manual testing remain explicitly distinguished;
S11 acceptance does not retroactively change their evidence. Native-menu
animation and bounded glyph/layout limitations stay in TODO, not represented
as fixed. The whole T retains registered S7 DIV-331; S10/S11 and final
documentation closure add no mirror/guest diff.
[Archived status](../history/m0-t445-pre-closure-status.md) retains the former
detailed chronology; no historical packet there is active.
