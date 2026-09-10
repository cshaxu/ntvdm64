# M0 T310 S8 P4: SoftPC Sound / NT I/O ABI Closure

## Scope

This evidence closes the x86/x64 call-contract findings in the selected
original `mvdm-host/softpc.new/host/src/nt_sound.c` source. It does not alter
non-width source warnings or introduce new sound policy.

## Source findings

- `host_ica_lock` and `host_ica_unlock` are original `void` providers in
  `softpc.new/host/src/nt_eoi.c`; `nt_eoi.h` contains their declarations but
  also imports monitor-only `VDMVIRTUALICA` state that this sound translation
  unit does not select.
- `OpenBeepDevice` calls the historical `NtCreateFile` form with the selected
  32-bit `OPENNT_IO_STATUS_BLOCK`. The existing facade covered adjacent NT
  file APIs but not this entrypoint, so modern `winternl.h` selected its
  incompatible pointer-width status carrier.
- The K&R `host_alarm`/`host_ring_bell` declarations pass only historical
  fixed-width `long` duration values. Their warnings occur on both targets;
  they are retained as non-x86/x64 source-style diagnostics.

## Repairs

- `nt_sound.c` declares only the two original `void` ICA contracts, retaining
  call order and avoiding the unrelated monitor carrier.
- The adapter's `ntioapi` facade now exposes same-shaped `NtCreateFile`, uses
  native `IO_STATUS_BLOCK` solely at the public NTDLL call, and copies its
  documented status into the original-width host-local carrier.

## Verification

Formal Ninja targets compiled outside the sandbox:

- `build/M0-T310/S8/p1-machine-source/x64`: `obj/host/nt_sound.obj`, exit 0.
- `build/M0-T310/S8/p1-machine-source/x86`: `obj/host/nt_sound.obj`, exit 0.

Neither log reports C4013 for the ICA calls nor C4133 for the `NtCreateFile`
status argument. Remaining visible warnings are the original K&R and unused
parameter diagnostics plus existing profile macro redefinitions; they are
outside the P4 x86/x64 repair class.
