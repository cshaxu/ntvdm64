# T200 S9 — COMMAND exhausted-plan native terminal

One source-built CPU5 native run at
`artifacts/build/t200-s9-command-terminal-native-r1/` reports `runs: 1`.
Its CMDINFO observation records `accepted=1 outcome=2 delivered=1` for the
exhausted `54:01`: the command session accepted the request and produced the
typed stop disposition.  The fixture's passive raw-boundary marker remains
`observed-stop=0` and `next=00:00`; no generic raw #UD escaped.

This closes the current one-slot COMMAND launch/terminal slice.  It does not
claim an interactive COMMAND shell, WOW, broad process execution, or general
NTVDM completion.
