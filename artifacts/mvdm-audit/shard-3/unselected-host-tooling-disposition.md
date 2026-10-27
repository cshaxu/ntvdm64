# Unselected host source and standalone-tool disposition: shard 3

The 23 initially pending `unselected-host-source` rows are enumerated in `unselected-host-tooling-disposition.csv` with original target/role and the remaining capability obligation. All 23 are frozen exact-original rows, not mirror-difference rows. The original SOURCES, actual product graph, `docs/design/GOAL.md` and T473 S1 disposition distinguish host runtime from standalone/debugger/guest roles; **that distinction does not itself exclude an original user capability from this T**.

Four rows are excluded in this T only as specifically bounded build/test or other-architecture inputs: OEMUNI `toemuni.c` UMTEST; `dat2obj.c` build generator; `dasm386/build.c` PPC/MIPS/Alpha disassembler build carrier; `obj.vdm/c_regs.c` A3CPU-to-CCPU bridge gated out by the selected CCPU architecture. Generated data, OEM conversion, disassembly behavior and CPU register correctness remain independent capabilities; the exclusion cannot stand for their success or absence.

Nineteen rows remain pending, in the following finite capability groups:

1. **BDE, IEUVDDEx and VDMEXTS debugger extensions (9 rows):** inventory each original command/caller and whether an admitted debugger integration supplies equivalent observable behavior; where not, record concrete missing command/host ABI and seek an explicit product scope disposition. The separate original debugger DLL target is evidence of ownership only.
2. **PIFEDIT `mods.c` (1 row):** identify user-visible edit/modify operations and compare present PIF utilities; retain missing operations or obtain an explicit product-scope decision. It cannot be dismissed merely because PIFEDIT is a standalone original target.
3. **DPMI allocator:** compare selected portable `dpmimemr.c` observable allocation/resize/failure behavior to original i386 monitor owner; do not import native monitor wholesale.
4. **CPU shim:** map each `ntstubs.c` helper to selected CCPU ownership or approved profile boundary; avoid duplicate CPU/video globals.
5. **Host error text:** trace reached `hs_err_message` consumers/current error resource and preserve or explicitly register differences.
6. **Virtual floppy:** identify admitted product boundary and actual provider/guest interface; original `VFLOPPY` code is real optional functionality, not the test trace shim.
7. **ForceDOS:** trace CREATE_FORCEDOS through Run16, reservation, command record and child classification with actual end-to-end evidence. Original SOURCES uses TARGETTYPE=LIBRARY with UMAPPL=forcedos; separate utility ownership does not exclude forced-child semantics.
8. **FastWOW:** compare selected slow thunk against original fast path's observable dispatch and performance while respecting monitor dependency closure.
9. **Three BIOS sources:** immutable guest build inputs. Pin the actually loaded image and compare relevant startup/configuration behavior using independent probes; no guest modification or rebuild.

Pending is not a claim of a present bug. This audit changes no product, guest, combined ledger or runtime and ran no new tests. Mirror differences must remain partial; none of these 23 rows is a mirror-difference row.
