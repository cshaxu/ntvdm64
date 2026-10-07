# T436 S2 Windows 3.1 mouse-driver boundary

Owner accepts S1's unstable enhanced execution and admits the independent
mouse driver, followed by S3 installer patches. This record covers S2 only;
startup-only research is not resumed and no new runtime capability is claimed.

## Initial ABI audit

Input O:/win31/SYSTEM/MOUSE.DRV, Mouse8.20 SHA256
B4F0B9DE8043071FA2B82B16373BF5188BBE8162C3EF24C8C3D1B01F86B96B07.
`tests/observation/audit-win31-mouse.py --driver <input> --output
build/M0-T436/S2/r001-abi/retail-mouse.json` reads movable/fixed NE export
bundles, segments, imports and names. It does not write the input, copy code
or prove execution. NE expected Windows version030A, automatic data segment2,
exports Inquire1/Enable2/Disable3/MouseGetIntVect4, GetSetMouseData5,
CplApplet6/PowerEventProc7 and named WEP8; original imports KERNEL/SYSTEM.
The Win101 audit intentionally rejects movable bundles and is not weakened.

## Provenance and design choices

OpenNT WOW mouse.asm/def retain public entry shapes but implement WOW thunks;
they cannot serve as a real NT DOS guest driver. Original DPMI dxintr.asm
PMIntrMouse converts protected function12 callbacks to a real-mode reflector,
but function20 exchange is explicitly not translated. Do not assume its raw
returned address is a protected selector. The selected independent driver
therefore uses public DPMI0300/0303/0304 for raw INT33 and callback ownership;
retail DOSX/VMM support remains a runtime gate, not inferred from OpenNT source.

Win101's authored endpoint normalization and reversible callback/pointer
ownership are reusable concepts. Its real-mode NE callback and code-segment
mutable state are not reusable blindly in protected Win3.1. Keep mutable state
in driver data, preserve the original INT33 provider, and avoid a second INT71
consumer or hardware-vector/PIC programming. No host/mirror/protocol changes.

Microsoft [DDK adaptation guide](https://www.pcjs.org/documents/books/mspl13/win/w3ddkadp/)
defines the fourteen-byte MOUSEINFO and the driver entry/event contract.
Microsoft [Q74572](https://jeffpar.github.io/kbarchive/kb/074/Q74572/) explains
that the default Windows mouse driver does not consume the installed DOS
driver automatically, and VMD arbitrates hardware rather than acting as the
Windows driver. Both are primary Microsoft texts preserved by archives.

## Remaining gates

Implementation/build and exact NE/export/relocation checks; callback allocation,
real/protected stack return, old callback restoration and failure rollback;
normal/missing-provider/repeated-enable/disable tests; actual standard-mode
arrow and click/show-hide/exit/restart; enhanced-mode interaction when it starts.
Inherited enhanced startup failures remain distinct and never count as mouse
passes. S3 is not admitted; final installer files are not yet delivered.
