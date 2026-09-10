# T229 S2 P15 — DEM pathname search Direct/Readonly conformance

OpenNT `src/opennt/base/mvdm/dos/dem/demsrch.c` keeps pathname search in the
DTA contract: `demFindFirst` obtains the registered DTA, writes its first
result, and `demFindNext` consumes that retained state. The original first
search path explicitly retains `ERROR_NO_MORE_FILES` as its terminal error.

The formally built whole-provider session fixture now has a `search-only`
mode that limits the run to the supported Direct and Readonly profiles. In
both profiles it verifies:

- `50:09` pathname FindFirst succeeds through the installed DEM session;
- `50:0B` consumes the associated DTA continuation and succeeds;
- a no-match `50:09` returns the source-shaped `AX=18`, CF-set terminal.

The command was:

```text
ninja -C build/M0-T229-S2/formal-search-session-20260819 bin/t229-s2-dem-whole-provider-session-fixture.exe
build/M0-T229-S2/formal-search-session-20260819/bin/t229-s2-dem-whole-provider-session-fixture.exe search-only
# exit 0
```

The fixture leaves Overlay and retained Virtual test paths untouched and does
not use them as acceptance evidence. This establishes normal DTA continuity
and the no-match terminal, not stale-DTA or attribute-search variants.