# M0 T287 S7 — XMS owner-package closure audit

## Closure result

T287 is closed as an original-source, code-complete and local-regression
closure. It does not claim external BOP ingress, a continuous guest run, a
successful physical UMB map, or full keyboard-controller behavior.

## Source and exception audit

- The selected `mvdm-host/xms.486` root has 15 original paths. Eleven remain
  byte-identical to both selected OpenNT roots: `xms.c`, `xmsdisp.c`,
  `xmsmisc.c`, both direct-address/RISC memory backends, five original build
  descriptions, and the remaining exact headers/forms.
- Four files are minimal same-shaped mirror modifications, not identity
  matches: `xms.h`, `xmsa20.c`, and `xmsumb.c` are registered as
  `MVDM-HOST-DIV-010`; `xmsblock.c` is registered as `MVDM-HOST-DIV-011`.
  The matching material implementations live only in
  `mvdm-host-overlay/xms.486`, and each mirror callsite carries its
  `DIVERGENCE` marker.
- The corrected per-file identity/hash ledger is
  [S1 file ledger revision 002](../operations/m0-t287-s1-xms-file-rebaseline-ledger-002.tsv).
  The earlier all-exact wording is superseded by this closure record.

## Interface and state ownership

- Original `XMSInit` and the twelve-entry `XMSDispatch` table are compiled and
  run in the S6 x86/x64 matrix. Original source continues to own service
  ordering and register results.
- `adapter-mvdm-host-out/softpc` is the sole same-shaped provider for the
  reached register, A20, and numeric extended-memory contracts. It only calls
  typed `adapter-bochs` mechanics and exposes no Bochs type.
- `session` contains the only mapping-manager implementation. XMS takes fresh
  guest-memory leases through its existing session instance; neither the
  mirror, overlay, nor adapter creates a second mapper or retains a host
  pointer.
- The direct address bodies `i386/xmsmem86.c` and `xmsmemr.c` remain exact
  source evidence and are not product runtime inputs. Their reached contract
  is supplied through the registered same-shaped XMS memory facade.

## Transferred prerequisites

- Successful physical UMB range ownership remains with the next queued
  **Original VDM monitor and keyboard/IVT substrate** package. Until it
  recovers the original monitor/SAS range contract, XMS preserves its original
  empty-map `B1`/`B2` outcomes.
- The production `UpdateKbdInt15` algorithm remains the original
  `mvdm-softpc-firmware/softpc.new/base/keymouse/keybd_io.c` owner. The same
  next package explicitly includes its required keyboard/IVT seam; the S5/S6
  carrier was test-only.

## Verified boundary

The S6 formal MSVC `/MT` Ninja matrices passed on x86 and x64. Their manifests
reject `src.old`, prebuilt product archives, and the direct pointer XMS
backend. The closure therefore proves the complete original XMS owner package
at its locally admitted boundary, with each remaining external prerequisite
assigned to the next named owner package.
