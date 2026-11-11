# T482: Win95 restart into MS-DOS

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
