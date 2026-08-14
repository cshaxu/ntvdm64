# M0 T198 S82: SpcKbd Native-INT10 Provenance Audit

## Question

What owns the observed `073B:0939 -> C000:014A` transfer, and does it arise
from a guest image/control-flow error or a dynamic historical display-vector
write?

## Inputs And Procedure

- The source-built NTIO image declaration: 33,792 bytes at physical `0x700`.
- Its locked historical build manifest and retained `ntio.map`.
- The S74/S76 trace: predecessor `073B:0939`, bytes
  `EA 4A 01 00 C0`, target `C000:014A`.
- A read-only extraction of the generated NTIO declaration at physical
  `0x7CE9` (`073B:0939`) and a scan for the exact pre-run/observed byte forms.
- OpenNT `spckbd.asm` and `sysinit1.asm` copy/install flow.

## Observations

The historical map identifies `SpcKbdBeg` at `009D:3790`,
`int10h_vector` at `009D:402D`, and `SpcKbdEnd` at `009D:4452`. The runtime
segment `073B` is therefore within the source-built, copied resident SpcKbd
region; `073B:0939` lies after its public INT10 vector entry and is consistent
with the source's local `jmp_native` far-jump site.

The static generated NTIO bytes at the traced physical address are:

```text
... 9D EA 00 00 00 00 E9 E6 FE ...
```

The exact runtime predecessor is instead:

```text
EA 4A 01 00 C0
```

The static source-built image contains no `EA 4A 01 00 C0` sequence. Hence the
four operand bytes were dynamically changed from the original `host_int10 dd
?` placeholder before the trace reached the far jump. `spckbd.asm` describes
this form exactly: the fullscreen branch uses `db 0EAh` followed by the
mutable `host_int10` far pointer.

`InstSpcKbd` contains the known historical write path: after its BOP-`5F`
continuation reaches `isk_int9`, it reads IVT 10h (`0000:0040`) and stores the
two words at `host_int10`. The resulting C000 target is consequently a
historical guest-vector handoff, not an unexplained CPU transfer.

## Limit

S81 proves that the current observable bridge did not record canonical BOP
`5F`. S83 supersedes the pre-bind uncertainty: the pointer is still zero at
the first bound canonical ingress. The remaining question is whether `5F` was
consumed before the passive post-composition observer or an alternate source
continuation performed the same IVT write. No retained symbol listing names
the local `host_int10` offset, so the correlation is source/range/byte based,
not a fabricated symbol address.

## Disposition

The required next boundary is a lifecycle-observation question, not a ROM,
Bochs-VGA, BOP-provider, or CLI-mode implementation task. A future observer
must establish when the four `host_int10` operand bytes change relative to
bridge binding and canonical BOP ingress, without modifying guest memory.

## Confidence

High confidence: `073B:0939` is a dynamically patched SpcKbd native-INT10
far jump and C000 is its source-owned target. Medium confidence: the historical
`isk_int9` path is the writer; its execution is not directly observed in the
current bridge lifetime.
