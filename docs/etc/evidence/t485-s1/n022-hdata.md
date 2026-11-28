# S15 P55 N022 DDE data handles - delivered

## Scope and fixed inputs

Start HEAD348bbe8ae, runtime63 delivered. hdata.c uses XP SP1 blob
8db5b7465fb5279dd420d5fd4995b7a5da645ac5 at fixed revision
a2f6d7c93aa4f11efc51147566d5961c59a8ef89; previous OpenNT blob
ab1cae5c6916a1e83d47dd53753f9266dde81a37. Before nonerror1point/985lines.
Global1329files/785byte-identical/796nonerror-identical/533different/
7836points/92889lines;1292XP/37OpenNT. Original guest unchanged.

## Whole-file scope and necessary omission

Original functions DdeCreateDataHandle, InternalCreateDataHandle, DdeAddData,
DdeGetData, DdeAccessData, DdeUnaccessData, DdeFreeDataHandle,
InternalFreeDataHandle and ApplyFreeDataHandle require the private DDEML
instance, critical section and handle table. Current production only registers
FreeDDEData in the WOW output handler; wumsg.c invokes that handler on raw
shared data handles. HDDEDATA management handles and raw HGLOBAL data are
not interchangeable. XP InternalFreeDataHandle validates the private handle,
reads its flags, releases its data and destroys that private handle.

Repository symbol search finds no ValidateInstance, ValidateCHandle,
PciiFromHandle, CreateHandle or DestroyHandle implementation for DDEML.
Pinned XP ddemlcli.c owns pciiList and gpDDEMLHeap; its companion instance and
handle machinery is outside the admitted independent USER data boundary.
The full XP source, with the existing host include and logging macros bound,
fails under current product compiler flags (full-bound.log, exit2):
PCL_INSTANCE_INFO, EnterDDECrit, HDATA_EXECUTE and related owners are missing.
This is not proof that XP is faulty or that unused code must be deleted.
Restoring those functions requires admitting the complete private DDEML
manager, rather than declarations alone or mapping its handles onto public
USER32 DDEML. Keep the existing unconditional omission explicitly visible;
do not add a synthetic #if/#error to hide it.
The unconditional omitted range is XP lines15-475 (including surrounding
whitespace); the retained FreeDDEData comment begins at476. The raw residual
blocks are the include replacement and this omission/#line anchor.

## Restored code and ABI

Restore complete XP FreeDDEData, CopyBitmap, CopyPalette and CopyDDEData,
including unused copy helpers. XP initializes CopyBitmap's failed return
handle and frees a newly created palette if SetPaletteEntries fails.
Keep all XP casts, lock/free order and nested format handling unchanged.
The C source includes original w32w64.h for KERNEL_PVOID. The existing B
native-data contract uses public PVOID, matching the selected x86 original
windows/published/ddetrack.w layout (8 bytes, Data offset4); no B-to-C header
or runtime dependency is added. Existing test validates this shape and invalid
bitmap/palette inputs, plus nested format release and copy independence.
The original retail UserAssert is non-evaluating. No second DDE algorithm,
new state machine or original guest change.

## Final verification and delivery

runtime64 cached full build, dual13-image26hash publication, controls40/40,
real CCPU window-borrow and genuine guest SOL Options pass.
[Receipt](n022-hdata-result.json), [census](n022-hdata-census.json).
N02226/39; overall57/159 and S15 directories9/14 unchanged. Four completed
mirrors since P51 runtime60 Full24/24; no new Full or S/T closure claimed.

## Mirror accounting

hdata.c OpenNT1point/985lines becomes XP2points/465lines. The residuals are
host includes and unconditional private DDEML-manager omission with #line476.
There are no excluded #error branches in this file. All retained functions
are byte-identical XP source with correct diagnostic line mapping.
Original LF restoration separates the two residual blocks.
Global before:1329files/785byte-identical/796nonerror-identical/533different/
7836points/92889lines. After:1329/785/796/533/7837/92369.
Delta:0/0/0/0/+1/-520. Baselines1293XP/36OpenNT. Newline/EOF counted.
