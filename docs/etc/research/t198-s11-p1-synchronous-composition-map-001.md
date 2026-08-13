# T198 S11 P1 Synchronous Composition Map 001

## Question

Do the adopted CPU5 generic-UD records and the S8 mechanical action already
close a synchronous `bx-vdm` boot-namespace transaction, without carrying BOP
meaning into `bx-core` or `bx-mantle`?

## Inputs

The audit reads the enabled `#UD` gate in `src/bx-core/cpu/exception.cc`, the
generic event/outcome ABI and default mantle bridge, the i686 alternative
bridge, the mantle-owned ordinary-RAM action executor, and the closed S10
plane. It uses the S7 i686-to-MSVC/x86 link witness as the existing object
direction baseline. No source or build configuration is changed.

## Mapped Transaction

1. CPU5 copies `#UD` vector, fault boundary, register/selector values and at
   most 15 instruction bytes into `bx_ntvdm_generic_ud_event_v1`.
2. The composed i686 bridge reconstructs adapter boundary/state/window,
   applies common ingress and provider selection, then calls the one S10
   plane. Selector and provider terms stop here, in `bx-vdm`.
3. Direct plane results translate losslessly to the already equivalent generic
   outcome: disposition, resume RIP, low-16 GPR mask/values and CF mask/value.
4. For a write action, the same adapter composition calls the mantle-owned
   `bx_ntvdm_mantle_execute_mechanical_action_v1`; the mantle validates and
   copies ordinary RAM but sees no selector or provider data. The adapter then
   translates the associated typed result to the generic outcome.
5. For a read action, the mantle fills the copied action payload; the adapter
   completes the plane, executes any resulting write action through the same
   mantle call, translates the completed typed result, and returns synchronously.
6. The CPU gate alone applies the opaque outcome and `longjmp`s to the native
   resume/controlled-stop path. It remains selector-blind.

## Observations

`bx_ntvdm_cpu_result_v2` and `bx_ntvdm_generic_ud_outcome_v1` have the same
accepted result vocabulary: resume/stop, RIP, eight low-16 GPR fields and CF.
The generic bridge is synchronous and does not retain either argument; the
mantle action executor likewise receives a fixed action pointer only for its
call. Therefore no new core or mantle ABI is needed for the inner transaction.

Two composition gaps remain outside those record layouts:

- The current alternative bridge is deliberately classification-only and has
  no initialized adapter-owned S10 plane/profile state. A composed target
  needs an explicit adapter-owned, bounded session initializer/lifetime before
  it supplies the alternative bridge symbol.
- S10 exposes write actions but does not currently return their associated
  transaction result from `dispatch`: COMMAND boot preparation creates
  `transaction->result` and normal read supplies `bulk.result`, while the
  plane leaves its outward `result` at pass-through for those action paths.
  The future composed bridge would otherwise write guest RAM and re-enter the
  same fault. This is an adapter-plane result-propagation fix, not a Bochs
  change and not a reason to add a new mechanical record.

## Interpretation And Confidence

The underlying ABI is sufficient. The next implementation must be an
adapter-owned synchronous composition session that (a) initializes the
immutable profile/plane explicitly, (b) drives read/write actions through the
existing mantle function, and (c) propagates the existing typed result after
each accepted action. It may replace the alternative bridge object in the
known i686/MSVC x86 closure, but must not modify core/mantle or add Bochs
service vocabulary. Confidence is high: every field and call has a current
source owner, and the only discovered omissions are adapter-local state and
result propagation.

## Follow-up

Admit an implementation S for the adapter composition session and the two
result-propagation assignments, with a focused i686 transaction test before
attempting an MSVC/x86 `UD2` native fixture.
