# M0 T198 S86: Non-Invasive CLI Stream Profile Design

## Decision

The first CLI profile's only admitted display state is **stream-I/O**
(`STREAM_IO = 2`). This is an explicit profile declaration, not an implicit
BOP-`5F` fallback and not an emulation of a Windows console. It selects the
existing guest-resident `spckbd.asm` stream branch before native INT10 or
SoftPC video BOP routing.

The profile does not implement text output yet. It only defines the minimum
state publication that prevents a CLI run from falsely selecting the historical
fullscreen native-ROM path. A later output sink must retain the original
buffer/flush contract rather than treating every INT10 call as a host print.

## Source Basis

`spckbd.asm` defines `STREAM_IO = 2` and routes `int10h_vector` to
`sw_video_io` when `use_host_int10` equals that value. The routine handles the
well-behaved text forms `AH=0Eh` and `AX=13FFh` in guest memory, with a bounded
buffer and an explicit fallback BOP `42h` when the buffer cannot accept a
request.

Historical NT already has a stream state: `host_enable_stream_io()` records
`sc.ScreenState = STREAM_IO`; reset recognizes `host_stream_io_enabled` and
uses that state. Its implementation otherwise depends on old console/event
machinery and is not imported.

## Future Capability Boundary

At the verified `AX=BEEF`, real-mode `C4 C4 5F` handoff, `spckbd.asm` has
placed the `kio_table` base in `DS:SI`. The original `kb_setup_vectors()`
reads the table's word at index 17: the offset of the one-byte
`use_host_int10` state. Therefore S87 may use exactly one synchronous,
adapter-owned transaction:

```text
copied BOP state (DS:SI, real mode, AX=BEEF)
  -> checked 2-byte ordinary-RAM read at physical(DS:SI + 34)
  -> validate the returned offset in the same DS segment/aperture
  -> checked 1-byte ordinary-RAM write of value 2
  -> existing CF=1, RIP+3 continuation
```

The transaction is adapter-owned source-derived composition. Bochs receives
only its existing typed read/write requests. The CLI supplies one fixed
profile enum, never a raw guest address; the adapter derives the address from
the copied guest table. The write must be rejected atomically if either range
is invalid, the event is not the exact BEEF form, or the profile has not
declared stream mode.

## Explicit Exclusions

| Profile/value | Disposition |
| --- | --- |
| `STREAM_IO=2` | First CLI profile; design admitted, implementation S87. |
| `WINDOWED=0` | Rejected: needs historical SoftPC/VGA BOP plus GUI/regen composition. |
| `FULLSCREEN=1` | Rejected: needs executable mantle-owned ROM/INT10 and device/PIC lifecycle. |
| No display declaration | Reject pre-entry/profile composition before BOP-`5F`; do not leave the static fullscreen byte active. |

There is no new user-facing `--display` switch in S86. The immutable first
CLI profile declares stream mode by construction; future profiles may expose
a versioned option only after the profile schema, negative cases, and output
sink are implemented together.

## Output Sink Boundary

The stream state is **not** permission to print `AL` from every INT10 to host
stdout. Original guest code batches bytes in `sw_video_io`, preserves
busy/dirty counters, supports `AX=13FFh`, and delegates overflow/non-text
forms through BOP `42h`. A future CLI output capability needs a fixed copied
buffer/acknowledgement ABI, encoding policy, stdout failure behavior and a
separate fallback disposition. Until then, the state publication is a
continuity prerequisite only; visible terminal output remains unclaimed.

## Result

S86 establishes the profile decision and the exact prospective write contract
without executing it. S87 may implement and test only the two-range BOP-`5F`
stream-state transaction. It must not add a text sink, BOP `42`, GUI/VGA/ROM,
PIC behavior, or a general display setting.
