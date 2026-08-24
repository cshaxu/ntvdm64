# M0 T261 S9 P1 — Adapter semantic-residue owner map

## Purpose

Classify every live production source in `adapter-bop` and `adapter-softpc`
whose name, input or control flow still has OpenNT BOP/guest-family meaning.
This is an ownership map, not permission to rewrite behavior while moving it.

## Result

### Move unchanged to `opennt-bop`

These files are invoked from `opennt_bop_route.c`, decode BOP window bytes, or
describe a particular OpenNT guest family. They cannot remain in a
selector-blind SoftPC adapter.

- `bx_ntvdm_bop_sequence_observation_v1.{c,h}`
- `bx_ntvdm_command_bootstrap_observation_v1.{c,h}`
- `bx_ntvdm_command_current_dir_observation_v1.{c,h}`
- `bx_ntvdm_dem_open_observation_v1.{c,h}`
- `bx_ntvdm_demfile_create_observation_v1.{c,h}`
- `bx_ntvdm_generic_ud_sequence_observation_v1.{c,h}`
- `bx_ntvdm_ntdos_exec_observation_v1.{c,h}`
- `bx_ntvdm_ntdos_exec_entry_observation_v1.{c,h}`

Their current production consumers are `opennt-bop/ingress/opennt_bop_route.c`,
`app/ntdos64_native_cli.c`, and in the DEM-create case the OpenNT Win32
facade. Moving their paths and module membership is source ownership repair;
their fixed-width observation ABI stays unchanged.

### Delete from production; retain only fixtures

The following are not provider semantics and have no live product consumer
outside test fixtures. Their sources/headers must leave the adapter library;
their focused tests may retain a local test helper copy if still useful.

- `bx_ntvdm_normal_return_observation_v1.{c,h}`
- `bx_ntvdm_normal_terminal_sequence_observation_v1.{c,h}`
- `bx_ntvdm_terminal_observation_v1.{c,h}`

P3 completed this disposition by deleting the three production pairs and
their isolated tests.  The former direct-engine fixture now validates only its
machine terminal result, not a BOP observer's private static state.

### Split source-owned BOP interpretation from mechanical aliases

- `bx_ntvdm_dem_direct_context.{c,h}` and
  `opennt_dem_ccpu_sas_facade.{c,h}` currently classify DEM service numbers,
  path/data spans and provider-specific input layouts. That interpretation
  moves under `opennt-bop/dem`; the lower `opennt_ccpu_sas_facade.h` remains in
  `adapter-softpc` because it contains only the CCPU/SAS call-local aliases.
- `bx_ntvdm_host_transaction_abi.{c,h}` and its `host_service_contract`
  identify `50:xx` and `54:xx` ranges. The only direct consumer is a fixture;
  delete them from the production library and retain/rehome the fixture helper.
- `dpmi_descriptor_source_shim`, `dpmi_startup_source_shim` and
  `dpmi_startup_session_shim` are source-shaped DPMI owner shims and move to
  `opennt-bop/dpmi`. `dpmi_xmem_record_adapter` has no production consumer:
  P4 removes it from `adapter-softpc` and retains its isolated historical
  allocation exercise only as a test-local helper.

### Adapter-bop fixture residue

`adapter-bop/bx_ntvdm_generic_ud_bridge.h` currently exposes
`generic_ud_context_fixture_v2` and terminal-observation controls. These are
not an ingress ABI. P2 must move them behind test-only compilation or into the
fixture tree while retaining the selector-blind production bridge signature.

## P4 completion result

P4 moved the DEM direct-context pair and the DEM CCPU/SAS source-shaped pair
to `opennt-bop/dem`. Its BOP-facing CCPU/SAS aliases now live under
`opennt-bop`, while `adapter-softpc` retains only the lower machine request
and checked-memory APIs they call.  The three DPMI source shims moved to
`opennt-bop/dpmi`; their records now carry statement-local `BOP-DIV-067`
through `079` evidence.  The P4 audit also proved that
`dpmi_xmem_record_adapter` had no production consumer, so it is now a
test-local helper instead of adapter production code.

The cached `s9-r001` formal graph refresh rebuilt 131 affected actions after
the DEM move, with no `bx-core` object action.  The DPMI follow-up rebuilt only
the five DPMI source objects, the `opennt-bop` library and its two fixtures;
startup and descriptor fixtures pass.  `t251-s3-redir-ingress-fixture.exe`
returns `8` identically from the pre-move `s9-r002` and post-move graph, so it
is recorded as pre-existing Redirector/mailslot behavior, not a P4 regression.

## Explicitly retained adapter-softpc content

Historical `include/mvdm` and `include/softpc.new` headers remain only as the
required source-level ABI closure for imported OpenNT owners. They are not
adapter provider implementations. The following source groups remain
mechanical after this audit: CPU state/result, guest range/read/write, unique
mapping managers, A20, IVT, machine engine/stage, physical IRQ/port actions,
and the same-shaped SoftPC/CCPU/SAS facades with no family classification.

## Implementation order

1. Move the eight pure observation pairs and update all include/module paths.
2. Delete/rehome the three fixture-only observations and remove their archive
   inputs.
3. Re-root DEM source interpretation; keep the neutral CCPU/SAS alias seam.
4. Re-root DPMI source shims and remove the fixture-only XMEM record from
   production after its consumer review.
5. Remove `adapter-bop` fixture ABI residue, rerun the semantic-token scan,
   then complete formal closure.

## Guardrails

- No selector/service literal may be moved into `bx-mantle` or `bx-core`.
- No static observation state may be replaced with a new session manager.
- No observation may become a new product BOP handler merely because it moves
  under `opennt-bop`.
- Existing `adapter-softpc/include/...` historical header closure is retained
  until imports no longer need each header.
