# T222 S2 Drive-View Drive-List Session Regression

`50:0F demGetDrives` now runs through the admitted Direct/Readonly normal
session.  With only C: admitted, both profiles resume at `+4` with `AX=3`
and `CF=0`, preserving the provider's DOS letter-bound contract.  The MSVC
x64 `/MT` fixture in `build/M0-T222-S2/027-drive-view-drive-list` exited zero.
This is a single matrix entry; media and DPB entries remain open.
