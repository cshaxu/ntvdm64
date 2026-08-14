# T200 S11 — native CLI/engine entry ABI map

## Question

What is the smallest product entry boundary that can replace the retained
`bochs.exe` shell while preserving the five-component architecture and without
leaking Bochs or BOP semantics into `src/cli`?

## Inputs

- `src/cli/README.md`, `ntdos64_run.c`, and `ntdos64_bochs_engine.c`;
- `src/bx-vdm/bx_ntvdm_composition_runtime_v1.[ch]`;
- `src/bx-mantle/bx_ntvdm_minimal_machine.h`,
  `bx_ntvdm_finite_run.h`, and `bx_ntvdm_mechanical_action_v1.h`;
- `src/bx-vdm/bx_ntvdm_cpu_result_v2.h` and the current boot/native BOP
  composition sources;
- `src/bochs/main.cc`'s retained product-startup hook.

## Existing ownership map

| Surface | Current owner | Useful fact | Not admissible as the new product boundary |
| --- | --- | --- | --- |
| `ntdos64-run` | CLI | Validates profile/root/target, starts one child, owns cancellation and returns child status. | It must not include Bochs headers or guest mechanics. |
| `ntdos64-bochs-engine` | retained CLI shell | Verifies a bundle, writes `bochsrc`, starts external `ntdos64-bochs.exe`, and forwards its exit. | It depends on product configuration, ROM copying, and a foreign process; it cannot report typed composition terminal kinds. |
| `bx_ntvdm_composition_runtime_v1` | bx-vdm | Forms and binds the current package sessions from validated inputs. | Its environment-loading convenience is not a stable CLI-to-machine ABI. |
| `bx_ntvdm_minimal_machine_c` / finite runner | bx-mantle | Owns native machine lifecycle and bounded fixture execution. | Both are C++ private fixture-facing interfaces; neither is a product ABI. |
| `bx_ntvdm_cpu_result_v2` | bx-vdm → bx-core mechanical bridge | Correctly represents a BOP-local resume/stop result. | A BOP stop is not, by itself, a CLI exit result. |
| `src/bochs/main.cc` hook | old Bochs product shell | Demonstrates the currently reached startup sequence. | It imports adapter composition and startup-plan semantics into full Bochs `main`, violating the intended mantle/adapter separation. |

## Required product boundary

The replacement is one native engine executable linked from the minimal
`bx-core`/`bx-mantle` closure and its `bx-vdm` composition, not a rebuilt
`bochs.exe`.  The public crossing is a C-only, copied-data entry in
`src/bx-mantle`, tentatively named `bx_ntvdm_engine_run_v1`:

```c
int bx_ntvdm_engine_run_v1(
    const struct bx_ntvdm_engine_request_v1 *request,
    struct bx_ntvdm_engine_result_v1 *result);
```

The eventual public header may include only `<stdint.h>` and fixed-size copied
records.  It must not include `bochs.h`, C++ classes, BOP selectors/services,
guest addresses, raw host handles, or pointers retained after the call.

### Request ownership

The request is versioned (`magic`, `abi_version`, `struct_bytes`) and carries
only the engine-admitted immutable launch description:

- bounded copied profile/root descriptors or a prevalidated serialized
  selection, plus the declared launch-plan encoding;
- copied drive-policy masks and fixed execution limits selected by the CLI;
- no ambient CWD, host handle, Bochs option, firmware path, or guest-memory
  locator.

`src/cli` remains responsible for parsing, profile admission, process-group
cancellation, diagnostics, and converting engine result categories to its
documented process status.  The native engine entry owns setup/teardown order:
compose `bx-vdm` before guest execution; initialize/reset `bx-mantle` around
the run; always unbind/release composition before returning.

### Result ownership and taxonomy

The result is likewise copied and versioned.  It records an engine category,
not a DOS exit code:

| Category | Owner of fact | CLI interpretation at this stage |
| --- | --- | --- |
| rejected request/profile | CLI or engine admission | invocation/configuration failure |
| composition installation failure | bx-vdm | engine setup failure |
| native machine lifecycle failure | bx-mantle | engine failure |
| finite policy/budget terminal | engine orchestration | bounded-run result, not success |
| typed BOP controlled stop | bx-vdm with mechanical acknowledgement | guest terminal requiring later lifecycle policy |
| ordinary guest completion | future COMMAND/DOS lifecycle evidence | not currently claimed or assigned a status |
| host cancellation | CLI | cancellation result |

The `terminal_kind` field must retain these distinctions.  In particular, it
must not collapse the S9 typed stop into zero/success or expose an internal
`bx_ntvdm_cpu_result_v2` as the public result.

## Dependency direction

```text
src/cli (opaque C ABI; process ownership)
  -> src/bx-mantle native-engine entry (lifecycle orchestration)
     -> src/bx-vdm composition install/reset (host-service plane)
        -> src/bx-core (opaque machine mechanics)
```

The entry may call `bx-vdm` installation/reset functions, but `bx-core` and
`bx-mantle` remain selector-blind.  `bx-vdm` remains the only owner of BOP
identity and provider selection.  The legacy full-product `src/bochs/main.cc`
hook must be deleted or compiled out only in the later implementation S after
the new entry has equivalent startup and teardown coverage; S11 does not
modify it.

## Procedure

Read the current entrypoints, composition installation/reset sequence, private
mantle interfaces, and typed BOP result ABI.  Compare ownership against the
architecture's CLI and core/mantle/vdm boundaries.  No source was modified,
no build was run, and no guest execution occurred.

## Interpretation and confidence

High confidence on the current call graph and its boundary violation: the
legacy engine delegates to a complete `bochs.exe`, while the source-built
composition is currently injected through `src/bochs/main.cc`.  High
confidence that a C-only engine request/result boundary is required.  Exact
serialized request fields, product executable location, normal DOS completion
semantics, and process exit-number mapping remain implementation design work;
they cannot be inferred from the fixture's status.

## Follow-up

Admit T200 S12 as a single native-engine contract implementation package:
define the public C request/result header plus negative ABI tests and a
non-executing setup/teardown fixture.  Do not replace the old engine, remove
the Bochs hook, or run a guest until that contract and its object/link closure
are proven.
