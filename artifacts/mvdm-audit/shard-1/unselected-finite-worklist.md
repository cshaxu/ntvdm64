# Unselected host and standalone tooling: finite worklist

Frozen universe SHA256 F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196. 61 shard-1 pending paths are listed individually in `unselected-finite-worklist.csv` with actual/frozen hashes, exact target-input membership, package-c hash matches, original role, evidence and explicit next action. These actions are grouped by owner below; none is silently reclassified as fully restored or excluded solely because a worker graph omits it.

Target graph identities:
- `build/.ninja/msvc-x86-release/worker` / `ntvdm.exe`: SHA256 `3706BBEAE9FEF8D67A9BAAC6E5CAFF1A7BDB44077F97DAB0DC1FCB0DC3433F83`
- `build/.ninja/msvc-x86-release/worker` / `VDMREDIR.dll`: SHA256 `3706BBEAE9FEF8D67A9BAAC6E5CAFF1A7BDB44077F97DAB0DC1FCB0DC3433F83`
- `build/.ninja/msvc-x86-release/wow32` / `wow32.dll`: SHA256 `03FE1C4AEB2CFB7434FA1471C55A3EF62652254615E630BA5861AE7F8729E278`
- `build/.ninja/pkg-uuid-x86/worker` / `ntvdm.exe`: SHA256 `B1BC7B95C2A9DBC1A9DD8CC88931D86941EE146017C37A5ECCB6843FA80AC470`

## Finite owner groups

1. Debugger extensions: BDE, IEUVDDEx, VDMEXTS and WOW debug commands are separate from selected original dbg.c transport. Resolve each command’s modern target structures, public API and cleanup; do not cast opaque modern pointers to NT4 private data.
2. Fax: follow selected WOW fax caller, installed guest driver, spooler UI and lifecycle; decide actual missing provider capability.
3. SIM16/SIM32: packet transport / test utility, not a CPU executor; determine intended remote debug function and whether it is in admitted product scope.
4. CMOS and legacy host error/termination/UI/HUNTER: compare original conditional paths and selected owners, enumerate missing observable behavior, and avoid duplicated authorities.
5. Firmware assembly and bin86 guest media: establish actual byte lineage and capabilities with no guest modification or rebuilding.
6. Shared includes: inspect per-header actual consumers / preprocessing and distinguish product use from immutable guest build inputs; do not mark excluded merely because individual header is not in Ninja inputs.
7. VDD samples and rename build tool: verify original build consumers, then distinguish standalone demo/generator itself from generic VDD behavior; seek existing scope authority before any exclusion.
8. GRAFTABL, PIFEDIT and WIN: retain distinct original utility semantics and known codepage handoff mismatch; compare installed tools and error/exit behavior.
9. Cursor cache: reconcile historical claims against actual source/linked provider, verify handle/resource lifetime.

The CSV is a finite triage and evidence checklist, not proof that all capabilities need new implementation. No new user authorization is invented. None of these entries has been marked overall excluded: available evidence establishes component roles and specific candidate work but not an authoritative exclusion of every feature. Ninja dependency logs positively identify nine of the 21 shared headers as used by cached product objects; their exact observed consumers are listed per row. Graph absence does not prove remaining indirect includes, runtime loading or guest capability. No product code, guest or package was altered.
