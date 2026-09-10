# NT4-Style DOS Bootstrap Contract

## Status and Scope

This is a clean-room engineering contract inferred from the checked-in
historical source snapshots. It describes roles and ordering, not copied
implementation, binary layout, or a promise that an arbitrary user-supplied
runtime will boot. The owned `nvtdm` engine is the implementation target.

The target profile is a user-selected, internally consistent NT4-era EN-US
runtime. Runtime files stay outside this tree. This project does not install,
patch, extract, or redistribute them.

## Evidence Boundary

OpenNT's DOS bootstrap source establishes these high-level facts:

- The I/O/bootstrap component starts with largely unspecified loader register
  state, then establishes a known local stack and real-mode data segments.
- It preserves and replaces selected interrupt vectors, initializes BIOS/DOS
  state, and performs device initialization before transferring to the DOS
  kernel.
- The DOS kernel returns control to the bootstrap path after its own setup;
  only then is the command processor loaded and entered.
- The historical system contains a guest-to-host service mechanism for DOS,
  XMS, DPMI, command, redirector, input, video, notification, idle and stop
  responsibilities. Those service families are proof of a host contract, not
  a license to depend on historical host integration.

The historical implementation's byte sequence and selector assignments are
runtime-profile details. They are not part of the `nvtdm` public command line,
and no generic instruction-override API is derived from them.

## Owned Engine Phases

1. **Discover and validate**: accept one user-selected runtime directory;
   require regular, non-empty files for the declared base profile; record
   metadata without modification.
2. **Construct machine**: allocate an owned guest address space, create a
   real-mode CPU state, and register only explicitly selected memory and device
   mappings. The engine owns all host resources.
3. **Prepare firmware environment**: install an engine-owned IVT/BDA/ROM
   policy appropriate to the selected profile. This is a profile decision, not
   a host-PC passthrough.
4. **Load bootstrap and DOS kernel**: parse and validate their selected binary
   formats before any guest state is committed. Loader and image-placement
   rules must be measured from a supplied, version-locked runtime, not guessed
   from filenames.
5. **Run bootstrap initialization**: execute CPU instructions through the
   owned CPU semantics. Device and service requests use explicit engine
   contracts.
6. **Load command processor or target**: create an owned DOS process context,
   command tail, PSP/environment, and handle table. This occurs only after the
   bootstrap/kernel phase reports success.
7. **Host handoff**: PE32 and PE32+ are launched by the separate dispatcher
   with ordinary `CreateProcessW`; they never enter the DOS guest. NE/Win16 is
   a separate optional tier and is not implied by this contract.

## Service Boundary

The runtime adapter may recognize a finite list of profile-specific service
requests. A request can only produce one of: resume with a validated register
and guest-memory result, inject a normal guest interrupt, wait, terminate, or
report a guest fault. It cannot directly change CPU execution mode, replace
memory routing, call arbitrary host pointers, or mutate host process state.

All file, directory, stream, media, input, clock, screen, and diagnostic work
is supplied through engine-owned capability objects. The initial implementation
uses documented user-mode Windows APIs. Undocumented APIs may be catalogued as
historical evidence, but may not become a required execution dependency
without an explicit isolation decision.

The source-evidence-to-owned-capability mapping is maintained in
`docs/OPENNT-SERVICE-BOUNDARY-MATRIX.md`.

## BYOB Runtime Profile

The current inventory profile names `NTIO.SYS`, `NTDOS.SYS`, `COMMAND.COM`,
`HIMEM.SYS`, and `DOSX.EXE` as mandatory role candidates; `VDMREDIR.DLL` and
`EDIT.COM` are optional. Presence and size are only admissibility checks.
Before boot support is claimed, a profile must additionally lock each input by
metadata such as version and user-recorded SHA-256, and demonstrate the
bootstrap entry and required service set using a trace that records state and
events but never stores protected bytes.

## Explicit Non-Goals

- No AppInit/AppCert, DLL injection, registry edits, system-directory writes,
  kernel drivers, CSRSS hooks, or private console/loader ABI.
- No automatic acquisition of MinNT, old NT source, SDK images, guest media,
  or Microsoft binaries.
- No compatibility claim based solely on OpenNT or NTVDMx64 source inspection.
- No Win16 execution claim.

## Next Evidence Needed

1. A lawfully supplied, version-locked runtime manifest with hashes retained
   locally by its owner.
2. Read-only startup traces identifying entry state, low-memory layout, and
   the first required service requests.
3. An owned CPU/machine implementation capable of real-mode execution and
   bounded service dispatch.
4. A separately tested DOS namespace and device policy.

The CPU-backend decision and its explicit non-claim for the current probe are
recorded in `docs/CPU-EXECUTION-BACKEND-DECISION.md`.
