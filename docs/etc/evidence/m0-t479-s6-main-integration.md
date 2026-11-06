# T479 S6 P2 — combined main integration delivery

## Scope and source identity

The owner requests fast-forward import of `task/mirror-source-audit`, followed
by a new combined build, applicable focused verification, Full gates and a
coherent eleven-image publication. Main was clean at f095c77bf; the ancestry
check found exactly67 commits through661a1315f. `git merge --ff-only` succeeded.
No reset, history rewrite or force push was used. The
[rebase handoff](m0-t478-main-rebase.md) remains historical, not combined proof.
Application0.0.479, protocol67 and the active T479 S6 scope are retained.

The post-import production sources are unchanged. The only additional code
changes are test infrastructure: the CPU/PIC fixture now rejects unexpected
VM bridge/command entry instead of linking an incomplete production frontend,
and the slave-BIOS test rebuilds its actual Ninja prerequisites before linking.
These test-only seams terminate with ERROR_CALL_NOT_IMPLEMENTED; they do not
return fabricated success or enter product libraries. Original guest images
and mirror code receive no additional change in this integration checkpoint.

## Combined verification

All runs use main-worktree sources and the current cached MSVC x86/x64 trees,
with repository-local ccache. Source-worktree assets are not release inputs.
Evidence is owned by `build/t479-s6-integration`; the
[machine receipt](m0-t479-s6-main-integration.json) binds retained logs/results
and publication identities by SHA256.

| Verification | Result |
| --- | --- |
| Complete selected host builds, including VCDEX | x86 and x64 build001 exit0; shared suites14/14 on each architecture. |
| Existing command, VM frontend, broker, frontend lifetime and handoff fixtures | release001 passes its retained assertions before Full. |
| Full product gate |16/16 selected gates pass in409839ms; verifier reports identical11-file package. External DOS4GW/Nesticle fixture remains explicitly skipped. |
| Imported focused owners |13 fixture runners pass: DLC worker event, redirector async baseline, DIB failure/retirement, EDIT tabstops, OLE root, fax, file cache, font, palette, spool/loader and Winsock rundown. Negative baseline observations are not claims of corrected original defects. |
| GDI alias/class brush; XMS; slave BIOS/PIC | Alias fixture errors0; class-brush collision0; XMS139/139; slave BIOS EOI74/74. |
| S6 text model | Current mvdm-vm-text CTest passes, including candidate fill operations. |
| Actual Win3.1 fill and exit | fill003/004/005 each pass both VM identities,320 callbacks, three continuation forms, guest readback and normal system exit. Original installation bytes unchanged. |
| Actual nested task chains | nested007 passes both guest VMs, actual nested MEM output before EXIT and normal Win3.1 exit. Provider input is not physical-input acceptance. |
| Actual WOW guest behavior | Kerning, palette and embedding reports have errors0; WINMINE flag/counter, reset, opened cell, WM_CLOSE and Run16 completion pass. |
| Actual ROM RTC behavior | RtcEntry and RtcAlarm pass normal launcher exit, wait/count/flag semantics and alarm callback/EOI restoration. |

Initial slave-BIOS linking used stale fixture libraries. Rebuilding exposed
the missing test isolation boundary for the newly connected VM entry points;
the bounded correction above resolves it. The first guest-kerning invocation
used an absolute path where its existing runner expects a repository-relative
output; the corrected invocation passes. The first gameplay invocation was
rejected by its existing live-broker guard while the separately owned RTC
probe was exiting; after that probe finished, serial gameplay passes. Failed
attempts remain in the receipt and are not counted as successful runs.

## Preserved failure and original limitations

The [pre-import fill002 failure](m0-t479-s6-pre-import.md) is **not repaired or
reclassified as passed**. Its retained trace already contains six successful
application acknowledgements, six completion consumptions and both VM
retirement notifications, but lacks final system exit. The same preserved
old package rerun as oldfill006 now exits normally, as do the three combined
package runs. This narrows the observation beyond initial fill publication
but does not establish the historical timeout's root cause. It remains an
unlocalized S6 failure obligation; no product workaround, longer timeout or
semantic concession was added. This P delivers the tested combined baseline,
not S6 closure or a claim that intermittent normal-exit failures are impossible.

The controlled alternate-host RTC fixture reproduces the previously recorded
67 checks/13 failures. It is **not a passing gate**. The
[T478 S35 disposition](m0-t478-s35-rtc-investigation.md#final-disposition-and-reproducible-verification)
identifies the normal path as the original ROM, whose actual guest tests pass
again here. No revival or repair of that unselected alternate route is added.
Physical optical/audio devices, every original allocation failure and complete
S6 physical input/fonts/graphics are not certified by this delivery.

## Publication and review

Fresh package795498ef-6a71-4fd8-a82c-0f0a7d02e76c contains the ten existing
host images plus newly built VCDEX.DLL. All eleven images, the hook license
and one common manifest are copied to both `assets/release` and
`O:/winnt/system32` and hash-verified. Prior files are backed up with explicit
absence records for the new DLL/manifest. Only exact installed project
process paths are eligible for publication shutdown. The independent addon
remains an S6 candidate and is not copied into System32.

Implementer/reviewer inspection verifies the owner request, preserved commit
history, selected sources, no original-guest mutation, test-only boundary
changes and exact package identity. Documentation governance and whitespace
checks accompany this P commit/push. S6's47-operation I/O completion and the
retained fill002 observation remain active; no T/S closure is claimed.
