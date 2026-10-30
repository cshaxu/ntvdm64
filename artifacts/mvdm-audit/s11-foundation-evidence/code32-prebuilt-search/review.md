# Prebuilt DPMI interrupt witness: code segment distinction

The historical S38 record names `D38K.COM` as a 32-bit-code-segment interrupt
and fault-return witness. A current filename scan under repository `build/`
found no `D38K.COM`. A separate content scan of `.COM` files under `build/`
for `S38_INT32_RETURN_OK` found the three basenames in `basenames.txt`; this
does not search the entire machine or prove no differently labelled witness
exists. The only additional prebuilt basename is `S39I32.COM`.

The four Git revisions that changed `assets/winnt.zip` were also inspected
read-only in memory. None contains a `D38K.COM` entry or a `.COM` entry with
the expected `S38_INT32_RETURN_OK` marker. The reproducible script and result
are `search-old-zips.py` and `old-zips.txt`. This covers these archives, not
other historical packages or media.

The retained first 40 instructions of the exact `S39I32.COM` and
`S39H32.COM` hashes show a 32-bit DPMI client request (`AX=1`), followed
directly by 16-bit code after the far call. Neither has the descriptor clone,
default-size-bit update and far jump which the current `CODE32` test source
uses before its 32-bit instruction stream. The existing `S6PROBE.COM` is the
same hash as `S39H32.COM`. These facts cannot prove every possible executable
path, but they do not support treating either prebuilt as the dedicated
32-bit-code interrupt/fault-return witness.

No guest was built or modified. The dedicated current-release CODE32
interrupt/fault-return boundary remains unverified; the separate existing
32-bit code-segment INT 21 witness is not the same test. User's guest
immutability restriction remains in force. This is an inventory result, not
a product regression or a reason to modify original CCPU behavior.

## Existing CODE32 guest: actual DPMI dispatch observation

The unchanged prebuilt `D38R32.COM` was rerun against the current staged
package with the existing `MVDM_S42_DPMI_BOP_REPORT_PATH` trace enabled.
The guest emitted both required CODE32 INT 21 and failure-cleanup markers;
Run16 exited zero, and the temporary Z: mapping was removed. The trace file
contains 914 `dpmi-bop` lines covering indices 00, 01, 02, 03, 04, 05, 06,
07, 08, 0a, 0c, 0d, 0f, 10, 11 and 13; none reports index 15 or 17, the
selected 32-bit interrupt and fault-return entries. The relevant reporter
is called by `DpmiDispatch`; it does not record guest CS descriptor mode,
and failures to open/write its optional log are silent. Thus this run does
not promote the CODE32 INT 21 probe to an interrupt/fault-return witness or
prove those entries unreachable. The guest and product binaries were not
modified or rebuilt.

The complete trace SHA-256 is
`6807AA02F0C5CB379A09DFEE9ABCFABD2D6F31BA156A5CAFD96154B7295FB350`;
the guest console SHA-256 is
`E5B87AB5CAE9E3CC7C50672017D94475002D54BD88940E1F9DF084254D8DE5EC`;
the isolated runtime image manifest SHA-256 is
`204D67973C30B17AC581A303890D8F053A8343721411E1AEF09AFB9B32E20C85`.
Results and the exact eight recorded image hashes are under
`build/t473-s11/dpmi-code32-entry-trace-run/`; the full trace is
`build/t473-s11/dpmi-code32-entry-trace.txt`. This is a bounded real guest
observation, not exhaustive exception or interrupt validation.

## DOS4GW and NESticle: non-passing fault-loop observation

The existing `O:/assets/nesticle-x.xx/DOS4GW.EXE` (SHA-256
`B8265123AC8A189637448618409EF3ECD2E9F3E1A47062C685A02240F688DEC1`)
was copied without modification into a test-owned package and invoked without
a program argument. The existing observer timed out after 25 seconds. The DPMI
trace contains one index 15 and 212277 index 17 events. This is an observed
non-pass, not proof that standalone no-argument operation is supported or that
an actual 32-bit code-segment return succeeded.

A separate test copied the user's unmodified `NESTICLE.EXE` (SHA-256
`224E348E1FD5C11B16EF096FF5607C7C57C49D65FA7E4DBBC9B2A5C9C53E832B`)
and the same DOS4GW to the isolated package, exposing the test directory on
PATH. The application copy was named `S6PROBE.EXE`, so name/working-directory
dependencies have not been excluded. During the 15-second observation the
existing product fault trace recorded 36729 vector-0d dispatches with
`frame32=1`: first from `00c7:00004978`, then repeatedly from
`01c7:00006b51`, with handler `01c7:00006abb`. The BOP trace separately
recorded one index 15 and 36728 index 17 events. The observer timed out and
reported no graphics handshake. This shows repeated fault-dispatch/return
entry under instrumentation, **not** successful returns, a proven DPMI source
fault, or a confirmed code-segment default-size bit. The test app is
interactive, so timeout alone cannot establish that its normal exit failed.
A separate no-trace run also timed out with no graphics handshake, but did
not independently establish the same exception sequence. No guest binaries
were changed or rebuilt.

Both owned stages were cleaned up and their Z: mapping removed. The trace
reporters can silently fail to write; the observed lines are positive events,
not completeness proof. The first 4-second attempt never started the observer
and is not a product result. Later investigation must retain the original
file name and startup directory, establish a valid application interaction,
identify the exact faulting instructions and segment descriptors, and compare
with the original host before deciding whether this is an in-scope connection
failure or an excluded independent CPU issue. Do not count this as the missing
successful CODE32 interrupt/fault-return witness.

Evidence SHA-256: standalone BOP trace
`E185AF83DE2DF33AD88AEEAEAAD9A2F1830FEAFB82829CC7DA87FFF1F1763895`;
NESticle fault trace
`501535C8E583F6BE89252C3947C233B3694940C0C4EED4FD6F0F630CBAEA7422`;
NESticle BOP trace
`85EA6925634B76D145F78FD3F6C8DA7D6994EBE90C38CF8F24B836B22FAB97D6`;
traced and untraced observer reports respectively
`511CF091DF4637AAAF28617F1B419D9DFA6F727D02CF0ED21DEFBCF7E6F7D1C3` and
`C5A979F07C1A559DE4C0A8A78CEBF9CFF63116902280F9E3F008A727202D9BD7`.
Full traces and private runners remain under `build/t473-s11`.
