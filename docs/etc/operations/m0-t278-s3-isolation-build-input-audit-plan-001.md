# M0 T278 S3 — component isolation and build-input audit plan

S3 is a read-only post-move audit. It compares every file under the four
MVDM mirror roots to the S1 destination ledger, allowing only the component's
own README outside the ledger. It repeats all SHA-256 checks.

The build-input sweep is restricted to current build/configuration files. The
historical OpenNT build descriptions inside the mirrors are source evidence,
not active project build inputs. Any current reference to a moved old host
path is recorded as an unenabled successor binding debt rather than repaired
by this physical re-root task.
