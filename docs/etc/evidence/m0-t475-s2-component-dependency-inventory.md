# T475 S2 — component dependency inventory

## Question and baseline

The owner asks for all illegal dependencies between non-public, non-mirror
components. A production component may select common, lib-nxvm, lib-win32,
mvdm-host and non-mvdm; only NTVDM and NTVWM may select worker-base. Explicit
runtime protocols and the existing NTVDM-provided module ABI are recorded
separately from source ownership and must not be silently removed. No repair
or behavioral concession is admitted by this inventory.

Baseline `f677b8131` contains the four T475 S1 library commits and local
package `2b1325bd-17fb-4435-b2be-55995de325f6`. The repository's ten host
images and their project-owned component roots are in scope, including both
Hook architectures, plus worker-base and independent addon source references.
Original mvdm-host and non-mvdm trees are shared mirrors, not private products.

## Procedure and limits

Inspected the generated x86 `worker-x86` and `wow32` production graphs and
the native `worker-native`, `frontend`, `launcher`, `service`, `monitor` and
`hook64` graphs under `build/.ninja/t475-handle-compare-p`. Traversed only
explicit object/archive link inputs recursively; did not treat an order-only
build edge or an import-library producer as if its source were statically
linked. Compared the resulting selected source candidates with the actual
production `.map` files. `ninja -t deps` on production-owner objects checked
compiler-recorded direct and transitive headers, rather than only spelled
`#include` paths. The generated graphs and compiler dependency database are
the built T475 S1 P4 configuration, not proof for every possible alternate
configuration, preprocessor branch, generated header or runtime behavior.

The ordinary VDMREDIR link did not emit a map. A diagnostic x86 link used the
same `ninja -t commands VDMREDIR.dll` link inputs and toolchain, with output,
import library, `/map` and `/verbose:lib` redirected to `build/t475-s2`.
It exited zero; command, PID, times, output and map are retained in
`build/t475-s2/link-audit.run`, `link-audit.stdout` and `audit-vdmredir.map`.
This diagnostic DLL was not published or used as product acceptance evidence.

## Actual source and selected static implementation

| Consumer | Private owner selected | Evidence and use | Disposition candidate |
| --- | --- | --- | --- |
| Run16 | NTHook `context.cpp`, `hook.h` | `frontend_scope.c` calls `nthook_context_read`; the x64 Run16 map selects `nthook-context:context.obj`. Reads inherited hook/launcher payload and handles. | Move the product-specific shared context contract and implementation to a suitable common owner; keep hook-specific interception policy in NTHook. Preserve payload identity, handle lifetime and errors. |
| NTVDM | NTHook `context.cpp`, `installer.cpp`, `hook.h` | `command_process_compat.c` calls `nthook_context_paths` and `nthook_install`; the x86 NTVDM map selects both Hook-owned objects. Installs propagation into a suspended native child before resume. | Share product-level context/installation orchestration through common, using existing `lib-win32/process-hook` for low-level injection. Keep worker creation and rollback policy with NTVDM. |
| NTVWM | NTHook `context.cpp`, `installer.cpp`, `hook.h` | `execution.c` calls the same paths and installation operations; the x64 NTVWM map selects both Hook-owned objects. Propagates authenticated frontend/execution capabilities before target resume. | Same shared provider as NTVDM, while NTVWM retains native-target admission, resume and rollback. |
| WOW32 | NTVDM `win32/crt_compat.c` | WOW32 production graph compiles this source directly as `obj/adapter-win32/crt_compat.obj`; its map selects `_sprintf`. This is the modern CRT spelling/ABI compatibility adapter. | Reassess the bounded CRT adapter for a shared `compat`/platform owner, preserving exact calling convention, original unbounded contract, provenance and both architectures' selected symbols. |
| VDMREDIR | NTVDM `win32/thread_start_compat.c` | VDMREDIR selects `softpc-win32-bindings.lib`; diagnostic map selects `thread_start_compat.obj`, including `opennt_create_cdecl_thread_named`. Its thread thunk binds/unbinds NTVDM session state and calls NTVDM-exported session operations. | Separate reusable calling-convention mechanism from worker-specific session binding only after tracing exact callbacks, owner lifetime, exception cleanup and redirection callers; do not relocate the state wholesale or create a second owner. |

The diagnostic VDMREDIR link input also lists `worker-base.lib` and
`softpc-bindings.lib`. Its map selects no member of either archive. The
Worker Base input is therefore an unnecessary prohibited link input, not an
observed live Worker Base function dependency. Remove only after a targeted
negative-link/build check verifies it is unnecessary in the supported profile.
The initial `ninja -t inputs` report followed the `ntvdm.lib` producer and
overstated NTVDM sources as VDMREDIR inputs; the explicit-edge and link-map
inspection above supersedes that inference.

## Private headers and runtime ABI

| Consumer | Provider | Evidence and interpretation |
| --- | --- | --- |
| Run16, NTVDM, NTVWM | NTHook | Each production source above directly includes `nthook32-dll/hook.h`. These declarations and the selected implementation form the same three live source-boundary violations, not extra independent implementations. |
| WOW32 | NTVDM | Twelve direct source/header include sites name NTVDM private paths. Compiler dependency data for the 70 selected WOW32-owned objects shows 40 objects directly or transitively include eight NTVDM-owned headers: `session/guest_memory_lease.h`, `session/session.h`, `softpc/include/mvdm_softpc_firmware.h`, `mvdm_softpc_guest_memory.h`, `mvdm_softpc_wow_page_domain.h`, and `wow/include/wow_gdi_alias.h`, `wow_user_runtime.h`, `wow_user_session_binding.h`. Their actual use includes guest-memory lease and WOW page-domain/USER/GDI contracts. Move only genuinely shared declarations to a shared owner after resolving their state/lifetime contracts; do not copy session state into common. |
| VDMREDIR | NTVDM | `mvdm_redirector_guest_copy.c` directly includes `redir/include/mvdm_redirector_worker_copy.h`; compiler dependency data for its three project-owned objects also shows `softpc/include/mvdm_softpc_symbol_compat.h` via build selection. The copy functions are actual NTVDM imports. Put the shared declaration at an appropriate common boundary while retaining the worker's sole guest-memory authority. |

The WOW32 map contains 184 NTVDM import thunks, including guest-memory and
page-domain APIs. The diagnostic VDMREDIR map contains 38, including guest-copy
and session operations. These are real runtime module ABI edges and not proof
that NTVDM's full source is statically included. The current architecture
recognizes this ABI separately from source ownership; changing it requires a
distinct interface and lifetime design, not deleting imports to satisfy an
ownership graph. Counts are for this built package/profile only.

## Other components and follow-up

The inspected NTCON, NTSRV, NTMON and Hook32/64 production closures did not
select another product component's private source; Run16's and the workers'
Hook exceptions are listed above. NTVDM and NTVWM select worker-base, which is
the owner's explicit exception. Worker Base's inspected x86/x64 source
closures select no private product source, and its owned source uses common
contracts. NTCON objects occur in other components' test executables, not in
their inspected production link closures. No addon source reference to the
ten private component roots was found; addon build scripts and non-C dynamic
loads were not comprehensively audited.

This is a production-graph and source ownership audit, not an assertion that
each selected runtime branch was exercised. Confirm archive members and header
contracts in alternate supported graphs before applying each fix. Preserve
the current package, original guest behavior, callback sequencing, session
ownership and cross-component ABI during any subsequent remediation.
