# M0 T273 S3: minimal app machine-shell closure

## Candidate audit

The quarantined app/session material was reviewed before any reuse.

- Current `session/session.{c,h}`, `mapping_manager.*` and
  `guest_memory_lease.*` are already the T272-reviewed neutral replacement:
  explicit caller identity, instance-owned state and context-bearing teardown.
  They are retained unchanged.
- `src.old/app/engine_*`, `startup_*`, `byob_*`, `bop_composition.*` and
  `app/bop/**` are rejected for this S.  They embed CLI launch selection,
  OpenNT providers, BOP routing, guest-image input, host APIs or historical
  runtime-stage policy.  Copying any would violate the admitted shell boundary.
- The earlier `src.old/session/session.*` is also not copied: its global
  identity allocator and callback-without-context teardown are less safe than
  the already-admitted neutral implementation.

## Delivered composition

`app/machine_shell.{c,h}` is the entire new app assembly body.  It binds an
already-active caller-owned `session` to a machine created exclusively through
`adapter-bochs/machine_lifecycle.h` and registers a context-bearing LIFO
teardown.  Its public record contains only version/state and a `session *`;
no Bochs object, guest locator, service selector, host handle or callback
escapes the component boundary.

The shell has no run, load, BOP, provider, worker or scheduler method.  S2
continues to own raw machine operations; later composition work must choose
its own admitted higher-level session contract rather than expanding this
minimal shell opportunistically.

## Verification

The focused fixture initializes and activates a neutral session, opens one
1 MiB machine through the app shell, then disposes the session.  Disposal
invokes the registered LIFO teardown and makes the shell inactive.

- MSVC x64 `/MT`: incremental compile/link into the P3 149-object closure and
  native run succeeded (`exit 0`).
- MSVC Win32/x86 `/MT`: the same focused compile/link/run succeeded (`exit 0`).

The only emitted runtime diagnostics are the native CPU reset, CPU5 CPUID and
RAM allocation lines.  The disposable binaries remain below
`build/M0-T273-S2/r005*`; no guest image, BOP, OpenNT host body or product
device shell entered either execution.

## Result

S3 is closed.  The final T273 S4 verifies the complete directed boundary:
production consumers, include paths, overlay privacy and both host-width
fixtures.  It must not add application behavior merely to make the audit pass.
