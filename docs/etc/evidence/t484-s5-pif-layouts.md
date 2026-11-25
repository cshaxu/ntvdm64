# T484 S5 P4 Standard and NT-only PIF fixtures

The existing CMD fixture-copy entry and released PIF editor now prepare
the 391-byte standard and 555-byte NT-only PIF layouts from two authored
templates. The mixed-batch and DOS-observation PIF writers, their two native
targets and 61 C lines were removed. The DOS-observation build entry still
builds its six separately authored DOS witnesses.

Old and new writers produced identical bytes for the standard layout and
for both tested NT flags. The x86/x64 native payload suite passed all 26
cases, including standard and NT-only layouts, executable and argument
fields, both NT flags and rejecting a second copy without changing the
existing file. The two NT outputs from that run also matched the old
writer's hashes. The DOS-observation fixture build passed separately.
Receipts are under build/t484-s5-pif-layout-fixtures.

The binary path binder serves fixed-capacity fields in live Win31 and Win95
test witnesses. It is not a PIF or INI producer. Direct parser and fault
fixtures retain independent byte construction for malformed or isolated ABI
cases.

PID 11080 passed the cached full build and verified the eleven host images
against both release locations and all fourteen addon release hashes.
