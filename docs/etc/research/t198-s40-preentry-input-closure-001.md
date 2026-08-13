# T198 S40 pre-entry input closure 001

## Delivered boundary

`bx-vdm` now owns a fixed-width opaque pre-entry input record with at most four
checked physical ranges and 64 copied payload bytes. It rejects malformed
headers, empty input, length/offset overflow, physical overlap and payload
overlap. It converts a valid record to the pre-existing generic
`bx_ntvdm_mechanical_action_v1` write record; no new Bochs operation exists.

The private mantle finite request carries that copied action. After bare
machine initialization and before its existing preserve → NTIO write →
restore → CPU-entry sequence, mantle validates and executes the action through
the existing all-or-nothing ordinary-RAM mechanic. An absent/malformed action
is rejected before CPU entry. Mantle contains no profile, firmware, NTIO,
BOP, DOS or OpenNT branch.

## Selected profile input

The finite fixture supplies the complete currently evidenced r18 CPU5 profile
snapshot, in one action:

| Range | Bytes |
| --- | --- |
| `0x40..0x6F` | 48 recorded IVT bytes |
| `0x496` | `10` |
| `0x714..0x717` | `00 00 00 C0` |

`0x413..0x414` is intentionally not supplied: its future `12h` use has no
recorded value in the selected snapshot and has not been reached. This is a
deferred profile capability, not a zero/default synthesized by mantle.

## Verification

The S40 recipe builds the adapter record, its focused request fixture, the
changed mantle finite runner, NTIO preparation and full CPU5 x64 `/MT` finite
fixture. The focused fixture proves valid construction plus rejection of
identity mismatch, payload-length mismatch and overlapping payload ranges.
The existing mechanical-action boundary test confirms ordered preflight before
copy and selector-blind mechanics.

The exact native run exits `0` and still source-builds/reaches the prior BOP
family. Its first generic non-BOP stop has changed from S39's `0000:0A84` to:

```text
fault=0000:0047 vector=6 window=15:f041f800
history-tail predecessor=0000:0043 seq=a771 fault=0000:0047 seq=a772
cs-transition previous=9346:4d53 seq=a74d current=0000:0000 seq=a74f
```

Thus installing the real selected startup input removes the old all-`FF`
low-RAM frontier. The remaining `9346:4D53 -> 0000:0000` transfer persists,
and the resulting `0000:0047` bytes are part of the recorded IVT slice rather
than a BOP or a device operation.

## Disposition

S40 closes the complete currently evidenced pre-entry input contract. It does
not prove why control transfers to `0000:0000`; no additional seed, BIOS
emulation, device or BOP implementation is authorized by this result. The
next task must map the predecessor's source/stack/transfer mechanism and
separate an incorrect CPU/exception result from a missing original machine
precondition.
