# Historical standalone VDM utilities: source and captured package

Five shard-2 pending files relate to three separate historical utilities, not to a single worker implementation. Three source/declaration files match both original editions; the two `obj/i386/*.com` files are current-only and absent from both originals. The observer verifies these identities, original source edges, current worker/WOW32 Ninja graph snapshots, and complete file-size/SHA-256 checks of `assets/winnt.zip`. Both `.com` files have neither a same-name package entry nor an exact renamed copy; their same-name files are absent from installed System32.

| Entry | Evidence and recommendation |
| --- | --- |
| `vdmutils/graftabl/graftabl.h` | Original `graftabl.c` includes this message-ID header; original `sources` declares a standalone console utility and resource. Candidate **historical standalone utility input** in the observed build profile. This is not proof of code-page/graphics behavior or all consumers. |
| `vdmutils/graftabl/obj/i386/graftabl.com` | Current-only binary of unknown provenance; no exact captured package copy. **Pending** provenance and alternate delivery. It must not be relabeled as an original or assumed produced by the adjacent recipe. |
| `vdmutils/pifedit/pif.def` | Original module definition describes legacy PIFEDIT and exports window procedures. The adjacent `sources` selects a separate PIFEDIT library/application surface, but exact `.def` consumption is not proven. Candidate **historical standalone utility metadata**, or pending if the ledger requires actual link proof. |
| `vdmutils/pifedit/pifedit.c` | Original source has `WinMain` and PIF editor window procedures; adjacent `sources` selects it as a standalone shell utility, not the captured worker/WOW32 graph. Candidate **historical standalone utility source** for that profile, not PIF parsing/launch capability exclusion. |
| `vdmutils/win/obj/i386/win.com` | Current-only binary of unknown provenance; no exact captured package copy. **Pending** provenance and alternate delivery. Original `win.c` is a native process launcher with CreateProcess; do not confuse this source with a guest Windows 3.x launcher or infer the current-only binary was produced from it. |

Historical source recipes, object identity and package absence do not prove runtime invocation, feature coverage, alternate installation paths or link provenance. Existing governance `tools/governance/Audit-T276S11W0W1Disposition.ps1` separately distinguishes the standalone original utilities from the MVDM host runtime. This audit did not execute utilities, run historical builds, or change guest, product, build trees or shared ledger.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/vdmutils-family-observer.py` and compare JSON.
