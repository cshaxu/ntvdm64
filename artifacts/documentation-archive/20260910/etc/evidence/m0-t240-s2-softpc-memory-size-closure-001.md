# M0 T240 S2 SoftPC `memory_size` Closure Evidence

## Question

Can the reached SoftPC `C4 C4 12` query use the original `mem_size.c`
contract without a Bochs semantic change or a frozen adapter value?

## Inputs and procedure

- Original owner: `src/opennt/base/mvdm/softpc.new/base/bios/mem_size.c`.
- Existing selector-blind machine seam:
  `bx_ntvdm_mantle_checked_ram_read_v1(address, bytes, count)`.
- Active route: `src/bx-vdm/bop/opennt/softpc/mem_size.c`, its named shim,
  `softpc_memory_size_v2_generic_ud_bridge.c`, and the common bx-vdm ingress.
- Focused MSVC x64 `/MT` compile/run of
  `t240_s2_softpc_memory_size_fixture.c`.
- Fresh formal Ninja r3 fixture link/run; it compiled the full `bx-vdm` static
  library and printed `T240 S2 source-shaped SoftPC memory_size reads mutable
  BDA RAM`.
- Fresh formal Ninja r4 linked `ntdos64-native.exe` in 255 graph edges. One
  watchdog-bounded Direct invocation used its current sibling `dos/` and
  `wow16/` bundle contract plus the primary source-built `TARGET.EXE` input.

## Observations

1. The direct mirror preserves the original local `word memory_size`,
   `sas_loadw(MEMORY_VAR, ...)`, then `setAX(memory_size)` order.
2. Its only divergence is a private, prefixing compatibility shim: historical
   SAS/CPU global spellings would otherwise collide with independently
   imported OpenNT modules. It converts precisely one BDA `0x413`, two-byte
   checked read and one low-16-bit AX write; it exports no BOP, BIOS, DOS or
   OpenNT term to `bx-mantle` or `bx-core`.
3. The focused and formal fixtures both prove BDA bytes `7f 02 -> AX=027f`, a
   subsequent guest-visible BDA change `34 12 -> AX=1234`, exact `RIP+3`,
   AX-only mutation, and decline for unreadable RAM, wrong selector and
   protected mode.
4. The superseded hard-coded `bx_ntvdm_bios_memory_service` and its coupled
   `15h/AH=88h` fixture are removed from source and the formal manifest.
5. The single actual native execution exited normally in about 150 ms with
   `bop-sequence count=0` and terminal detail `3`. It therefore did not reach
   `C4 C4 12`; it is an integration limitation, not evidence against the
   locally verified provider. Two earlier command-line/bundle-admission
   attempts exited before guest execution and are not counted as guest runs.

## Interpretation and follow-up

The BOP 12 component is code-complete: original source shape, live BDA
semantics, active bx-vdm ingress, focused and formal local verification all
hold without a new Bochs intrusion. The current native product bundle stops
before the completed selector, so T240 makes no claim of whole-guest
continuity or live BOP-12 reachability. The next candidate is the separately
approved-by-queue keyboard/IVT/INT15 machine package; it must begin from its
own tracker rows and cannot treat this trace as authority for a new leaf
repair.
