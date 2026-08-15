# Proposal: contained CLI interactive COMMAND session

## Intent

After the first DOS profile has completed its full BOP package work and a
real one-shot DOS target has run through normal guest termination, add an
**opt-in** interactive `COMMAND.COM` profile.  The outcome is a persistent
DOS prompt whose input, output, process lifetime, and exposed files are
explicitly selected by the CLI caller.

This is not an attempt to recover the Windows NT4 desktop/session manager.
It is a modern, non-invasive CLI capability that preserves OpenNT's guest
control flow:

```text
COMMAND.COM needs work
  -> BOP 54:01 / original COMMAND contract
  -> bx-vdm routes to a selected CLI host capability
  -> one declared input command, or an explicit end-of-input result
```

The guest never receives ambient access to the host console, working
directory, process list, registry, devices, or filesystem.

## Preconditions for admission

This is an unnumbered candidate T, not current work.  It may be admitted only
when all of the following are independently evidenced:

1. every BOP family admitted for the first DOS profile has a package-level
   source/ABI/failure disposition and family regression;
2. the CLI has run a real declared one-shot DOS target through normal guest
   termination, rather than stopping at a synthetic fixture boundary;
3. terminal result transport has a distinct fixed-width engine/CLI contract;
4. the exact OpenNT sources and guest callers for console, standard handles,
   command input/output, redirection and COMMAND lifecycle have been mapped;
   and
5. every proposed host capability has a default-off policy, copied ABI,
   negative test, and explicit failure/disconnect behavior.

## Proposed ownership

| Layer | Responsibility | Explicit exclusion |
| --- | --- | --- |
| `src/cli` | Parse opt-in interactive flags; own terminal attachment, input policy, output sink, Ctrl+C policy, process lifetime, audit and terminal result presentation | DOS semantics, BOP decoding, Bochs types, implicit host drive exposure |
| contained host-capability seam | Fixed-width reads/writes and explicit end-of-input, output failure and cancellation results | Host pointers, implicit Windows handles, guest protocol interpretation |
| `src/bx-vdm` | Route source-mapped OpenNT COMMAND/DEM/redirector contracts to selected capabilities and return checked copied results | Generic shell, filesystem, console-driver, CPU, device or Bochs policy |
| `src/opennt` | Normative guest callers, layouts, ordering, and original failure behavior | A modern runtime-linked NT4 session manager |
| `src/bx-mantle` / `src/bx-core` | Only separately admitted PC mechanics required by a reached guest caller | BOP selector/service meaning, command, DOS, console or host policy |

## Capability profile

The default profile remains one-shot and non-interactive.  The future
`interactive-console` profile must declare, rather than infer:

- terminal input source and EOF behavior;
- terminal output/error sinks and encoding policy;
- admitted guest drive/file projection and current-directory mapping;
- whether standard handles and redirection are enabled;
- Ctrl+C / cancellation and timeout behavior;
- an audit/logging destination; and
- whether any child capability is allowed.

An omitted capability is unavailable with the original source-proven failure
or deferred behavior; it is never silently replaced with an ambient host
facility.

## Delivery shape

Admission should split the work by component package, not by trace hits:

1. **Source and ABI map.** Map original COMMAND console/bootstrap/launch,
   DOS device/handle paths, DEM file/search paths, and any reached redirector
   contracts; classify original provider, adapted provider, source-derived
   provider, unavailable or deferred.
2. **Terminal capability seam.** Define versioned copied input/output,
   EOF, cancellation and failure records with default-off and negative tests.
3. **Guest console and handle package.** Implement only the reached original
   DOS/COMMAND contracts, including standard-handle/redirection dispositions;
   do not pretend that `54:06` alone is a console implementation.
4. **Interactive COMMAND lifecycle package.** Connect explicit command input
   to the existing OpenNT-shaped `54:01` lifecycle, retaining normal
   `COMMAND.COM` return and end-of-input termination semantics.
5. **Integration and governance.** Run a bounded interactive transcript and
   a cancellation/disconnect case; verify no capability leaks into the
   one-shot default profile.

Each package requires its own source map, failure map and regression before a
native transcript is used for integration observation.

## Deliberate exclusions

This proposal does not admit GUI, VDD, printer/serial passthrough, unrestricted
host drives, host process launch, WOW16, or legacy NT4 session/CSRSS recovery.
DOS programs may execute other **guest DOS** programs using guest DOS process
semantics once that guest capability is complete.  Launching or waiting for a
host Win32 process is a separate future host-launch capability, not an
interactive-console side effect.
