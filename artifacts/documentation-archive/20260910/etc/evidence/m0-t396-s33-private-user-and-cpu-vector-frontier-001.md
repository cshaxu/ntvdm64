# M0 T396 S33 — private USER and CPU-vector frontier

## USER registration audit

The NT4-private `PFNWOWHANDLERSOUT` table has 21 callback fields.  The
original provider consumes 20 of them across window creation, task setup,
yield/wait, cleanup, menus, dialogs, DDE, resources and cursors.  Its first
runtime consumer is `pfnRegisterUserHungAppHandlers` in
`WK32InitializeHungAppSupport`.

Current USER32 still exports `UserRegisterWowHandlers`, but fixed-stage
evidence proved that it does not populate that NT4 table: after its return,
the original indirect call executed address zero.  With `WOW32.DLL` loaded at
`0x6a300000`, return `0x6a320c34` maps to provider RVA `0x20c34`, inside
`WK32InitializeHungAppSupport`.

The only admitted field recovery is the inert registration direction for the
absent private USER hung-app broker. It validates the original callback/event
arguments and returns success, but creates no broker, task policy, thread, or
callback invocation. It does not claim the remaining 20 private USER
capabilities.

## CPU40 vector recovery

After that field no longer masked execution, the next default-off exception
was an execute-through-null return in C-VID `setSTATUS`. The generated
`cvidc_cpu_binding.inc` explicitly assigned `Cpu.SetSTATUS = 0`, although the
selected CPU40 CCPU provider exports the individual flag setters.

The recovery assigns only this slot to `mvdm_cvidc_set_status`, which applies
the source `nt_cpu.c` order: NT, IOPL, OF, DF, IF, TF, SF, ZF, AF, PF and CF.
Each component is then delivered to the original CCPU `c_set*` provider.
No status bit, CPU instruction, BOP, guest memory or USER callback behavior
is invented.

## Rebuild and observation

The formal x86 parent rebuilt all 449 objects and linked normally. Its SHA-256
is `2600AA1CC70A2F039975310C13447D576116824BE88DB1CC443893F6E3418399`.
The current complete WOW32 provider SHA-256 is
`E8326EBAC340B69D391D6912E61CAC6B66C17429A1F4F0D4ED5EB7C79B8B0D4B`.

The fixed non-debug Console observation of `WOWEXEC.EXE` ran for the full
15-second bound (23 modules; `WOW32.DLL` loaded) without an exception. Its
main emulation thread was live at preferred RVA `0x433e89`, original CCPU
`_contributory_exception`, with the normal CCPU/host return chain. This is a
live frontier only, not Win16 lifecycle acceptance.
