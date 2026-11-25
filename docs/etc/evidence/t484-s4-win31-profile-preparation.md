# T484 S4 P45 Win31 profile test preparation

The existing CMD guest witness helper now builds and stages the profile or
DDE witness and initializes an empty PROGMAN.INI in an owned guest copy.
Both tests use the current single-PIF Win31 launcher. The selector's
optional Win31 mode accepts Standard or Enhanced while preserving the
existing Enhanced default. The tests retain their separate runtime
assertions and source installation hash checks.

The first run stopped before guest startup because both callers still
required retired WINSTD.PIF and WIN386.PIF files. With current WIN.PIF
selection and the normal logon environment, the empty-profile and DDE
tests passed in Enhanced mode with DOSONLY=0, including normal Windows
exit. DDE also passed in Standard mode with DOSONLY=1. The DDE witness
created four groups and verified the saved profile references. The
empty-profile witness verified INI writes and zero/nonzero Win16 file I/O.

Evidence is under build/t484-s4-launch-matrices in win31-retry and
dde-standard. The cached full host build completed and the eleven raw
images matched both published locations. Nothing required republication.

Two supplemental callers are delivered. The Win31 setup matrix and Win95
launch matrix remain open. The Win95 launch matrix change is outside this
commit.
