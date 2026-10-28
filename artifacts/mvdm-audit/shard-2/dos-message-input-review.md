# DOS message data and LOADFIX source inputs

This read-only shard-2 group covers four pending physical message paths. The
[observer](dos-message-input-observer.py) and [receipt](dos-message-input-observations.json)
verify their bytes against both pinned original editions and DOS16/WOW32
catalog identities, and check the original recipe and source edges described
below. All four retain 100% original bytes; this is not an estimate of guest
behavior or locale coverage.

| Path | Proposed role | Evidence and remaining boundary |
| --- | --- | --- |
| `dos/v86/messages/loadmsg.msg` | excluded candidate: historical guest build input only | Original LOADFIX assembly includes this file and references its four help/error labels; the original recipe declares it as an input. Preserve LOADFIX command behavior and message language as separate obligations. |
| `dos/v86/messages/usa-ms.idx` | excluded candidate: historical generated build input only | Original messages recipe defaults COUNTRY to `usa-ms` and builds the index from the message file; original BUILDMSG constructs and opens a country `.IDX`. No assertion is made that retained index bytes were produced by a particular build. |
| `dos/v86/messages/germany.msg` | pending | German messages are distinct source data; the parameterized message recipe permits country variants, but the supported build profile and shipped localized guest behavior are not demonstrated. Do not infer they are redundant from the default U.S. setting. |
| `dos/v86/messages/holland.msg` | pending | Dutch messages have the same unresolved variant/build and shipped behavior questions; source presence is not acceptance. |

The ZIP member `WINNT/SYSTEM32/LOADFIX.COM`, the installed copy and the
source-tree prebuilt copy match the recorded SHA-256. Their identity does not
prove that the historical assembly inputs built that binary, that LOADFIX was
executed, or that every message path works. Actual process execution, error
and return semantics remain separate product requirements. Original build
recipes are only read, never run; guest images and current installation are
unchanged. The coordinator should replay the observer and independently review
before applying either proposed file-role exclusion.
