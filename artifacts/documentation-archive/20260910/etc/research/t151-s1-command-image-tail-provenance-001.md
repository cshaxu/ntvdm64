# T151 S1 command-image tail provenance 001

## Question

Does the retained source-built `COMMAND.COM` supply a byte at the T149 fault
position `CS:IP = 0BC1:FFFF`, and can a retained map bind that position to
original COMMAND source?

## Controlled image identity

The source-build manifest names a 50,384-byte `COMMAND.COM` with SHA-256
`908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`.
The T149 staged input
`artifacts/analysis/t149-s2-10m-prefetch-observation-001/byob/COMMAND.COM`
has the same length and hash. Therefore the address arithmetic below concerns
the exact file supplied to the immutable T149 profile, rather than merely a
similar build output.

Original NTDOS COM loading (T144) uses `Exec_DMA:0100` as the first file byte.
For the observed `Exec_DMA = 0BC1h`:

| Quantity | Value |
| --- | --- |
| Fault physical address, `0BC1h * 16 + FFFFh` | `0x1BC0F` |
| File offset corresponding to `CS:FFFF` (`FFFFh - 0100h`) | `0xFEFF` |
| Controlled file length | `0xC4D0` |
| Inclusive last source-file-covered IP | `0xC5CF` |
| Difference from `FEFF` to the exclusive file end | `0x3A2F` bytes |

Thus no byte at `0BC1:FFFF` can be attributed to this `COMMAND.COM` file. It
is outside the whole source-built file interval; the question is not a missing
symbol for a byte that exists in the file.

## Map/listing inventory

The current retained build tree does contain a co-located `command.map`
(48,079 bytes, SHA-256
`7B5747267BC3F323844A81CD48ACBA5BF9CD62F1538B414F7E35AC0F1D516C53`) and
`command.lnk`. The map's final non-empty layout range ends at `0xC5CF`, with
zero-length tail markers at `0xC5D0`; it corroborates the raw-file range.
The manifest does not name the map as an output, so this audit does not use it
to make a stronger same-invocation source-label claim. In any event no map
entry could bind `FEFFh`, because it lies beyond the image extent.

This current inventory supersedes only the *availability premise* used in
T145: a map sidecar is now retained. It does not invalidate T145's core rule
against heuristic source attribution, and it does not provide a label for the
out-of-image tail.

## Disposition

The byte/control state at `0BC1:FFFF` belongs to original NTDOS allocation,
read/failure and guest execution after the COM file ends. It is not a
COMMAND-source mapping problem, a Bochs instruction defect, or an adapter
BOP-route implementation candidate. The next static audit must correlate the
observed `50:42` fast-read path (and absence of `50:16`) with original
`ExecRead`/DOS handle selection and the current original-unavailable provider
result before any patch or new observation is proposed.
