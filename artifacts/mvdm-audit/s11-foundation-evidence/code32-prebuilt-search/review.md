# Prebuilt DPMI interrupt witness: code segment distinction

The historical S38 record names `D38K.COM` as a 32-bit-code-segment interrupt
and fault-return witness. A current filename scan under repository `build/`
found no `D38K.COM`. A separate content scan of `.COM` files under `build/`
for `S38_INT32_RETURN_OK` found the three basenames in `basenames.txt`; this
does not search the entire machine or prove no differently labelled witness
exists. The only additional prebuilt basename is `S39I32.COM`.

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
