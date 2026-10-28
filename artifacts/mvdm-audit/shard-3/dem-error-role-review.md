# Shard 3: original DEM failure and hard-error owner

Proposed status for `dos/dem/demerror.c`: **partial**. The
[observer](dem-error-image-observer.py) and
[observations](dem-error-image-observations.json) verify both exact original
editions, DOS/WOW catalog IDs, selected compile/archive/dependency evidence,
and all five mapped functions against the released NTVDM image with
relocation words masked. Two actual `apfnSVC` pointers resolve in that image:
index 50 to `demSetHardErrorInfo`, 51 to `demRetry`. Four relocations from
the linked `demOpen` body resolve to the linked `demClientError` owner.

The original DEM owns conversion from host thread `GetLastError` to DOS AX
and CF or to an INT 24 hard-error packet. DOS supplies the packet and device
chain addresses; the original `demRetry` uses the saved SVC state. Modern
Windows owns the actual handle and native error. The four observed calls prove
one selected filesystem error chain, not that its failures or retries were
executed. No compatibility claim is made for unrelated error presenters.

Remaining: actual guest registration and retirement of packet pointers;
invalid/remote handles and device-name translation; native errors including
zero, ordinary errors, hard-error range and WRONG_DISK; AX/CF preservation;
retry correctness, nested/reentrant DOS operations and races with teardown.
The original source relies on INT 24 and failing-SVC register assumptions;
those assumptions are not validated by the linked table. Existing successful
guest file operations do not exercise those failures. Source text retention is
100%; complete functional recovery percentage is currently unmeasurable.
No product or guest image changed.
