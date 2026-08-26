# Project Goal

Deliver a self-contained, non-invasive `ntvdm.exe` CLI for modern 32-bit and
64-bit Windows hosts. It directly launches admitted DOS `.com`, `.exe`, `.bat`
and `.pif` targets and later bounded WOW16 workloads. It may use public Win32
APIs and ordinary host resources, but never requires replacement Windows
files, a rebuilt kernel/private subsystem, or installation-time host mutation.

OpenNT and Bochs recovery is source-first:

1. compose the canonical original source package unchanged;
2. preserve the reached historical interface, layout, ordering and failure
   contract through the smallest same-shaped adapter;
3. use a registered mirror-private overlay only when an adapter cannot make
   the original unit composable;
4. author new behavior only under a recorded last-resort exception.

The twenty-component architecture separates canonical original MVDM host
runtime, support-library, standalone-tool and firmware-input mirrors:
`opennt-mvdm-host`, `opennt-mvdm-support`, `opennt-mvdm-tools` and
`opennt-mvdm-firmware`. It also
keeps exact `opennt-platform-abi`, complete load-only DOS and WOW16 guest
mirrors, Bochs core and nine adapters, neutral `session`, cross-process
`broker`, and final `app` composition. A preserved/buildable historical tool
is never treated as a host-runtime dependency merely because it shares the
MVDM source tree.

The current execution profile is one active MVDM session per `ntvdm.exe`
process, with multiple processes allowed concurrently. A session must support
the original in-guest DOS/COMMAND/WOW task and parent/child lifecycle. All
project-owned interfaces are multi-instance-safe so future in-process multiple
sessions do not require an ABI redesign; imported MVDM process-global state is
made reentrant only after a dedicated original-source audit.

On both x86 and x64 hosts, imported MVDM code observes a controlled 32-bit
compatibility object space. Session-owned typed mapping-manager instances
associate 32-bit surrogate identities with native HANDLE/pointer-sized
resources, while guest pointers use checked synchronous memory leases. This
keeps 64-bit implementation details out of original MVDM control flow and
prevents x86 builds from relying on accidental native-value identity.

The broker recovers only required cross-process VDM registration, identity,
command-queue, notification and cleanup contracts using public IPC. It does
not recreate CSRSS or the NT4 kernel and never transports local native
resources or guest pointers.

Success means reproducible source and artifact provenance, a manifest-driven
x86/x64 build, auditable package selection, minimal imported-source diffs,
bounded one-session execution, explicit unsupported behavior, and a stable
path to original multi-process and intra-session semantics.
