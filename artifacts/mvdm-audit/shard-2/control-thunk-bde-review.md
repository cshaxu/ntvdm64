# S10 shard 2: control-thunk fragment and BDE debugger sources

`wow32/wctlm32.c` is byte-identical in both frozen source comparisons,
SHA256 E5C1218929F64A5BB1C2227AB1903E095E0DB50E3A1F44DBB4AD4A028579B8CA.
It is neither in the original WOW32 `sources` list nor the selected current
product graph. It starts with bare `case` labels and includes naked
asterisk placeholder lines at 212 and 224, so the frozen input is a partial,
non-compilable control-message conversion fragment, not a complete runnable
translation unit. No non-self references to its named `ThunkEMMsg32` and
similar functions were found in the current source search. The separately
selected original `wmsg16.c` table dispatches EDIT, combo and list 16-bit
message operations to selected `wmsgem.c` etc.; their existence does not prove
that every converse 32-to-16 message/ownership behavior is equivalent to this
fragment. Do not include the fragment as a build fix or claim the entire
control-message family restored. Pending on source-role disposition and a
function-level conversion/callback/copyback evidence matrix.

`bde/data.c` defines WinDbg/NTSD extension callbacks and process/thread
context globals. `bde/disasm.c` implements x86 guest disassembly; original
`bdecmds.c:535-567` command `u` calls `Unassemble`, which obtains the guest
context and reads code via BDE helpers. Original `bde/sources:41,56,83-90`
builds a separate `bde` debugger-extension DLL, not the main VDM/WOW runtime.
Both files are unselected by the frozen product build. They are not evidence
that normal guest execution lacks a CPU instruction decoder, nor does an
external modern debugger prove the old `!bde.u` extension restored. Until
original debugger-tooling scope and any replacement/acceptance are decided,
keep both pending rather than claim capability equivalence or exclusion.

No guest, component, or tests were run or modified. Original call/source and
build evidence is static; runtime and user-visible debugger behavior are not
proven by this review.
