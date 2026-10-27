# Guest source, image and observed capability reconciliation — shard 3

## Scope and evidence

The frozen 1,168-row shard contains 656 paths labelled immutable guest, guest
media or guest-build input. They include 163 historical `.obj` files, source,
headers, messages, tools, binaries and debug/retail build artifacts. The label
establishes an inventory role only; it does not select a host provider, prove
the source generated a shipped executable, or prove guest behavior. Current
constraints prohibit modifying or rebuilding guest images.

`assets/winnt.zip` SHA-256 matches `assets/winnt-guest-manifest.json`
`archive_sha256` (`7a0534dec9940b2d8366c8ef71dd536ed94be7817d7c4e1e19bed05f6e60b8f7`).
The current mirror file hashes were compared with every manifest member by
exact SHA-256, without guessing relationships by name. Twelve shard rows match
eleven distinct `WINNT/SYSTEM32` files: COMMAND.COM, KB16.COM, HIMEM.SYS,
KEYBOARD.SYS, KEYBOARD.DRV, MOUSE.DRV (two matching mirror paths), GDI.EXE,
SHELL.DLL, SYSTEM.DRV, USER.EXE and WFWNET.DRV. Their installed
`O:/winnt/system32` files match the manifest hashes as of this read-only
check. Each exact match and installed file is in that row's CSV columns.
This proves byte identity only, not that every function was executed.

COMMAND.COM's installed/manifest/mirror SHA-256 is
`908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`.
Installed DOSX.EXE, KRNL386.EXE and WOWEXEC.EXE also match their manifest
hashes, but the corresponding binary rows are not necessarily in this shard.
The ZIP contains MMSYSTEM.DLL and TOOLHELP.DLL with matching manifest hashes;
they were not present at `O:/winnt/system32` during this check. This is an
installation-location observation, not a claim of missing product capability
or of failure of other staging/search roots. T470's real ToolHelp query probe
used a separately staged guest copy and recorded the negative result below.

## Functional groups and limits

Every guest input row now has a `guest_function_group`, group observation,
precise hash-match fields and an explicit boundary. Group-level evidence is
context, **not evidence that each constituent source file was compiled or
invoked**. Particularly:

- DOS COMMAND/utility: S9 r10 Console17/Window17 and T420 S21 cover direct
  and nested MEM, COMMAND and selected native child flows. Other DOS commands,
  error/exit combinations and source-to-binary correspondence remain open.
- Win16 USER/GDI/kernel and WRITE: S9 r10 exercised selected real Win16 DC
  and window operations, WINMINE and SOL interactions, and WRITE
  close/restart/close. That does not prove the complete guest libraries or
  WRITE edit/save/printing.
- Win16 common dialogs: T459 S4 records a real 16-round modeless lifecycle;
  guest OLE client/server was explicitly not accepted there. Do not convert
  host alias tests to guest OLE success.
- ToolHelp: T470 S2's separately staged original guest test returned zero
  for TaskFirst and valid-size ModuleFirst; ClassFirst is original WOW
  UNIMPLEMENTED. Different ToolHelp/KERNEL image pairing was documented; the
  exact private-field cause is not established. This is a negative witness.
- Multimedia: T466 S2 tested a selected synchronous host/body timer route;
  T466 S3 explicitly requires real guest async open/completion/cancel and
  task-loss coverage. Presence of WINMM exports does not satisfy that.
- Print: T468 S2 real Win16 HDC normal/cancel passed with Print to PDF, while
  raw spool lacked legacy profile configuration and its first device-caps
  probe did not return within observation. Review later evidence separately.
- Guest driver groups: T473 S5's PIC/keyboard/mouse fixtures are host device
  evidence, not proof that every historical guest driver/image is selected
  or all hardware operations are correct.

No review class was changed solely due to a matching guest filename,
byte-identical source, image location, or a group-level test. The one guest
source mirror diff remains partial under the owner's rule even though its
line content matches the pinned original.
