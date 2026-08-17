# T225 S29 native first-fault source/layout attribution

## Question and scope

S28 captured the first native non-UD exception, before normal Bochs exception
delivery, as `#GP(13), error 0, CS:EIP = 0C41:2357` in real execution mode.
S29 determines what can be attributed from the immutable source-built inputs,
the current startup/load contracts, original OpenNT sources, and the adopted
Bochs access path.  It does not add an observation, modify guest state, repair
a descriptor, or implement a BOP/provider.

## Immutable inputs and address arithmetic

The S10 primary root remains the sole input set for this attribution:

| Image | Bytes | SHA-256 |
| --- | ---: | --- |
| `NTIO.SYS` | 33,792 (`0x8400`) | `CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937` |
| `NTDOS.SYS` | 27,858 (`0x6CD2`) | `957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84` |
| `COMMAND.COM` | 50,384 | `908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43` |
| `TARGET.EXE` | 882 | `69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC` |

The current startup contract in
`src/bx-vdm/bx_ntvdm_startup_session.c:82-88` and
`src/bx-vdm/bx_ntvdm_composition_runtime_v1.c:355-363` starts NTIO at real
`CS=0070`, writes it at physical `0x0700`, and therefore occupies only
`[0x0700, 0x8AFF]`.  The S28 fault address is:

```text
physical = (0x0C41 << 4) + 0x2357 = 0xE767
```

It is outside the initial NTIO range.  Thus it cannot be attributed to the
initial NTIO placement or to COMMAND/TARGET staging.

NTDOS is deliberately not an initial preload.  The bound 50:11 path in
`src/bx-vdm/bx_ntvdm_boot_namespace_plane_v1.c:111-114` invokes
`bx_ntvdm_dem_load_dos_service_v1_prepare`; that source-derived service
(`src/bx-vdm/bx_ntvdm_dem_load_dos_service.c:17-42`) copies the exact NTDOS
image to `((uint16_t)EDI << 4)` and resumes after the four-byte BOP.

If the real 50:11 request used `EDI=0x0C41`, the exact NTDOS interval would be
`[0xC410, 0x130E1]`; `0xE767` lies in it and `0x2357 < 0x6CD2`.  This is a
strong *compatibility* result, not proof of the load segment: S28 intentionally
did not capture EDI or the 50:11 transaction.  What is proven is that the
fault lies beyond initial NTIO, while the observed address is compatible with
executing the source-built NTDOS image after its formal load transaction.

## Guest-image and original-source attribution

Disassembling the immutable NTDOS bytes at file offset `0x2357` gives the
following local window:

```text
00002347  2E8E065722  mov es,word [cs:0x2257]
0000234C  26C43EA005  les di,word [es:0x5a0]
00002351  E8AAEE      call 0x11fe
00002354  7503        jnz 0x2359
00002356  E82D07      call 0x2a86
00002359  26FF05      inc word [es:di]
0000235C  C3          ret
```

The recorded EIP is inside the encoding of the `call 0x2A86`, rather than an
independently observed instruction boundary.  It must not be relabeled as the
callee, an effective operand, or a source routine from that fact alone.

The original-toolchain `NTDOS.MAP` under
`build/M0-T225-S8/ntdos-primary-source-build-r1/.../dos/NTDOS.MAP` contains
only a DOSCODE range `00000..07938` and DOSDATA range `07940..08F21`; it has
no code symbol-to-offset listing.  The companion `msdos.lnk` supplies only
object order.  Although the original source tree supplies relevant DOSCODE
modules such as `mscode.asm`, neither document proves an exact module/function
for file offset `0x2357`.  S29 consequently attributes the reached plane to
the original guest NTDOS DOSCODE image, while retaining exact source-function
ownership as unresolved.

This is consistent with, but independent from, the earlier retained NTDOS
control-flow evidence: the S43 DEVIOCALL2 record establishes that source-built
NTDOS lifecycle and temporary/DOSDATA placement must not be synthesized by a
fixture or adapter.  It does not identify this later fault.

## Adopted Bochs access-path attribution

Both S28 stderr records end in the original adopted Bochs message
`read_virtual_word_32(): segment limit violation`.  In
`src/bx-core/cpu/access32.cc:493-545`, `read_virtual_word_32(s, offset)`
checks the selected segment's cached scaled limit and then invokes
`read_virtual_checks(seg, offset, 2)`; either rejected access reaches
`exception(int_number(s), 0)`.  The 16-bit `MOV AX, moffs` implementation in
`src/bx-core/cpu/data_xfer16.cc:145-150` is one caller of that helper.

The `_32` helper name is not evidence of protected-mode execution:
`src/bx-core/cpu/cpu.h:3732-3735` dispatches all non-long-64 word reads to it.
S28's execution-mode field is explicitly real mode.

The first-fault event deliberately contains no segment-register index,
effective access offset, cached limit, access-class result, instruction bytes,
or guest-memory copy.  Therefore the present evidence proves only that a
two-byte virtual-word read was rejected before normal exception delivery.  It
does not distinguish:

1. a guest NTDOS/startup path that produced invalid real-mode segment state;
2. an incorrect Bochs segment cache/access calculation; or
3. a later exception-delivery/control-flow problem.

## Rejected owner hypotheses

| Hypothesis | Disposition | Reason |
| --- | --- | --- |
| Direct versus Readonly host mutation policy | Rejected | The S28 typed record and stdout/stderr hashes are byte-identical in both modes. |
| Initial NTIO placement | Rejected | `0xE767` is outside `[0x0700, 0x8AFF]`. |
| A missing BOP/provider or adapter service | Rejected | The record supplies no selector/service and this access path is wholly CPU mechanical. |
| Exact NTDOS source module/function | Unresolved | The retained original map has no code symbol mapping for `0x2357`. |
| Guest-state versus Bochs-access mechanics | Unresolved | S28 lacks the access tuple needed to discriminate them. |

## Smallest successor and boundary

The smallest next owner is a **selector-blind bx-core segment-access fact
diagnostic**, exposed only through the existing default-off mantle finite-stop
path.  It must record, before `exception()`, the access kind, segment-register
index/selector, effective offset, requested width, scaled limit, and which
original check rejected the read.  It must not decode BOP, read or copy guest
memory, identify OpenNT/DOS/SoftPC, alter CPU state, or change unarmed Bochs
behavior.

No original OpenNT host provider can expose this Bochs-internal cache fact,
and bx-vdm cannot reach it without breaking the core boundary.  Accordingly a
future default-off diagnostic requires a separately registered Bochs intrusion
exception and a separately admitted S task.  S29 itself admits no source or
runtime change and makes no repair claim.

## Confidence

High: the failure is post-initial-NTIO and profile-neutral, and the adopted
Bochs source establishes its mechanical two-byte access path.  Medium: the
address is compatible with a formal NTDOS load.  Intentionally unresolved:
the actual loaded segment, the access tuple, the exact guest source routine,
and whether guest state or core access mechanics is wrong.
