# OEMUNI private conversion and error declarations

`oemuni/oem.h` defines original `InitOemString` and `BaseSetLastNTError`
macros. The [observer](oemuni-private-header-observe.py) and
[receipt](oemuni-private-header-evidence.json) record byte identity against
both OpenNT editions, the selected x86 worker dependency groups and cached map
owners. Both `file.obj` and `process.obj` record the exact header dependency;
their sources use the macros for OEM string initialization and NTSTATUS to
Win32 error translation. The cached map identifies selected original OEMUNI
file and process functions, including `CreateFileOem`, `SearchPathOem` and
`GetEnvironmentVariableOem`.

The historical [T420 S37 record](../../../docs/etc/evidence/m0-t420-s37-oemuni-capability-review.md)
reports a selected-source fixture for non-ASCII full path/short buffer and
environment deletion, and real DOS direct and nested non-ASCII file operations
on its specified CP437 formal package. Its later CP932 tests use controlled
mock limits; they are not full DBCS-host or Win16 acceptance. Historical
fixture and guest observations do not automatically bind to the present cached
map or installed package. The selected `file.c` and `process.c` are separately
modified, partial original providers; header identity does not erase their
registered mirror differences or failure obligations.

Recommend **partial** for the physical header's original selected interface.
S12 retains query/grow races, short/unterminated buffers, allocation and
conversion failures, exact Win32 error mapping, PIF/COMMAND/WOW callers and
resource cleanup against the actual delivery identity. The header's source
text is 100% retained; a behavioral completion percentage is not inferred.
Replay the observer from the repository root. No new product, guest, build or
shared-ledger change occurred here.
