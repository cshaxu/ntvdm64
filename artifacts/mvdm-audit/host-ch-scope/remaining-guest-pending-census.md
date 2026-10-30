# Remaining mixed-inventory guest C source roles

Fifteen pending paths remain after the separate DOS, WRITE, DDEML, MMSYSTEM,
OLE, USER, TOOLHELP and REGEDIT reviews. They comprise three COMMDLG, one
KILLWOW, one MCIOLE, one RASAPI16, four SHERLOCK and five WFWNET C sources.
The [per-file census](remaining-guest-pending-census.json) records original
and current hashes: all 15 are byte-identical and their same-stem objects
appear in the corresponding original makefiles. The makefiles describe
16-bit guest targets and linked object sets. WFWNET and RASAPI16 also have
conditional compiler selection, so a makefile match alone must not be
generalized to all configurations. No makefile branch or guest was built.

The exact 15 paths were absent from the inspected x86 worker/WOW32 Ninja
commands and recorded dependencies; graph-query positive controls are in the
DOS crosswalk. [Bounded host-source dispositions](remaining-guest-pending-host-dispositions.csv)
exclude these guest roles from that selected host input scope only. The
original services, debugging and provider behavior remain separate runtime
obligations. Other profiles, complete dependency capture and guest execution
are not proved. The frozen mixed-scope ledger and guest images are unchanged.

Together with the five other pending-source disposition tables, this accounts
for the 195 rows previously tagged `guest-target-observed`. This is a source
role crosswalk, not a final T-wide functionality or test completion claim.
The [crosswalk verifier](check-guest-pending-crosswalk.py) checks all 195
paths for uniqueness, current/original hashes, absence from the current mirror
diff table, selected x86 graph absence with positive controls, and continued
`pending` status in the mixed ledger. Its [result](guest-pending-crosswalk-verification.json)
binds all six disposition tables. The older scope-correction policy explicitly
reopened whole-file decisions when only a bounded host role was proved, so
these records must not be merged as final whole-file exclusions on this basis.
