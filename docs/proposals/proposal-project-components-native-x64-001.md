# Proposal — Native x64 launcher, frontend and monitor

## Status and dependency

Owner places this unnumbered candidate at Queue head, immediately after the
current native-worker/Hook package. No numeric T or implementation is admitted
by this planning change. The current package delivers NTVWM32/64 and
nthook32/64 while all other components remain x86. This candidate then migrates
run16.exe, ntcon.exe and ntmon.exe to x64-only builds without changing their
roles. NTSRV, NTVDM, WOW32.DLL and VDMREDIR.DLL remain x86; both NTVWM and Hook
variants remain available. NTCON is the visible frontend, not the native worker.

## Objective and boundary

- Preserve run16 classification/submission/direct-result semantics and CLI.
- Preserve NTCON's worker-neutral Console/Window, logical surface and single
  NTSRV-authorized I/O connection; preserve NTMON's service-only projection.
- Rebuild each selected static dependency and RPC client for its consumer ABI,
  with separate x86/x64 generated/object/library/CRT roots below build/.
- Retain NTSRV control authority, actual native child identity/waits/results,
  original x86 CCPU40/WOW execution and the established worker handoff.
- Keep all installed images at the existing product-relative system32 paths.
  Do not globally disable WOW64 redirection or silently change user search.

No x64 MVDM/NTSRV/WOW32/VDMREDIR, new scheduler/registry, frontend kind branch,
guest mutation or additional resident helper is selected. The preceding
package's approved installer mechanism is reused, not redesigned here.

## Source-first porting audit

At admission recheck actual link maps, not merely archive names. Existing maps
show run16 selects original vdm.c classifier/command/environment code,
csrutil.c capture and RTL environ/error; NTCON selects project Base bindings,
not those original bodies, and NTMON has neither closure. Historical x86
assembly being present in a library does not prove it is linked into a consumer.

Audit native SECTION_IMAGE_INFORMATION/syscall declarations, original
same-machine classification, pointer narrowing, task-ID versus HANDLE carriers,
private TLS PEB/TEB projections, environment sizes and capture ownership. Record
which bytes reach the public OS ABI and which remain private local structures.
Do not assume every ULONG is a pointer or every capture field is exercised.

Attempt unchanged original composition with an architecture-correct bounded
ABI facade first. If a source change is unavoidable, register the smallest
owner-correct diff and compare retained behavior. A large uncomposable slice
requires a reviewed alternative, not permission to write another classifier.
Run16 and both Hooks retain one discovery/recognition contract; no independent
resolver or extension-only parser. Reuse the preceding Hook64 ABI conclusions.

## Proposed implementation sequence

At numeric admission allocate bounded S briefs for: dependency/ABI audit and
design; x64 frontend/monitor and shared RPC bindings; x64 launcher/Base-client
composition and both Hook-to-launcher context directions; coherent package
regression/publication and owner acceptance. This sequence allocates no S now.

## Acceptance

Prove actual image widths and independent object closures; x64 clients with
x86 NTSRV, authenticated resource transfer and version-mismatch negatives;
Hook32/64 context-only delivery to x64 run16; native32/64 and DOS/Win16
classification; native child real handles/exit codes; independent roots,
disconnect/failure cleanup, worker reuse and DOS/native bidirectional handoff.
Retain Console17/Window17 and independent WOW frontiers against the preceding
verified package. Seal/recover/publish one coherent eleven-image set and verify
its hashes. Compiler/linker success alone is not runtime acceptance.

All generated/build/test products remain below build/; preserve other-session
changes. This proposal changes no production source, process or deployed file.
