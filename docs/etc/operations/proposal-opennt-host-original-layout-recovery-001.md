# OpenNT host original-layout recovery proposal

## Proposed task

**M0 T261 — OpenNT host original-layout recovery and composition closure**

## Decision

`opennt-host` must follow the same source-fidelity rule as `opennt-guest`:
the component root may replace the historical repository prefix, but a retained
OpenNT translation unit keeps its historical relative directory, filename and
body.  A different filename such as `*_shim.c` is not an OpenNT mirror merely
because it implements a related contract.

The task does not require importing every OpenNT host source file.  It requires
every **admitted** source-derived host capability to have one of four explicit
dispositions:

1. direct original translation unit, at its re-rooted historical path;
2. original translation unit with only source-marked `DIVERGENCE(<ID>)` edits
   to call `adapter-win32` or `adapter-softpc`;
3. project-owned composition adapter, moved out of the original path to its
   correct adapter, ingress, or `app` owner; or
4. unavoidable host-owned source-derived capability, with a named recovery
   exception that cites the unavailable original composition path.

No new BOP behavior is authorized by this layout-recovery task.

## Source of truth

The file-by-file baseline is
[T261 S1 host source-truth audit](m0-t261-s1-opennt-host-source-truth-audit-001.md).
It is the only permitted inventory for the migration.  A later discovered
source or dependency must be added to that audit before it enters an S.

## Proposed S sequence

| S | Outcome | Exit condition |
| --- | --- | --- |
| S1 | Freeze current 50-file host inventory, exact-content result, original-source candidates and owner dispositions. | Every live file has one disposition; no source move or behavior change. |
| S2 | Prove the original source/ABI/failure path for every non-identical file and decide its target component/path. | Each row has a first-usable recovery-rung decision and no unresolved generic `shim` category. |
| S3 | Recover the PIF/COMMAND host group under re-rooted original `softpc.new/host/...` and `dos/command/...` paths; externalize composition-only files. | Direct imports retain original body; every body edit is source-marked and registered. |
| S4 | Recover or relocate DEM host support by original source owner; preserve host-file API, error order and handle/mapping contracts. | No `dem/*_shim.*` survives without an explicit original path or source-derived exception. |
| S5 | Split the selector-blind copied-frame ingress from `opennt-bop` into `adapter-bop`, preserving OpenNT selector/service routing in `opennt-bop`. | `adapter-bop` owns the generic typed ingress/completion ABI; it names no selector, service family, DOS/WOW algorithm or provider, and `opennt-bop` no longer owns generic machine-event mechanics. |
| S6 | Reconcile the ten-component target across architecture, goal, coding, rules and the active-packet terminology. Do not move implementation. | Component count, current build-module count, mapping ownership, adapter terminology and S follow-on ownership are internally consistent. |
| S7 | Split the dependency-free per-VDM `session` foundation out of current composition/adapter state. Move only neutral lifecycle, resource/token, capability-registration, completion/event and teardown ownership; keep `app` as instance owner and retain all OpenNT/BOP/Win32/Bochs meaning in its existing owner. | `session` has no product-component dependency or service vocabulary; all extracted state has an exact former/new-owner map; no component calls upward into `app` to obtain session state. |
| S8 | Remove selector-aware BOP/NTDOS/COMMAND observation and fixture-only residue from adapter production roots; relocate source-owned interpretation to `opennt-bop` and generic test hooks to `tests/` without altering provider behavior. | `adapter-bop` and `adapter-softpc` are selector/service/family/DOS/WOW blind in production code; component roots contain no fixture-only provider. |
| S9 | Resolve XMS, Redirector, top-level and configuration support with the same rule. | No generic `xms/`, `redir/`, `top_level/` or `config/` shim remains unclassified. |
| S10 | Repair manifest/includes/tests, delete superseded host shims, run formal Ninja closure and audit source/README exception one-to-one. | All production `opennt-host` paths meet the re-rooted-original or explicit-exception rule. |

## Target layout rule

Examples of admissible paths:

```text
refs/opennt/base/mvdm/softpc.new/host/src/nt_pif.c
  -> src/opennt-host/softpc.new/host/src/nt_pif.c

refs/opennt/base/mvdm/softpc.new/host/inc/nt_pif.h
  -> src/opennt-host/softpc.new/host/inc/nt_pif.h
```

An adapter source may not masquerade as an OpenNT file.  For example, a new
composition wrapper for `nt_pif.c` must have a distinct, registered adapter or
composition name and must not replace the imported `nt_pif.c` body.

## Constraints

- Preserve the mandatory recovery ladder: original source, smallest same-shaped
  adapter, registered external intrusion, then new behavior.
- `adapter-win32` owns unavailable Win32/NTDLL API implementations;
  `adapter-softpc` owns SoftPC/CCPU/SAS-to-Bochs mechanical conversion.
- `adapter-bop` owns selector-blind copied-frame ingress and typed completion;
  `opennt-bop` owns BOP route and selector/service meaning; `app` owns final
  composition and creates each `session`. `session` has no product-component
  dependency and owns only neutral lifecycle/resource/token/capability/event/
  teardown contracts. This task must move any misplaced file to its true owner.
- Do not create generic `adapter-common`, `adapter-host`, or `compat` roots.
  A VDD/debugger, Redirector or WOW package remains under its original
  OpenNT owner unless a later source/ABI audit proves that a specialized
  adapter is necessary beyond `session`, `adapter-win32`, `adapter-softpc`,
  and `adapter-bop`.
- A `DIVERGENCE` edit in an imported OpenNT body has one statement-level ID and
  one component-README row.  A source-derived file has a separate recovery
  exception; it is not represented by a blanket README row.
- References, obsolete shims and replaced source bodies move to
  `docs/etc/legacy_code/`; tests remain below `tests/`.

## Verification

- For direct imports: relative path/name and SHA-256 equality against
  `refs/opennt`, or exact enumerated source edits.
- For each adapter/rehost: original call site, retained interface/layout/order,
  unavailable dependency, failure behavior, owner and focused fixture.
- Static scan: no generic `*_shim.*` remains in `opennt-host` without an audit
  row and explicit final disposition.
- Formal T260-derived manifest/Ninja build, `git diff --check`, documentation
  governance, and an independent single-person review of the final diff.

## Non-goals

- No completion claim for unavailable VDD, CSR/BaseSrv, Redirector, WOW or
  machine product shells.
- No import of a full reference tree merely to make file counts look complete.
- No new BOP selector/service enablement, no Bochs change and no source-build
  guest change.
