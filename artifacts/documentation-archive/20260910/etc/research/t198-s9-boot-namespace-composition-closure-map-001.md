# T198 S9 Boot Namespace Composition Closure Map 001

## Question

What is the smallest complete, globally routed provider closure for the finite
read-only boot namespace, without relinking the historical broad
`adapter_runtime` product derivative?

## Inputs

- Global route and ownership records:
  `bx_ntvdm_bop_ingress_v1`, `bx_ntvdm_bop_provider_registry_v1`,
  `bx_ntvdm_dem_plane_v1`, and `bx_ntvdm_command_plane_v1`.
- Provider facade and members:
  `bx_ntvdm_boot_namespace_provider_v1`, COMMAND boot-file service, DEM
  readonly-file service, DEM pathname-search service, readonly namespace and
  immutable profile-search snapshot.
- Existing focused `bx-ntvdm-boot-namespace-provider-test` object inventory
  in `CMakeLists.txt`.
- `bx_ntvdm_adapter_runtime.c` only as negative composition evidence.

## Procedure

1. Enumerated each public provider facade entrypoint and followed it to its
   leaf transaction/result record.
2. Compared those operations with the S8 mantle record: one copied read,
   one or more copied writes, or a bounded copied write.
3. Read the retained runtime call order solely to locate prerequisites and
   reject its environment/session/diagnostic composition.
4. Searched the candidate sources for `adapter_runtime`, `host_session`,
   environment discovery, host paths/handles and direct OS file APIs.

## Whole-Family Route Matrix

| Global route | Provider facade and native owner | Action/result disposition | Closure status |
| --- | --- | --- | --- |
| COMMAND `54:0c`, `54:0d` | `prepare_boot_file` → COMMAND boot-file service | copied multi-range write plus typed resume | selected |
| DEM `50:12` | `prepare_open` → readonly open; `complete_open` | copied 128-byte read, then typed open result | selected |
| DEM `50:00` | `seek` → readonly seek | direct typed resume/failure; no RAM action | selected |
| DEM `50:16` | `read` → readonly read | bounded copied bulk write plus typed result | selected |
| DEM `50:02` | `close` → readonly close | direct typed resume/failure; no RAM action | selected |
| DEM `50:09` | pathname `prepare_first`/`complete_first` | copied 134-byte gather read, then copied multi-write | selected, but needs DTA prerequisite |
| DEM `50:0b` | pathname `prepare_next`/`complete_next` | copied 45-byte gather read, then copied multi-write | selected, but needs DTA prerequisite |
| DEM `50:42` | fast-read provider | source-derived fast-I/O path | excluded: it is not normal readonly lifecycle and remains outside this family |
| DEM FCB forms | FCB search contracts | distinct FCB ABI | excluded: pathname rules must not be projected onto FCB |

`50:09` and `50:0b` take a `bx_ntvdm_dem_dta_registration_v1` input.  The
provider does not own its production.  Therefore a direct provider closure
must include the already separated DEM DTA prerequisite (`50:1b`) as a
composition input, or leave both pathname routes deferred.  It may not invent
a DTA location or mine one from unbounded memory.

## Candidate Object Closure

The provider leaf closure is the existing focused test closure:

```text
CPU/exception/window/result/range/write transaction records
readonly namespace + immutable search snapshot
search session/result/plan/request/transaction
COMMAND boot-file + DEM readonly-file + DEM pathname-search
boot namespace provider facade
```

The executable composition additionally needs the global ingress, registry,
DEM plane and COMMAND plane objects, an adapter-owned synchronous action
conversion/completion object, and the mantle S8 action object.  The DTA
provider is a required input only for the two pathname routes.  No object in
this candidate list is `adapter_runtime`, `host_session`, environment
installation, a host filesystem shim, or a direct byte recognizer outside the
global ingress tables.

## Rejected Closure

The retained `adapter_runtime` dispatch order mixes unrelated COMMAND,
mouse, drive, device, environment, observation and session state with the
selected provider.  Its `queue_*`, `take_*` and completion methods also hold
the old pending state and raw payload lifetime.  It is source evidence for
call sequencing only, not a link input or a replacement boundary.

## Verification

The candidate-source forbidden-dependency scan passed for the selected
provider and leaf objects.  An i686 MinGW GCC 16.1.0 compile linked the entire
focused provider test as a PE32 i386 executable with SHA-256
`24DFF6C9F9663A54C4410F5FA0879DAC0D8BE5FB1D593561E63A0669A3DEA539`
(331,323 bytes).  It exited 0 and reported:

```text
bx-ntvdm boot namespace provider: paired config and normal read-only lifecycle verified
```

The compile intentionally used the CMake-equivalent ordinary warning policy,
not `-Werror`: existing `src/cli/byob_profile.c` reports its unrelated unused
`json_skip_value` static helper under GCC.  No provider source warning or link
failure occurred.  This verifies only the candidate x86 provider leaf closure;
it does not attach the provider to Bochs or run a BOP.

## Interpretation And Confidence

High confidence that the selected unit is a whole provider family, not a
service-by-service patch: all selected services are facade members over the
same immutable namespace and share only three S8 mechanical action forms.
The DTA fact is a real composition dependency; it prevents claiming the
pathname half executable until the prerequisite is explicitly composed.

## Follow-up

The next implementation slice must introduce one adapter-owned provider-plane
executor that invokes this entire facade only after common ingress/plane
classification.  It must own one fixed pending action and completion record,
must take the DTA prerequisite explicitly, and must leave pathname routes
deferred until that prerequisite is supplied.  It must not reuse
`adapter_runtime` or attach only `54:0c`/`50:12` as a special path.
