# Part 2: DOSX VCPI source and selected guest-profile boundary

Four owned files `dpmi/dxvcpi.asm`, `dxvcpibt.asm`, `dxvcpi.inc`, `prot386.inc` were hash-bound to both originals in `dpmi-guest-family-observations.json`. Original `dpmi/makefile:40-80` defines a VCPI option but places VCPIOBJ under `!if 0`; its 486 WOW `OBJS486` list excludes these two objects. The inspected x86 `dosxi.lnk` likewise does not name either. These are recipe facts, not proof that no other guest binary or configuration uses VCPI; current immutable DOSX.EXE source-to-binary provenance remains unproved.

`dxvcpibt.asm:1-55` explicitly separates VCPI detection/setup as discardable boot code and guards its body with `if VCPI`. `dxvcpi.asm:35-37,214-247,322-446` contains resident VCPI-specific allocation/free and system-page cleanup under the same condition. The data and allocator are guest VCPI ownership, not proof that host CPU40 always uses these private paging structures. `dxvcpi.inc` is the original shared VCPI interface and `prot386.inc` is a protected-mode declaration input; exact active conditional expansion differs by profile.

| Owned path | Decision |
| --- | --- |
| `dpmi/dxvcpi.asm` | Preserve original resident VCPI implementation for relevant variant; not selected as an object by observed WOW 486 response. |
| `dpmi/dxvcpibt.asm` | Preserve original discardable VCPI setup; do not equate historical source with runtime selected paging. |
| `dpmi/dxvcpi.inc` | Preserve guest VCPI interface for callers/other profile; declaration use does not prove VCPI is enabled. |
| `dpmi/prot386.inc` | Preserve protected-mode interface; its use alone neither selects VCPI nor proves instruction execution. |

`m0-t420-s40-wow32-research-replan.md` explicitly withdraws an earlier assertion that selected NTVDM necessarily uses DOSX VCPI paging. Its original source traces are useful structure evidence, not a current CR0/CR3 runtime witness. Existing T420 guest FPU/protected-mode observations likewise do not establish VCPI, allocation rollback, switch or teardown. No runtime test is executed here.

S11 should correlate actual unchanged guest DOSX, CR0/CR3, mode transition and original entrypoint under the selected runtime before any paging-provider decision. For an actually selected VCPI variant, validate allocation failure, free, setup/discardable teardown and repeated use; S12 retains guest DOS service and S19 binds immutable binary and package hashes. Do not enable VCPI with an isolated object, introduce a second page authority, rebuild guest, or claim default VCPI support based on historical source.

Replay `rg -n "VCPIOBJ|OBJS486|VCPI" src/mvdm/dpmi/makefile src/mvdm/dpmi/dosxi.lnk src/mvdm/dpmi/dxvcpi.asm src/mvdm/dpmi/dxvcpibt.asm`, inspect original profile conditions and run the part-2 DPMI observer. No product or shared ledger changes.
