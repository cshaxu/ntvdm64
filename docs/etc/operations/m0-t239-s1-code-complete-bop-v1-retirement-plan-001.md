# M0 T239 S1 — Code-complete BOP-v1 retirement

## Question

Which retained `src/bx-vdm/bop-v1/` source, header, fixture and non-product
reference still corresponds to a BOP that already has a code-complete current
OpenNT-derived route, and can therefore be deleted now?

## Rule

For every BOP marked code complete in the live tracker, its old v1 provider
material must be removed as part of that completion. A shared v1 file may
remain only when a concrete unfinished BOP still compiles or tests against it;
the ledger names that file and consumer. A filename suffix such as `_v1` on a
current mechanical ABI is not itself v1 provider material and is out of scope.

## Procedure

1. Inventory all 50 retained `bop-v1` files and reverse-map their production,
   fixture and header consumers.
2. Classify each source unit by BOP-family responsibility against the tracker:
   code-complete owner, unfinished owner, shared-only, or obsolete.
3. Delete all code-complete and obsolete v1 units plus their unneeded tests;
   repair non-product includes only when removal proves them stale.
4. Record residual v1 units and their exact unfinished consumers.
5. Rebuild the formal graph and execute current v2 fixtures. No v1 fixture is
   retained merely to preserve historical comparison code.

## Completed retirement ledger

### Provider and composition source

All 50 former source/header inputs below were deleted.  None has a remaining
unfinished-v1 consumer: unfinished owner packages will start from their
OpenNT-source recovery route rather than retain a project-authored fallback.

```text
bx_ntvdm_adapter_runtime.h
bx_ntvdm_bop_catalog_v1.c             bx_ntvdm_bop_catalog_v1.h
bx_ntvdm_bop_ingress_v1.c             bx_ntvdm_bop_ingress_v1.h
bx_ntvdm_bop_provider_registry_v1.c   bx_ntvdm_bop_provider_registry_v1.h
bx_ntvdm_bop_selector_probe.c         bx_ntvdm_bop_selector_probe.h
bx_ntvdm_composition_runtime_v1.h     bx_ntvdm_cpu_result_bridge_v1.h
bx_ntvdm_debugger_package_facade_v1.c bx_ntvdm_debugger_package_facade_v1.h
bx_ntvdm_dpmi_package_session_v1.c    bx_ntvdm_dpmi_package_session_v1.h
bx_ntvdm_dpmi_plane_v1.c              bx_ntvdm_dpmi_plane_v1.h
bx_ntvdm_emm_unavailable_service.c    bx_ntvdm_emm_unavailable_service.h
bx_ntvdm_exception_observer_v1.c      bx_ntvdm_exception_observer_v1.h
bx_ntvdm_host_service_catalog.c        bx_ntvdm_host_service_catalog.h
bx_ntvdm_host_service_plane.c          bx_ntvdm_host_service_plane.h
bx_ntvdm_host_session.c                bx_ntvdm_host_session.h
bx_ntvdm_legacy_plane_gate_v1.h
bx_ntvdm_machine_bop_facade_v1.c       bx_ntvdm_machine_bop_facade_v1.h
bx_ntvdm_mouse_install1_mapping_service.c
bx_ntvdm_mouse_install1_mapping_service.h
bx_ntvdm_native_bop_composition_v1.c   bx_ntvdm_native_bop_composition_v1.h
bx_ntvdm_printer_unavailable_service.c bx_ntvdm_printer_unavailable_service.h
bx_ntvdm_redir_package_facade_v1.c     bx_ntvdm_redir_package_facade_v1.h
bx_ntvdm_redir_unavailable_provider_v1.c
bx_ntvdm_redir_unavailable_provider_v1.h
bx_ntvdm_spckbd_init_service.c         bx_ntvdm_spckbd_init_service.h
bx_ntvdm_system_plane_v1.c             bx_ntvdm_system_plane_v1.h
bx_ntvdm_top_level_package_facade_v1.c bx_ntvdm_top_level_package_facade_v1.h
bx_ntvdm_vdd_create_user_notify_service.c
bx_ntvdm_vdd_create_user_notify_service.h
bx_ntvdm_wait_if_idle_service.c        bx_ntvdm_wait_if_idle_service.h
```

The one current non-product include formerly reaching the catalog did not
retain a provider.  Its frozen pointer-free family/request record now resides
in `src/bx-vdm/bx_ntvdm_host_service_contract.h`; the record's `_v1` name is
its ABI layout version, not an implementation selection.

### Tests and tools

The 37 v1-only C fixtures, PowerShell boundary tests and probes were deleted
with the provider tree, as were four stale build helpers: the former native
container configurator, the former narrow startup-transaction root creator,
the boot-file-predicate derivative helper, and the startup transaction
boundary test.  They referenced only deleted provider/catalog symbols and
were not formal-manifest members.  The four retained T230/T231/T236/T237
governance gates are deliberately negative checks: they assert that no
`bop-v1` route can reappear.

### Residual classification

There is no residual `src/bx-vdm/bop-v1/` tree, no selected product source and
no retained v1-only fixture.  Existing names ending in `_v1` below current
`bx-core`, `bx-mantle` or `bx-vdm` describe fixed mechanical ABI versions or
current observation records; they are outside this retirement scope and
remain formal-manifest inputs where listed.

## Verification result

- Fresh formal-r3 generated and built all **353/353** MSVC x64 `/MT`,
  CPU5/P-MMX Ninja edges, producing 51 executable targets.
- The focused OpenNT-derived current-route sweep passed **31/31** eligible
  fixtures: 10 before the disposable-volume fixture, plus 21 remaining DEM,
  COMMAND and native-session fixtures.  `t236-s4-ntfdisk-readonly-fixture`
  was intentionally not counted: it exits with its documented setup request
  until `NTDOS64_T236_S4_TEST_DRIVE` names an owner-authorized disposable VHDX
  drive.  It has no dependency on the retired v1 tree.
- Negative route and ownership gates passed: T230 DEM route precedence, T231
  COMMAND v2 route, T236 DEM/COMMAND retirement, T237 XMS retirement and the
  T237 machine-semantic boundary.
