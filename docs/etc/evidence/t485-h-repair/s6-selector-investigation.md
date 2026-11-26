# S6 selector 0040 repair investigation

Status: final production verification complete. This receipt is delivered by the S6 P1 commit; S7 is admitted after the S6 closure checkpoint.

## Ownership and reproduced failure

The removed c_seg.c shortcut forced every protected-mode selector 0040 to BIOS
base 0400 and limit FFFF, even when a guest owns an unrelated GDT descriptor.
The audit counterexample and case 186 exercise the original CCPU descriptor load.

DOSX's early FSTI needs the NT-owned fixed BIOS GDT slot. Its reserved source slot
can contain C5 poison with the Present bit set; testing Present is insufficient.
The host now constructs this slot in its existing private DOSX shadow GDT without
writing the immutable guest image or its source descriptor table.

A second real WINMINE failure exposed the later publication boundary. Diagnostic
worker traces show SetShadowDescriptorEntries at CS 01A7:667A publishing LDT index
0040 with base 0400, limit 0, access F3. The existing broad projection also wrote
that entry into the GDT fixed slot. A later CCPU load at 00C7:056C saw base 0400,
limit 0 in both the DOSX shadow and the WOW GDT. Hence initial publication alone
cannot remove the old CPU shortcut safely.

The candidate preserves GDT 0040's independent ownership during LDT updates.
Other existing descriptor publication behavior is unchanged. Case 186 also
updates the actual LDT slot through SetShadowDescriptorEntries and checks that
the LDT receives limit 0 while the fixed GDT retains FFFF, then switches to an
independent guest GDT and verifies its original base and limit.

## Evidence and limits

- P4 parent: e6e10bd896be21d1fe28b3e6651170e0496ad09f.
- Frozen point: HXP-R-00558; c_seg.c selected code restored to original.
- Companion provider: dpmiselr.c, whose remaining whole-file audit belongs to S7.
- Source contract: src/mvdm-guest/dpmi/dxstrt.asm early InitLDT/FSTI and fixed
  non-WOW_x86 GDT 0040 initialization; no guest edits or recompilation.
- Local evidence: build/t485-s6/selector-trace.txt and setter-trace.txt;
  diagnostic packages are isolated and not production publication.
- Initial candidates failed actual WINMINE despite native tests passing. These
  are not accepted packages. Final-candidate build and real-program gates remain
  required before closure; measurements will be recorded from final code.

The repair does not claim all descriptor projections are minimal. S7 retains
its complete dpmiselr.c responsibility and must preserve this proven distinction.

## Current final-candidate evidence

Cached full build and all 60 CCPU/DPMI cases passed on the final source
(build/t485-s6/build3.log, unit-final3.log). The real WINMINE window now passes
on runtime3 after dual hash-verified publication. Win3.1 standard/enhanced, Win95 strict window/fullscreen round trips and DOS4GW/NESticle all passed on this same package. Win95 retains the previously approved exit-code-1 boundary, not a normal-zero-exit claim.

[Non-error measurements](s6-mirror-diff.json): c_seg.c 1 point / 8 lines to
0 / 0; dpmiselr.c 12 / 54 to 12 / 61; total 13 / 62 to 12 / 61. Both point and
line totals decrease. Existing exclusion bodies and #line anchors are retained.
[Published package identity](s6-package.json) identifies all thirteen images.
## Closure and residual registration

The S6-owned c_seg.c file has no remaining non-error mirror differences; its
one assigned point HXP-R-00558 is restored. No new CPU condition, callback,
queue or state machine was introduced. Both descriptor responsibilities remain
at the existing DOSX provider. HXP-R-00175 and HXP-R-00184 receive the necessary
coupled provider changes; their entire file remains in the nineteen-file S7
review rather than being falsely counted as completed here. The residual
fixed-descriptor initialization and independent GDT/LDT publication are retained
with the concrete failing controls above, pending final S7 minimality review.

[Validation summary](s6-validation.json) records final tests and identities.
Full is not rerun for this one-point repair: P4 Full23/23 immediately preceded
it; this packet runs the specifically affected CPU, WOW, DOSX and guest startup/
switching gates. Migration stays 39/159. S6 restores 1 of 4,550 frozen audit
points in 1 of 1,039 assigned files; these are repair coverage, not migration
batch counts, and do not count the companion provider file as complete.

S7 begins with dpmi32/dpmi32.c in relative-path order. Preserve this correction
and P4's DPMI/Window changes. No original guest binary or winnt.zip was changed.