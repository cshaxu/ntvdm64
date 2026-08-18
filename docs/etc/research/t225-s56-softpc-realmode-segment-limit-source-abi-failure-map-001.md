# T225 S56 SoftPC real-mode segment-limit compatibility source/ABI/failure map

Date: 2026-08-17
Status: pre-implementation admission map.

## Need

T225 S30/S55 establish a profile-neutral real-mode DS:FFFF two-byte read
which current Bochs rejects through its original direct segment-limit branch.
The current record has no per-instruction attribution, but it has an exact
mechanical access tuple. This map asks only whether the adopted SoftPC CPU
would reject the same class of access.

## Original SoftPC behavior

OpenNT's retained CCPU reference implementation is
src/opennt/base/mvdm/softpc.new/base/ccpu386/c_addr.c.

limit_check computes the access span (range = nr_items * op_sz - 1), identifies
an expand-up segment-end crossing, and may set bad_limit. Its default
#ifndef TAKE_REAL_MODE_LIMIT_FAULT branch then returns without raising GP or
SF whenever GET_PE() == 0 || GET_VM() == 1. This is an explicit historical
SoftPC compatibility rule, not an inferred DOS policy.

The CCPU SAS implementation at base/ccpu386/ccpusas4.c:763-770 reads a word
by separate adjacent byte reads at a page boundary. Its effective address is
linear; therefore the historical no-fault path does not need a BOP, adapter or
DOS service to cross the offset boundary.

## Current Bochs behavior

Adopted src/bx-core/cpu/access32.cc uses the original Bochs direct-limit
predicate for a virtual word read. At offset equal to a FFFFh segment limit,
the current code logs the segment violation and calls exception(#GP, 0).
T225 S30 proves this exact branch for selector 0C41, offset FFFF, width two,
in real mode.

The current CPU5/P-MMX build defines BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS=1.
Consequently the existing first-fault observer's prev_rip is trace-granular,
not an instruction attribution; it validates the mechanical tuple but cannot
name a guest instruction or service owner.

## Four-rung recovery ledger

| Rung | Disposition |
| --- | --- |
| Original source reuse | Not composable: OpenNT CCPU is the displaced SoftPC implementation. Its limit_check is the source contract, but it cannot enter the Bochs runtime. |
| Smallest adapter/shim | Rejected: an adapter has no segment-cache/access-path control and must not export or emulate CPU semantics. |
| Registered external-code intrusion | Candidate: a default-off, selector-blind bx-core compatibility gate at the existing segment-limit rejection path, selected only by an opaque minimal-machine compatibility configuration. |
| Newly authored behavior | Rejected. The behavior, mode rule and failure suppression are source-defined by the historical CCPU implementation. |

## Required classification before patch

S56 must classify the whole boundary, rather than only the observed word:

1. real mode and V86 versus protected mode;
2. read and write; byte, word and dword spans;
3. expand-up and expand-down segments;
4. aligned and unaligned end crossings;
5. current Bochs native result when the gate is disabled.

The first implementation, if admitted by that classification, must retain
current Bochs behavior by default and must contain no address, selector, BOP,
OpenNT, DOS, host, CLI or profile token. It may not alter protected-mode
checking, descriptors, guest memory mapping, exception delivery outside the
source-proven compatibility mode, or any adapter interface.

## Verification requirements

A focused CPU5/P-MMX MSVC x64 /MT fixture must prove both branches:
disabled native rejection; enabled source-shaped real-mode/V86 continuation
for the classified boundary; and protected-mode rejection. It must also prove
that no BOP/adapter/provider participates. Only after the complete fixture and
full formal graph pass may a single paired native T225 validation be run.

## Stop conditions

Pause before source modification if the source classification requires a
general emulator mode, a changed protected-mode rule, a guest-address special
case, a BOP/service branch, a new device, an adapter callback, or behavior not
covered by the historical CCPU rule.
## Classification result and admission decision

The full-source sweep shows that the CCPU rule is not a word-read exception.
c_oprnd.h invokes limit_check for byte, word, dword, larger operands and
multi-item/string forms; c_stack.c applies it to stack operations; and the
historical call, return, jump and interrupt paths have their own real/V86
limit policy. Current Bochs likewise distributes segment-limit checks across
access32.cc, access.cc, stack helpers and control-transfer code.

Therefore a patch only at read_virtual_word_32 would preserve neither the
historical rule nor a coherent Bochs invariant. A single exception-stage gate
would be worse: it could suppress unrelated #GP/#SS conditions and cannot
recover the access category. The required complete historical behavior is a
whole CPU compatibility profile, not a narrow T225 mechanical fix.

S56 stops before a CPU patch under its admitted stop condition. BX-CORE-086
remains a registered but unimplemented candidate. The owner must decide
whether to admit a separate, whole SoftPC CPU compatibility-profile package,
with complete real/V86 operand, stack and control-transfer semantics and
regression, or to retain native Bochs segment semantics and accept that this
candidate cannot close T225. No address-specific workaround is permitted.
## Implementation decision

Under the owner's standing authority for individually registered, source-proven
Bochs exceptions, this project adopts the complete compatibility-profile route.
The profile is default-off and is assembled by bx-mantle before CPU execution;
it is neither a CLI mutation mode nor an adapter/BOP/OpenNT policy. Direct,
Readonly and future Overlay use the same CPU setting for a declared minimal
guest-machine profile.

The implementation is partitioned by native CPU ownership:

| Group | Bochs owner | Required profile result |
| --- | --- | --- |
| Scalar virtual data access | access32.cc and access.cc | In real/V86 only, suppress only the historical segment-span limit rejection while retaining descriptor-present/type, paging, alignment and physical-memory checks. |
| String and RMW access | access32.cc callers, string.cc and io.cc | Use the same scalar access rule; no separate string/DOS behavior. |
| Stack access | stack16.cc and access32 stack helpers | Use the same real/V86 access rule for ordinary stack words/dwords; retain stack pointer width and all protected-mode checks. |
| Instruction fetch | cpu.cc, icache.cc and access checks | Reconcile only the source-defined real/V86 span behavior with already retained 16-bit IP wrapping; no command-address or opcode branch. |
| Far control and interrupt return | call/ret/jmp/iret/exception transfer checks | Audit and apply the same mode rule only where Bochs independently raises an equivalent segment-limit failure; no relaxed privilege, descriptor, task-switch or protected-mode rule. |

The profile must expose one mantle-private lifecycle setter/query and a
selector-blind core predicate. It will be inactive by default, reset to
inactive during machine teardown, and be enabled only by the declared
minimal-machine composition before the guest starts. It names no guest,
service, BOP, host path or mutation mode.

This is a whole CPU compatibility component within T225, not an alternate
emulator and not a reimplementation of SoftPC. S56 P2 may now implement the
core/mantle gate and the scalar-access family first; subsequent P parts must
complete stack/fetch/control-transfer groups before any claim that the profile
is complete.