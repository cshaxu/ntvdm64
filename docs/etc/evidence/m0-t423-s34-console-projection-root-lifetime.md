# T423 S34 Console projection and root lifetime

## Question and inputs

After S33, the owner observed `cmd.exe` → `run16 command` → `dir` leave
part of the directory summary beside the DOS prompt in Windows Terminal.
Live cell probes distinguished the 28-row DOS logical page from the larger
physical viewport: rewriting one row and a terminal refresh did not remove
the visible tail, whereas clearing and replaying the whole current page did.
The owner approved publishing the complete logical DOS page into the physical
Console, with blank cells outside the DOS page; host scrollback history is not
an acceptance requirement. The owner also asked that an NTKVM borrowed from an
existing Console retire when that outer Console actually ends, and that its
NTVDM and NTW32 workers share the resulting root-loss semantics.

The S33 published protocol-25 package is the comparison baseline. Original
MVDM mirror files and guest media were not modified.

## Implementation and boundary

NTVDM still performs the original DOS cell writes, erases, scrolling and
cursor operations on its logical VGA page. NTKVM now copies a complete page
to the canonical visible Console, maps the logical cursor and blanks physical
cells outside the logical page. It grows the canonical backing buffer where
necessary instead of rejecting a larger physical viewport. It does not parse
DIR output, inject a redraw, reflow text or change DOS row selection.

The producer page is the current truth; repeated page projection need not
preserve all earlier text in the host's scrollback buffer. This is an
explicitly approved product-display difference, not a claim of byte-for-byte
OpenNT host scrolling. Per-command snapshots in the nested MEM test retain
evidence for each execution rather than requiring every output to remain in
the final physical buffer.

The frontend-root lease is protocol 26. Run16 and NTSRV establish an
authenticated root relationship; worker-base gives both worker types the
same root-loss handle contract. An NTKVM borrowed from an existing Console
waits on its attached external Console anchor. When that process exits, it
rechecks the Console members once and transfers the anchor or retires if no
external member remains. This is handle-driven, not interval polling and not
a task/Win32Record observer. An NTKVM rooted in a newly created Console follows
its owned-root lifetime. NTSRV remains the registration authority; NTKVM
root loss is not interpreted as a completed DOS/native task.

## Verification

All intermediate artifacts and transcripts are under
`build/M0-T423/S34/`; the x86 build completed the selected full graph.
The coherent eight-file runtime came from
`build/M0-T423/S34/frontend-lease/runtime/`.

| Gate | Result | Evidence |
| --- | --- | --- |
| 80×30 outer CMD → COMMAND → first and repeated DIR | Pass; `dirty-prompt=0` and real directory text | `probe/final-repeat-dir.raw*` |
| 80×24 first VER and DOS continuation | Pass; `preserved=1 dos-alive=1` | `probe/fullbuild-24-ver.raw*` |
| 17 Console product routes | 17/17 expected exits and text witnesses | `frontend-lease/s34-final-console17-summary.json` |
| 17 private-desktop Window routes | 17/17 expected exits and text witnesses | `frontend-lease/s34-final-window17-summary.json` |
| Zero-delay nested MEM typeahead | Pass; three distinct MEM completions observed in successive snapshots, not inferred from the final page | `frontend-lease/s34-final-typeahead-nested-mem-typeahead.txt.line-*.console.txt` |
| Native root and borrowed Console lifetime | 8/8 cases pass; observed NTKVM and both worker kinds retire on root loss; NTSRV follows after its idle interval | `frontend-lease/s34-native-root-anchor-*` |
| Native/DOS nesting, focused frontend and channel fixtures | Pass with their required hidden native Console or private-desktop harnesses | S34 build transcripts and test reports |
| WOW headless frontiers | Pass: WINMINE reaches its window; SOL and WRITE reach their known original out-of-memory modal frontiers | S34 WOW frontier report |
| Published package smoke | Pass: `O:\winnt` CMD → COMMAND → DIR has `dirty-prompt=0`; `run16 cmd.exe /d /c ver` prints the Windows version and exits 0 | `probe/published-dir.raw*`, `frontend-lease/published-cmd-ver.txt*` |

The directly inherited shell/ConPTY did not provide the geometry expected by
some focused Console fixtures; those fixtures were run in an isolated hidden
native Console, while the real outer-CMD observer and product matrices cover
the Console integration. A first WOW run without the package network profile
failed its expected NETWORK.DRV-dialog condition; rerunning with the required
profile passed. Neither is counted as a product failure or concealed as an
unqualified first-pass success.

The published eight files were hash-checked against the staged runtime:

| File | SHA-256 |
| --- | --- |
| run16.exe | `AD5C559DED95EC0249853B38141869CCE16131B14A7FBE5DA31FB5AD93B1513E` |
| ntsrv.exe | `7A44B6CA6D041460F04E48652A0073E77E19C63DF8549F79B8FFC5E34B9079D6` |
| ntvdm.exe | `107F9736A812EFE038B8065C1A05F62B92919B1D49543E1C95B163AAD3BF3E93` |
| ntw32.exe | `62AAC859B8273DB940ECD5FF6419E8A6D8F0BDDE55E302EF90820B56146FFD03` |
| ntkvm.exe | `962D50B67F58F0AFFC7359AB28901A0F0AF893AC70B296DDFBFFB189E57D96FE` |
| ntmon.exe | `454EB97A05195A23A90C984F250633F1D2D12305D37774DB38A04C67F6695FC2` |
| wow32.dll | `0D2AE60264B03A8040D98AA86BCF80455127064084E2217D318D5E13F90FA94A` |
| VDMREDIR.dll | `2B254A207C34B8F8191805B4805AF5982938633F04F90E151C5E95E329E76BDA` |

The pre-publication package is retained in the S34 build directory. Owner
side-testing on the actual Windows Terminal/RDP presentation remains a
separate acceptance observation; S34 does not close T423 or admit S35.
