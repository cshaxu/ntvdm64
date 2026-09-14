# T412 S3 entry integration

## Launcher process-support dependency

S2's classifier test supplied its own NtCurrentPeb, current TEB and process-heap
functions. That proved the original classification algorithm but left a real
run16 composition dependency unverified. The S3 test removes those definitions
and links the selected adapter-mvdm-host-out/win32/source/opennt_support_rtl.c.
No new process-support implementation is introduced.

The test checks that the provider initializes ImageBaseAddress from the actual
module. Native classification/path services remain selected through the formal
opennt-base-client and opennt-base-bindings archives. Link-map checks require
NtCurrentPeb, opennt_support_current_teb and RtlProcessHeap from support.obj,
not fixture.obj. The support TU is compiled with /Gy so unused unrelated
helpers are not pulled into this focused link. This is not yet the complete
run16 product link or an audit of every helper in that translation unit.

Verify-BrokerOriginalClassifier.mjs now writes under
build/M0-T412/S3/original-classifier and records the process-support selection.
With OPENNT_BROKER_OWNER_BUILD pointing to S2/formal-owners, the actual original
classifier plus real support pass MEM/COMMAND/EDIT DOS, WRITE Win16, absolute
and relative PE, DLL rejection, extension handling and missing-file error tests.
No guest program is executed or modified.

The three BaseInit suffix strings remain fixture-owned initialization, retaining
the original baseinit.c values from lines 270 onward and basedll.h declarations.
Run16 must compose that initialization rather than import the entire Base DLL
initialization shell. Other pending product dependencies include command-tail
handling, actual broker discovery, original launch calls and error propagation.
Removing test-owned process providers is not three-program acceptance and does
not permit deleting the old product entry before its replacement is verified.
