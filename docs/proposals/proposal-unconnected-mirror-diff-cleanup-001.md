# Proposal — unconnected mirror-diff audit and cleanup

## Admission and objective

Owner request, 2026-10-29: place this candidate first in Queue, to execute
after the current T completes. This proposal allocates no numeric T and does
not replace the active packet. Existing audit progress is recorded here rather
than in a separate `artifacts/exclude-audit.md`.

Audit and remove changes to original-source mirrors which are not connected
to the current product compilation/linking. **Runtime execution coverage is
not a criterion.** An enabled, compiled product path does not need a runtime
trace hit to justify retaining its necessary compatibility changes.

For an uncompiled file, unused header, disabled conditional branch or
unselected object/provider, restore the corresponding original bytes or
original changed block. Do not delete original functionality, enable an
unsupported profile to justify a diff, or retain a change merely because a
historical fixture uses it. Preserve connected changes in the same file.

## Scope and method

1. Freeze the admission-time source identities, pinned original editions and
   actual supported production configurations. Begin with the existing MVDM
   mirror ledger, then inventory the other original-source mirror roots;
   identify any roots outside this package explicitly.
2. Enumerate each substantive diff hunk and format-only difference. Bind C
   files to actual compile commands and product input closure; bind headers
   and included C fragments to their actual consuming translation units.
3. Resolve conditional compilation using those actual commands and compiler
   preprocessing. Check all relevant consumers, not a guessed macro set.
   Cover added, removed and replaced text, macro definitions and nested
   branches. A changed conditional that selects required code is itself a
   connected change even when one of its arms is inactive.
4. Check object/library selection and linked symbols where needed. Do not
   confuse COMMON symbols, generated providers, included source fragments or
   a missing MAP file with exclusion. Do not count an unexpanded macro as
   proof of exclusion merely because ordinary `/E` omits its definition.
5. Restore proven unconnected differences to the pinned original; adjust
   historical test dependencies and divergence registrations as necessary.
   Keep historical audit snapshots as historical evidence, with a successor
   disposition rather than silently rewriting their meaning.
6. Report exact file/hunk counts, original/current hashes, compile/link
   evidence, restored line counts and any unresolved items separately.

Follow [source policy](../etc/operations/policy/source-policy.md), preserve
unrelated work and never modify or rebuild guest images. Do not introduce
runtime coverage requirements into this audit.

## Existing progress — side-conversation checkpoint

This is a checkpoint, **not a completed branch/link audit**. Rebind it to
the actual source and graph identities at admission; the main task continued
editing during the investigation. No candidate product was built or released
by this side conversation.

### File-level restoration already performed

Initial SHA-256 comparison found 203 differing MVDM host C/H files. Four were
restored byte-for-byte from `O:/repos.external/OpenNT/base/mvdm`, after checking
the source against `artifacts/mvdm-audit/frozen-file-reconciliation.csv` and
checking that the destination had no unrelated textual edits. All four hashes
were verified after writing; the scoped `git diff --check` passed.

| Mirror-relative file | Finding and restoration | Restored SHA-256 |
| --- | --- | --- |
| `inc/vdmtib.h` | No occurrence in examined production include dependencies; restored original include and formatting. | `A5CE8099CC9F560EF1B4AB9C0AB9B338021D928B977545A09212E1E67A6703AA` |
| `softpc.new/host/src/nt_aorc.c` | Not selected into production; production uses generated `accessfn.c`. Restored original declarations/definitions and bytes. | `7816128FBD968703F2F7B478B0992902F522F9C6853CA166EB69A904CD965061` |
| `wow32/wkglobal.c` | Not selected into production; difference was newline-only. Restored original bytes. | `7B7E81EDAE26D68F6577A4928274FF7244637807805B268E62D4EE3CE3E7A1E9` |
| `wow32/wklocal.c` | Not selected into production; difference was newline-only. Restored original bytes. | `88FAEB38403ED790FF538383125B7BF1481A125D1F3F9C69F79BC1E6E1EF6ECB` |

Before restoration, the two substantive changes accounted for 40 added and
72 removed lines under Git's end-of-line-insensitive comparison. The other
two files were identical after CRLF normalization. The four restorations
reduced this C/H difference census from 203 to 199. This does not mean 199
files contain unconnected differences. Their submission belongs to the main
worktree delivery; this side conversation did not commit or push them.

`tools/build/Generate-T335S3WowCallbackCcpuNinja.mjs` still directly compiles
`nt_aorc.c` for a historical fixture. Its dependency needs reconciliation;
the fixture was not rebuilt here. It does not establish production selection.

### Compiler-based investigation performed

Read the current `build/.ninja/msvc-x86-release/worker` and `wow32` Ninja
graphs and the NTVDM/WOW32 MAPs. Checked `ntvdm.exe`, `wow32.dll` and
`VDMREDIR.dll` input closure; the redirector target is case-sensitive in Ninja.
The redirector link rule has no MAP output, so missing MAP symbols are not
evidence that its objects are unused.

Ran serial, read-only MSVC `/E /C` preprocessing with actual Ninja compile
commands for **149 directly compiled differing C translation units**, all
with exit code zero. No object or executable was produced. Collected emitted
source locations across 539 MVDM files and compared changed-line ranges with
the recorded conditional scopes. The census contained 199 differing C/H
files; included C fragments and headers are not independent translation units.

This scan produced candidates in the following **15 files**, not 15 proven
unused files. Most also contain necessary active changes. The output was
observed in the side conversation, not saved as a machine-readable receipt;
rerun a reproducible observer before using these counts as closure evidence.

| File | Candidate inactive change or check required |
| --- | --- |
| `dpmi32/dpmiint.c` | `i386` arm changing CX:DX publication of `VdmTib.PmStackInfo`. |
| `inc/softpc.h` | `i386` register declarations/calling conventions, `IntelBase` declaration and a macro continuation; verify all consumers and macro handling. |
| `softpc.new/base/ccpu386/c_main.c` | Decode-diagnostic block, non-PROD SAD event handling, and PIG setjmp change. |
| `softpc.new/base/comms/printer_.c` | Setup diagnostic call inside the MONITOR arm. |
| `softpc.new/base/keymouse/keybd_io.c` | Scan reported non-MONITOR `sas_loadw` changes absent: suspicious mapping result requiring manual preprocessing reconciliation, not an approved restoration. |
| `softpc.new/base/video/egawrtm0.c` | Prototype changes in the inactive alternative to `A_VID || CPU_40_STYLE`. Preserve active selection changes. |
| `softpc.new/base/video/egwrtm12.c` | Corresponding inactive alternative writer prototypes. |
| `softpc.new/host/inc/cfpu_def.h` | Big-endian FP64/FP80 layout changes; verify consumers and distinguish inactive-arm changes from necessary active layout selection. |
| `softpc.new/host/src/config.c` | MONITOR-arm candidate appears formatting-only in independent diff; do not count as a substantive defect. |
| `softpc.new/host/src/nt_bop.c` | Loader replacement in the `i386` arm of `SafeLoadLibrary`. |
| `softpc.new/host/src/nt_event.c` | X86GFX-arm change to blocked/fullscreen condition. |
| `softpc.new/host/src/nt_fulsc.c` | X86GFX-arm system-directory replacement. |
| `softpc.new/host/src/nt_msscs.c` | Non-CPU40 arm spacing change; separate formatting from the necessary changed selector. |
| `vdmredir/vrnetapi.c` | DBG-only changed diagnostic arguments. |
| `vdmredir/vrnmpipe.c` | DBG-only changed diagnostic format/arguments. |

No conditional-block candidate above was restored in this side conversation.
The scan is not exhaustive proof: deleted-only hunks, macro definitions,
partly active hunks, header consumers outside the changed-C sample and
function-level linker elimination need explicit checks. Do not turn this
candidate list into a final count without those checks.

### False positives already excluded

- `dos/dem/demdata.c` and `softpc.new/base/ccpu386/localfm.c` have linked
  COMMON symbols: `_dta_location`, `_current_pdb_location`, `_Cpu`, `_Video`.
  Absence of an object name beside those symbols is not a disconnected file.
- `vdmredir/vrnetapi.c`, `vrnmpipe.c`, `vrremote.c` have explicit objects on
  the `VDMREDIR.dll` link edge. They are not disconnected whole files.
- A header labelled `not-directly-compiled` is not thereby unused.
- Isolated XADD experiment changes under `build` were not production mirror
  changes and must not inflate this inventory.

## Delivery and closure

Produce a finite per-hunk disposition with no unchecked exclusions described
as complete. Restore confirmed unconnected differences, reconcile affected
tests and registrations, and verify exact original bytes for whole-file
restorations. Run appropriate cached builds and checks under the execution
rules; byte restoration of unselected source alone does not justify a new
product release. Any affected connected build requires its corresponding
validation. Finish with a truthful diff-reduction report and governed commit
and push when this candidate is admitted. Do not disturb the active T to
perform this queued work.
