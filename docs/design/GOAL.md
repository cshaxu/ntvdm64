# Project Goal

Deliver a self-contained, non-invasive NTVDM CLI for modern Windows hosts.
The current product executable is the x86 `ntvdm32.exe`; native x64 compilation
is not a product or acceptance target. It directly launches admitted DOS `.com`, `.exe`, `.bat`
and `.pif` targets and later bounded WOW16 workloads. It may use public Win32
APIs and ordinary host resources, but never requires replacement Windows
files, a rebuilt kernel/private subsystem, or installation-time host mutation.

OpenNT and SoftPC recovery is source-first:

1. compose the canonical original source package unchanged;
2. preserve the reached historical interface, layout, ordering and failure
   contract through the smallest same-shaped adapter;
3. use a registered mirror-private overlay only when an adapter cannot make
   the original unit composable;
4. author new behavior only under a recorded last-resort exception.

The production architecture separates canonical original MVDM host runtime,
standalone-tool and firmware-input mirrors: `mvdm-host`, `mvdm-tools` and
`mvdm-softpc-firmware`. Its original support paths remain inside
`mvdm-host` at their upstream-relative locations. It also
keeps exact `mvdm-platform-abi`, complete load-only DOS and WOW16 guest
mirrors, the original SoftPC CCPU40 execution package and its bounded
historical-interface adapters, neutral `session`, cross-process
`broker`, and final `app` composition. A preserved/buildable historical tool
is never treated as a host-runtime dependency merely because it shares the
MVDM source tree.

The current execution profile is one active MVDM session per `ntvdm.exe`
process, with multiple processes allowed concurrently. A session must support
the original in-guest DOS/COMMAND/WOW task and parent/child lifecycle. All
project-owned interfaces are multi-instance-safe so future in-process multiple
sessions do not require an ABI redesign; imported MVDM process-global state is
made reentrant only after a dedicated original-source audit.

Imported MVDM code uses its original Win32/x86 process-local 32-bit carriers
where they are directly composable. Guest pointers remain checked synchronous
memory leases. A fixed-width original ABI that cannot carry a pointer may use
only its own source-shaped narrow table; no generic identity-token manager is
part of the product. Every admitted recovery package builds and links its
reached closure under Win32/x86. Native x64 compile/link is neither product
evidence nor a source-repair obligation.

The broker recovers only required cross-process VDM registration, identity,
command-queue, notification and cleanup contracts using public IPC. It does
not recreate CSRSS or the NT4 kernel and never transports local native
resources or guest pointers.

Success for the current recovery phase means reproducible source and artifact
provenance, a manifest-driven Win32/x86 build for the original CCPU40 profile,
auditable package selection, minimal imported-source diffs,
bounded one-session execution, explicit unsupported behavior, and a stable
path to original multi-process and intra-session semantics. Original SoftPC/
CCPU40 is the sole supported original SoftPC machine profile. Any functional
acceptance that exercises a machine, guest, or MVDM host behavior must record
the selected x86 CCPU40 row. `CPU_30_STYLE` is an NT4
kernel-VDM V86 monitor contract, not a CCPU interpreter profile; it is
historical source evidence only and is prohibited from production build, link,
runtime and acceptance inputs. Historical Bochs material is retired comparison
material only and is not a product backend, build input or fallback.

`CPU_30_STYLE` is retired, not deferred: no current or future project build
configuration may define it. Its remaining appearances are immutable OpenNT
mirror conditionals or historical evidence, neither of which authorizes a
second CPU profile.
