# M0 T410 Console input, display and resource closure

## Outcome

T410 closes the native-Console restoration package. It removes the independent
project window route and restores/reconciles the selected OpenNT input, text
and resource boundaries without recreating Console Server or a GUI fallback.

| Row | Disposition | Evidence |
| --- | --- | --- |
| D09 | Restored pointer-display count as distinct from the text cursor. | S2 P1 and the indexed evidence. |
| D23 | Removed the independent app window, injection and stale wiring. | S1 evidence. |
| D24 | Restored source-shaped character conversion, AltGr/OEM handling and surrogate rejection. | S2 P4 evidence. |
| D25 | Moved immutable embedded-ROM product mechanics to the firmware adapter; preserved original resource order. | S2 P5 evidence. |
| D28/D29 | Restored VDM returned-input prepend, NOWAIT/NOREMOVE and Alt+Enter pair consumption. | S2 P2/P3 evidence. |
| D30 | Explicit unavailable boundary: OpenNT Console Server activates `hklActive`; public modern Console has no equivalent active-layout operation. The adapter uses the documented worker-thread layout fallback and makes no ownership-equivalence claim. | S2 evidence, D30 section. |
| D34 | Restored original private graphics-buffer call shape and removed the unpresented DIB/palette substitute. Public Console returns unsupported rather than fabricating palette success. | S3 evidence. |

## Verification

The focused fixture creates/opens a real conhost input endpoint when invoked
from an inherited pipe and passes end-to-end (`PASS`, exit `0`): returned-input
ordering, NOWAIT/NOREMOVE, reserved Alt+Enter pairing, pointer/text-cursor
separation, text-plane `OK` rectangle readback, and the explicit palette
failure contract. The formal Win32/x86 CCPU40 worker relinked after the final
two-byte x86 VDM text-cell correction. Documentation governance and
`git diff --check` passed for each delivery.

The retained limitation is D30 only; it is a missing modern public Console
capability, not a reason to reintroduce a private Console Server, GUI fallback
or unrelated broker. Subsequent CCPU40 work remains in the queue and does not
reopen T410's Console owner boundary.
