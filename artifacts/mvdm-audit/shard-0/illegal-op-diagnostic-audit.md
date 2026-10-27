# Illegal-op frame observations: source-linked comparison

Pinned OpenNT `illegalp.c` reads four bytes from SS:ESP to reconstruct the
faulting IP and CS, calls its existing formatter/panel, updates the return
frame and dispatches the unexpected interrupt. Current source adds a
DIV-328 report whose arguments unconditionally repeat two of those byte reads
and read two additional stack offsets before the reporter can check its
default-off setting. Those original four frame reads are functional and must
not be removed with the diagnostic observations.

`illegal-op-source-fixture.py` checks both frozen source hashes, extracts
the real `illegal_op_int` bodies, and compiles each with NTVDM/PROD and a
controlled MONITOR branch. Its stub formatter does not read instruction
bytes, allowing the frame accesses to be counted separately. With a normal
returning panel and disabled-report stub, original reports four reads and
current eight; both write two frame bytes, call the panel once, dispatch once
and advance IP from 1234 to 1235. This proves the extra source-body accesses
in the modeled branch. It does not execute the real formatter, the actual
CPU/memory handlers, enabled reporting, instruction faults, IP wrap or a
deployed guest; it does not establish a live crash.

Later diagnostic-access recovery should preserve original frame extraction,
panel and continuation, eliminate or safely gate only the extra samples, and
test enabled mode and exception ordering separately. DIV-328's previous
statement that only already-live stack values were copied was inaccurate;
the registry now records the actual memory accesses. The mirror file remains
partial, and no product or existing guest was changed in this audit.
