# M0 T388 closure — Fixed interactive runtime matrix and corrected shell boundary

## Closed scope

T388 completed the fixed-container audit of the current CPU40/x86 SoftPC
product's Console, keyboard, display and PIF-facing startup boundary. S1--S6
provided the source-shaped Console-first, keyboard-route, display-arbitration
and PIF dispositions. S7 then tested the only remaining premise: that the
first permanent `COMMAND.COM` started without an explicit target should reach
a guest DOS-CON command prompt in the retained fixed container.

That premise is false in the selected original COMMAND source. The enabled
first-shell `do_again` path uses `CMDSVC SVC_CMDGETNEXTCMD` (`54:01`) to obtain
the next Base VDM command record. Its direct DOSKey/console line-input route
is compiled out. `BaseSrvGetNextVDMCommand` brokers records and wait handles;
it is not a Console-line reader.

The original DOS-CON prompt path exists, but under a different lifecycle: an
explicitly declared, non-first child `COMMAND.COM` has `SCS_FIRSTCOM == 0`,
enters `DoReEnter`, and—subject to the original `SCS_CMDPROMPT` disposition—
reaches `Do16BitPrompt -> PRINT_PROMPT -> INT 21h/AH=0Ah`. That child is the
correct target for a future `ntvdm.exe command.com` interactive profile.

## Evidence and result

- The fixed `r80` and `r81` observations reached original COMMAND `54:01`,
  `54:0E` and `54:04`, observed timer service, and did not reach the
  `keyboard_io(AH=2)` wait trigger. This excludes a keyboard, PIC/EOI or
  Console-transport repair as the explanation for first-shell behavior.
- Earlier real-Console key-delivery observations still prove the separate
  SoftPC Console event-worker -> keyboard controller -> IRQ1 route. They do
  not create or prove a Base VDM command record.
- The inherited-environment transient-layout failure remains recorded as an
  independent COMMAND/NTDOS arena-and-environment issue. It is not repaired
  or reclassified as a prompt/input failure in this closure.

T388 therefore does **not** claim a visible prompt, `ver`, `dir`, editing,
Ctrl+C, `exit`, `EDIT.COM`, interactive COMMAND, or a general direct-shell
UX. Those rows are not silently skipped: they are explicitly inapplicable to
the invalid first-shell premise and transfer to the next profile.

## Transfer and exclusions

The queue's first candidate, [explicit interactive COMMAND profile](../etc/operations/proposal-explicit-interactive-command-profile-001.md), owns the
second-`COMMAND.COM` record, original copyright/header, DOS-CON prompt,
guest-owned keyboard, Ctrl+C, `exit`, parent return, Console/window/PIF
disposition and fixed-container proof.

T388 makes no guest-media change and adds no host prompt parser, synthetic
BOP record, direct guest-memory input write, private BaseSrv/CSRSS transport,
WOW expansion or `cmdExec32` work. It closes with the source boundary
identified and the successor proposal ready for separate admission.

## Closure checks

- Reviewed the active packet's stated matrix rows against retained source and
  fixed-container evidence.
- Reviewed this closure, Status, Queue and successor proposal for ownership
  separation.
- Whitespace checks passed. The generated documentation inventory must be
  refreshed in the next documentation delivery: its authoritative output file
  was held by another local process during this closure, so this record does
  not claim that refresh passed.
