# M0 T281 S2 — original DEM common-control composition plan

## Purpose

Establish the lowest-dependency original DEM compilation island: `dem.c`,
`demdata.c`, `demmsg.c`, and `demdisp.c`. These files provide the package
globals, diagnostics, and service-table control flow; their provider targets
remain declarations only in this S.

## Source-first disposition

1. **Original source:** compile the four selected OpenNT files. `dem.c`,
   `demmsg.c`, and `demdisp.c` remain byte-exact. `demdata.c` retains only the
   existing, registered MVDM-HOST-DIV-005/006/007 descriptor changes.
2. **Same-shaped bindings:** use only existing support/platform declaration
   carriers and existing adapter-softpc/adapter-win32/session forms.
3. **External intrusion:** none; Bochs is not a build or source dependency.
4. **New behavior:** none. A missing declaration or ABI is recorded and stops
   this island instead of gaining a local provider stub.

## Delivery sequence

1. Record the direct include and original/divergence identity of all four
   files, including their non-provider dispatch-table references.
2. Generate formal x86/x64 Ninja graphs that compile and archive only these
   objects; do not link a runnable executable or a provider body.
3. Verify the graph rejects run/link/provider inputs and record the first
   missing source-shaped boundary if either architecture stops.
4. Review the result, update evidence/governance, and deliver a sequential P.

## Exclusions

- Calling `DemDispatch`, enabling `50:xx`, or linking any DEM provider body;
- namespace/filesystem behavior, guest-memory I/O, host mutation, or a trace;
- new adapters, new session policy, Bochs changes, and COMMAND work.

## Exit criteria

- Each direct declaration import has one named source/owner/disposition.
- The four-file archive compiles on x86 and x64, or the smallest incompatible
  source-shaped boundary is documented before any implementation is proposed.
- The graph has no executable/run rule, BOP ingress, provider source, or
  Bochs source input.
- Documentation governance and `git diff --check` pass before P acceptance.
