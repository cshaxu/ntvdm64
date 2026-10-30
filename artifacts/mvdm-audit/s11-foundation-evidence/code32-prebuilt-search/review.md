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
