# T228 S1 P20 — Selector-Blind Software-Interrupt Observation Profile Admission

P19 leaves one whole guest-owned question: which COMMAND/NTDOS transient/MCB
stage first diverges. The historical images already execute that behavior and
cannot link into the MSVC x64 runtime. Existing `bx-vdm` interrupt work is
BOP `02h/06h` machinery, while upstream `soft_int.cc` has only an optional
`INT 21h/AH=4Ch` text log. Neither is a usable neutral source.

The first usable recovery rung is therefore a registered external mechanical
exception, `BX-CORE-088`. It records every executed real/V86 *software
interrupt* with no semantic branch on the vector or raw register values. The
record includes only fixed-width pre-delivery CPU values that already exist at
the original `INT_Ib` boundary. Mantle owns an opt-in bounded ring and copied
query; default builds compile no active hook.

The profile is deliberately complete rather than an INT 21 leaf patch:
software interrupt execution, real/V86 admission, protected-mode decline,
ring reset/wrap and default-off behavior are all one unit. It copies no guest
memory and does not decode an instruction beyond the existing `INT_Ib`
handler. It may not observe `INT3` or `INTO` unless the full profile source
review proves those are the same original delivery boundary; they begin as
explicit negative coverage.

This profile observes mechanics only. A later finite trace may correlate its
raw finite records to P19's source map, but cannot use one vector/function
record to select an allocator or BOP patch.
