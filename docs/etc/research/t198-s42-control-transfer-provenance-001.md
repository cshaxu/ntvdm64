# T198 S42 control-transfer provenance witness

## Scope

S42 is a default-off, mantle-only observation.  On an already-recorded
code-segment change it copies a fixed 15-byte ordinary-RAM window at the
previous real-mode position and a fixed 10-byte window at the current
real-mode `SS:SP`.  It does not select an address, decode an instruction,
recognize BOP, alter CPU state, write guest memory, or add an adapter ABI.

The change is registered as `BX-MANTLE-075` before implementation in
[`adapter-external-intrusion-exceptions.md`](adapter-external-intrusion-exceptions.md).

## Build and run

The CPU5/Pentium-MMX source membership was rebuilt under MSVC x64 `/MT` with
`BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY=1`.  The historical relink-only
recipe first stops at its known absent S40 pre-entry object; that is not a
CPU closure failure.  The S40 recipe then compiles that object, links the
same rebuilt history object, and runs the native NTIO fixture successfully:

```text
t198-s23 status=4 ... observed-first-generic-ud=1 fault=0000:0047 ... observed-stop=1 ... history=8
t198-s23 history-tail count=8 predecessor=0000:0043 seq=a771 fault=0000:0047 seq=a772
t198-s23 cs-transition valid=1 previous=9346:4d53 ss-sp=00a7:0918 seq=a74d current=0000:0000 ss-sp=00a7:090e seq=a74f
t198-s23 cs-provenance valid=1 predecessor-valid=1 stack-valid=1 code=36ff1e7a038b440836a37a0336ff1e stack=ffff1fbf4600584d4693
```

The fixture also rejects a disabled history and reports no transition or
provenance record before a transition exists.  The ordinary-RAM copy helper
fails closed for a real-mode window which crosses `0x100000`.

## Interpretation

The first five predecessor bytes are `36 FF 1E 7A 03`: an SS-segment-override
far indirect call through `SS:[037A]`.  Its next address is `9346:4D58`, and
the copied stack includes the little-endian far return pair `4D58:9346`.
Thus the observed `9346:4D53 -> 0000:0000` transition is an ordinary guest
far call whose current pointer value resolves to `0000:0000`; it is not a
BOP dispatch and does not by itself establish a missing Bochs device or
adapter service.

The remaining stack words establish a ten-byte observed position delta, but
S42 does not infer their owner.  The next task must identify the actual
`SS:[037A]` table value and the retained-image/loader owner before changing
any machine facility.

## Boundary scan

The S42 implementation has no selector, service, BOP, DOS, OpenNT, NTIO,
firmware, device, profile, callback, pointer-transfer, guest-write, or CPU
mutation path.  It stores copied fixed-width bytes only in mantle-private
state and exposes them only to the fixture bridge.
