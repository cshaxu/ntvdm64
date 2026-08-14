# M0 T199 S22 C1 A20 Capability Regression

The selector-blind mantle wrapper was rebuilt with MSVC x64 `/MT` against the
existing complete CPU5/Pentium-MMX source-built closure at
`artifacts/build/t198-s13-msvc-x64-lifecycle-r3`. The fixture recompiles only
the changed minimal-machine owner and A20 wrapper, then relinks all 130
existing x64 closure objects and runs successfully at
`artifacts/build/t199-s22-a20-relink-r3/t199-s22-a20.exe`.

It proves: pre-initialize rejection; initialized query returns enabled;
native set `1 -> 0 -> 1`; invalid value rejects while state remains disabled;
and post-cleanup rejection. The native log confirms CPU hardware reset and
APIC/CPUID initialization. No BOP, adapter, XMS/DPMI provider, bx-core source,
or guest trace entered this regression.
