# M0 T423 S14 product-experience evidence

S14 is admitted and remains open for owner-side validation of the exact
`COMMAND -> Window -> CMD -> modern EDIT` interaction and Explorer drag/drop.
This record distinguishes demonstrated component behavior from that final
interactive acceptance.

## Root cause and implementation

The modern Microsoft Edit Windows backend enables VT input and reads character
records, while the previous NTCON path forwarded only native `MOUSE_EVENT`
records to its hidden Console. NTCON's worker-owned square therefore moved but
Edit did not receive a menu click. A real modern Edit process is the negative
control: raw mouse down/up leaves its menu closed. Sending the same click via
production `ntcon_input_write` converts it to SGR VT mouse key records and opens
the File menu. Ordinary native Console mouse-input clients retain raw records.
This is a generic Console input-mode boundary, not an Edit executable special
case. No guest, original mirror, shared library, or protocol was changed.

NTKVM registers Ctrl+Alt+M as a Window-only mouse-release hotkey. Its input
callback queues an epoch-scoped request; the frontend polling owner invokes
the existing `kvm_window_release_mouse` operation. The Window stays open, and
no guest key is forwarded. Existing Ctrl+Alt+F display switching is unchanged.

## Reproducible evidence

- Formal incremental x86 build: `build/M0-T423/S1/restart-formal-x86`, including
  `ntcon.exe`, `ntkvm.exe`, `run16.exe`, and the frontend controller test.
- NTCON state fixture: `O:/winnt/Logs2/t423-s14-ntcon-state-r2.txt`, PASS with
  classic mouse-pair and VT input checks.
- Real modern Edit hidden-Console fixture:
  `O:/winnt/Logs2/t423-s14-modern-edit-mouse-r1.txt`, negative raw-event
  control and positive File-menu text (`New File`) after production conversion.
- Private-desktop frontend controller fixture:
  `build/M0-T423/S14/frontend-controller-r1.txt.console.txt`, verifies that
  Ctrl+Alt+M releases an actual capture while keeping Window display visible.
- Isolated candidate package at `build/M0-T423/S14/package-r1`: 17/17 Console
  cases (`t423-s14-candidate-r2-console-*`) and 17/17 Window cases
  (`t423-s14-candidate-r3-window-*`) under `O:/winnt/Logs2`.
- Candidate GUI/exit probes:
  `build/M0-T423/S14/lifecycle-{cmd,notepad,winmine}-q-r1.txt`; direct CMD
  then `exit`, Notepad, and WINMINE return without retaining a product Console
  member on the private desktop. Candidate WOW frontiers
  (`t423-s14-candidate-r5-wow-*`) preserve the WINMINE main window and the
  existing distinct SOL/WRITE out-of-memory modals; those two applications
  are not claimed functional.
- Published `O:/winnt` regression: 17/17 Console
  (`t423-s14-published-r1-console-*`), 17/17 Window
  (`t423-s14-published-r2-window-*`), and WINMINE/SOL/WRITE frontiers
  (`t423-s14-published-r3-wow-*`) under `O:/winnt/Logs2`.

The coherent eight-file publication was backed up at
`build/M0-T423/S14/publication-backup-r1`. Only `ntcon.exe` and `ntkvm.exe`
have changed production bytes relative to S13; the other six were republished
unchanged to preserve package coherence. Published SHA-256:

| File | SHA-256 |
| --- | --- |
| run16.exe | 160FDEA98A37600DD62F61D117226D4996E8FCBE9AC8C36D2CD35F6A12DB0E57 |
| ntsrv.exe | 552FEEEA2115221B7C39461AD4F6E7B91249D032AF893CACE712F62DA02A48EF |
| ntvdm.exe | 2DFB1104700B2A724E938F90030D0D8FFA02BCEB57AC66F29584E20288C9244A |
| ntkvm.exe | FC5152BC47251156BF9B928DFEFFBE08FF084D63A9067B116334ADC207CB6F5F |
| ntcon.exe | 90D4D59AC29C3FBE88050CD8B700089877CE8706BEC6A86A8CA69E9A6434C218 |
| ntmon.exe | 589936026400EF4C4C6E5D212E53C5B0389DB42F8843238E392E5CBF78C2EBA3 |
| wow32.dll | 0D2AE60264B03A8040D98AA86BCF80455127064084E2217D318D5E13F90FA94A |
| VDMREDIR.dll | 79698D12E90DC8F8ACF8FED250DC0A2C9BE57711C03E4DF44A0112549E023500 |

## Remaining S14 acceptance

- Owner should test the exact visible `run16 command` -> Ctrl+Alt+F -> `cmd`
  -> modern `edit.exe` -> mouse-click File menu chain. The focused real-process
  fixture plus Window route regression prove the pieces, not this complete
  interactive sequence.
- Actual Explorer drag/drop Console retirement is not yet reproduced on the
  physical desktop; the private-desktop direct GUI and interactive exit cases
  are not a substitute for that path.
- No physical-desktop focus or pointer-grab test was run; owner had asked not
  to interrupt desktop work. The private-desktop capture/release test is
  affirmative but bounded evidence.
