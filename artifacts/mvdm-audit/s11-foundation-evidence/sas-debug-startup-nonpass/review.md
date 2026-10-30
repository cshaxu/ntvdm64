# SAS startup debug observation: non-pass

A test-only debugger attempted to start an isolated broker under DEBUG_PROCESS,
then launch the unchanged DOS MEM guest, checking the selected CCPU tail
breakpoint address adjusted for the worker image base. The launcher exited
with 1722 (RPC server unavailable); no NTVDM process creation event was
observed. The broker did produce debugger events. The result therefore says
nothing about SAS execution, startup reachability, or guest compatibility.
It is not a product regression verdict or a successful test.

The first attempt likewise returned 1722 without identifying a worker; the
retained second attempt adds debugger event logging. Its exact test source,
run wrapper, output and binary/input hashes are recorded here. The reason for
the broker/launcher failure was not established. No guest or installed product
was changed, and the isolated stage was cleaned by its established owner.
