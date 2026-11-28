# S15 N024 window-data migration

P77 runtime93 verified for delivery. Starting HEAD is
3a733e2af4e6500fc4ab9162dcef7f0a1f3365d1; accepted package runtime92.
Before migration getset.c is1nonerror point/1234lines against its OpenNT object.

## Fixed source and retained behavior

XP commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89, path
Source/XPSP1/NT/windows/core/ntuser/kernel/getset.c, blob
7f721d604212d819d13255fc59bd402ed98370e6. OpenNT object is
c88fb7853cefbf1c9830b914484262f6946faf11. Raw inputs and starting bytes are
retained in build/t485-s15/n024-getset.

Restore XP xxxSetWindowLongPtr (line260): pointer-width data, DWLP indices,
GWLP_MDIDATA, control-private write restrictions and unsigned extent checks.
XP kernel/userk.h already aliases this entry to xxxSetWindowLong on x86.
That exact alias is shared in the existing B window-state declaration boundary;
the provider table and actual x86 ABI remain unchanged. No new entry route.

Also restore original FindPCPD (line863), DBGGetWindowLong (line1012) and
DBGGetWindowLongPtr (line1019), despite no current product caller. The latter
two use the exact __GetWindowLong/Ptr macros from XP kernel/userk.h. Both the
product and original-body fixtures include this existing declaration boundary.
No new allocation, dispatch table or CPD registry is added.

The existing native-dialog message-result branch remains: the actual native
dialog owns DWLP_MSGRESULT and modal result propagation; writing only a copied
PDIALOG would not update it. Its original fallback is retained for the existing
original-body fixture. Negative production indices reach the existing
xxxSetWindowData binding only for GWL_STYLE; the original client retains its
ListBox initialization guard while modern USER owns real style notifications.
FCallerOk remains the exact-owned-window check, not imitation kernel privilege
state. Original x64, DBG and FE_IME excluded bodies retain their original
conditions and #error; #line recovery executes outside excluded branches.

## Whole-file omitted owners

| Original owner | Concrete unavailable boundary or selected alternative |
| --- | --- |
| DefSetText | kernel desktop-heap allocation and kernel window-name storage; native USER owns real window text |
| FCallerOk | ETHREAD identity, CSRSS/system-thread flags and Winlogon PID; selected private writes instead require an existing owned and retained original view |
| _SetWindowWord | separate kernel word-entry route, including ProbeAndReadUlong of button storage; no corresponding provider in this client composition; no current direct caller |
| xxxHandleOwnerSwitch | kernel input-queue attachment through zzzAttachThreadInput; native USER owns real window parenting |
| xxxSetWindowData | kernel window/thread locks, menu/parent/procedure mutation; selected GWL_STYLE continuation uses the existing native USER boundary |
| GetCPD | kernel HM allocation and locked per-class/window CPD publication; original client/clmsg.c already owns the selected GetCPD and native CPD transport |
| MapClientToServerPfn | kernel aStoCidPfn dispatch table, absent from the finite process-control server view |

These are explicit unconditional source omissions, not invented #error arms.
Restoring the kernel CPD owner would duplicate the already selected client
symbol. Existing original-layout projection helpers remain together after the
original functions: their window/WW/CLS offsets belong to the original layout
provider; no ownership or lifecycle is moved or duplicated in this batch.

## Validation

Complete the entire file before cached full build and dual13-image publication.
Run existing control-boundary cohort (dialog extra words, edit/private storage,
native modal owner and message callbacks), native creation/retirement scopes,
and real Win16 controls against the final package. Original guest is immutable.
P75 Full24/24 remains the checkpoint; this is its second subsequent file.
Original-line mapping and final byte-sensitive census will be recorded before
delivery. No S/T closure is claimed.

## Final verification and failed attempts

[Receipt](n024-getset-result.json) and [mirror census](n024-getset-census.json).
Cached full product build, dual13-image publication/26SHA256 checks, all40
control-boundary cases, native creation, process retirement and genuine Win16
controls pass. The latter uses the final runtime93 manifest identity.
Two initial test builds failed before executing any fixture: an obsolete
class-atom projection marker, then the fixture header's missing original MDI
layout selection. The projection now starts at the tested function; the header
selects the same original extent layout as production. No assertions changed.
Only failed/unexecuted targeted gates were retried; no repeat publication or Full.
Original logical-line audit checks184unchanged nonblank lines with no mismatch.
Original x64/DBG/FE_IME exclusions and immutable guest are preserved.

getset.c: OpenNT1point/1234lines -> XP20points/927lines;
delta+19points/-307lines. Restoring original LF separates
the previous whole-file CRLF span into actual residual spans, rather than adding
that many new algorithms. Normalized and byte-sensitive final nonerror metrics
agree; seven original excluded branch bodies are accounted for separately.
Full population: 1332files/792byte-identical/
803nonerror-identical/529different/
7980points/62985lines.
Commit delta: 0files/0exact/0same/0different/+19points/-307lines.
Baselines1317XP/15OpenNT. N0244/9, S15 11/14, overall59/159.
Two files since P75 Full24/24. Next profile.c; no S/T closure.
