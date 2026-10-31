# S11 Win386 sustained mouse investigation

The owner reported that `O:\w31\patch\win386.cmd` enters Windows and freezes
after about five seconds of mouse movement, a regression from earlier builds.
The root cause is not established; neither the reported regression nor its
repair is disproved by the isolated probe below.

On the published `1a1b76419` ten-component runtime, a private-desktop probe
ran an isolated `O:\w31` copy with the same `SYSTEM.INI` and `MOUSE.DRV`
SHA-256 values as the installed tree. The 681-byte PIFs differ only in the
checksum, title and program, working-directory, CONFIG.NT and AUTOEXEC.NT
paths. CONFIG.NT contents match; AUTOEXEC.NT uses corresponding copied paths
and expresses TMP through TEMP in the copy. This does not establish identical
complete launch environments. The probe used the
existing synthetic input-sink route, not physical mouse input. After an
initial burst and 100 alternating movements, each acknowledged input showed
fresh surface pixels, and a subsequent move/down/up opened Program Manager's
File menu, showing guest response in this controlled run. Evidence:
[observations](s11-win386-mouse-freeze-evidence/mouse-observations.txt),
[menu capture](s11-win386-mouse-freeze-evidence/mouse-down-3.bmp).
The observer subsequently ended its 75-second observation and terminated the
isolated processes; this was not a clean Windows exit test.

The first trial's attempt to issue a subsequent click from outside the owned
probe desktop failed to attach; it is not a guest failure. The second trial
performed the click inside the same observer. These observations do not
exercise continuous physical Raw Input, prove the reported five-second
freeze fixed, or establish equivalence of the complete launch contexts. Next
investigation must compare those contexts and input paths, identify the last
progress boundary on the failing path, and retain the guest media unchanged.
The synthetic hook enters the frontend output boundary directly. Real captured
motion arrives through `WM_INPUT`, `kvm_win32_mouse_move` and the frontend's
queued/coalesced mouse delivery first, so those paths require separate evidence.
