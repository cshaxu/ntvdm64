# T482: Win95 restart into MS-DOS

Owner disposition superseding earlier pending-scope notes: this investigation
is closed and all unfinished capability/transition work is assigned to the
[Windows XP baseline-upgrade package](../../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md).
See [closure](../../history/m0-t482-closure.md). Historical observations below
remain unchanged; the feature is not claimed repaired.

Baseline 782854d6f, owner-approved next queue package. Original guest immutable.

1. Reuse the existing private-desktop probe and installed Win95 copy builder.
   Identify how the actual shell shutdown choice encodes its request; compare
   normal Windows exit. Record DOSONLY and driver settings and the actual
   process/guest transition, without conflating DOS return, reset and host exit.
2. Trace the reached original guest and host/addon chain to its first incorrect
   state transition. Establish failing and working controls. No program-name
   workaround, speculative monitor or extra lifecycle state machine.
3. Restore the missing original owner/binding with the minimum project change.
   Confirm DOS prompt input, commands and appropriate re-entry/exit; expand
   configurations only where they affect that transition.
4. Run focused controls and applicable Full, cached x86/x64 build, complete
   coherent publication and hashes, code-quality review, commit and push.

S1 owns the bounded attribution and minimal repair. Split a further S only
when a real dependency warrants it; do not add speculative coverage projects.
An immutable guest or unsupported original transition requires explicit evidence
and an owner-reviewed disposition; it cannot become a fabricated pass.


## Bounded decision brief after P1 (not implementation admission)

Keep two independent requirements. (A) The real Explorer menu must be able to
create/open its original localized PIF through a filesystem provider that truly
supports that name. (B) Executing that original real-mode request must unload
Windows and yield an interactive DOS environment, then support the intended
Windows return. Passing either requirement does not waive the other.

For A, the candidate architectural direction is a documented Win95 IFS/provider
boundary with host-backed file operations. No provider ABI or module reuse has
yet been selected or proved. A design review must establish which original
provider presently owns the drive and why it reports no LFN, before deciding
whether an existing provider can be connected or an independent adapter is
needed. Do not start by adding a second filesystem, new RPC transport or a
filename lookup table. Reuse existing host file facilities only after the guest
contract, naming/encoding, directory results, handles and error ownership match.
Do not attach file service ownership to either mouse/NIDDB or multitask driver
merely because those drivers already have a guest-to-host call boundary.

For B, continue the already-authorized bounded exit attribution independently
of A: the short-PIF diagnostic is a test input, never the delivered menu path.
Compare the existing configuration with NTVDM64 disabled and with both optional
companions disabled. Source inspection shows NTVDM64 closes its notification
IRQ while INSIGNIA requests original NIDDB snapshot reset. Configuration and
runtime outcomes must decide whether either is causal; source proximity alone
is insufficient. A single protected-mode PC snapshot is not a deadlock proof.

State ownership remains: guest Shell/PIF/Windows owns the requested real-mode
transition; NTVDM executes CPU/devices and reports actual task transitions;
NTSRV owns host requests and process lifecycle. A successful guest mode switch
must not be fabricated by terminating the worker, completing Run16 early or
normalizing the exit code.

Options for owner review:
- Authorize a finite provider-design stage, then approve its concrete minimum
  implementation once original provider reuse and dependencies are known.
  Keep the DOS return investigation distinct and require both before acceptance.
- Retain and transfer the reproduced capability/transition gaps with complete
  evidence. This changes the closure outcome to investigation, not fixed feature,
  and requires owner agreement. The queued XP/LFN work is a related candidate,
  not evidence that it supplies a Win95 protected-mode provider automatically.

No original Shell resource replacement, filename special case, guessed 8.3
alias, advertised-but-unimplemented LFN flag, original guest patch, or synthetic
successful shutdown is an acceptable implementation of the requested goal.
