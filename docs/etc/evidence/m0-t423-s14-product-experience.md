# M0 T423 S14 product-experience evidence

S14 was accepted by the owner on 2026-09-29 after the tested and published P1
`631206f9e`; the owner explicitly directed closure and a wait for further
instruction. This record distinguishes automated evidence from owner-reported
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

## Owner acceptance and evidence limits

- The owner reported verification passed and directed S14 closure; no detailed
  screenshot, transcript or individual Explorer drag/drop result was supplied.
  Therefore the owner-side verification is recorded as an acceptance decision,
  not relabeled as an independently captured automated witness for every
  physical-desktop path.
- Automated tests did not drive the full visible `run16 command` ->
  Ctrl+Alt+F -> `cmd` -> modern `edit.exe` -> File click sequence in one run.
  They independently exercised the real Edit menu input boundary and the
  Console/Window routes. Actual Explorer drag/drop and physical-desktop focus
  were likewise not captured by automation.
- T423 remains open. S14 closure does not claim SOL/WRITE functionality beyond
  the retained baseline frontiers.
