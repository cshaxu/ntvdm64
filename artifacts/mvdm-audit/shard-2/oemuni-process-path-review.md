# S10 shard 2: OEMUNI process/path conversion owner

`oemuni/process.c` is a selected, modified original host source. Rechecked
OpenNT SHA256 `38C9125F9F3C43F32F696616E565E63B8F056DA7EC264D980A5E5278CC640468`
and current SHA256 `FCF8695132E69E1612C08114A52938529131B951673291CC9C4CCEFF82C862B1`;
its status remains partial. Original RTL, Unicode/ANSI providers, source
entrypoints and cleanup own conversion and error results; the product's
system-directory binding supplies only the selected package path.

## Changed families and caller boundaries

- DIV-269 initializes `SearchPathOem`'s result to zero so allocation/query
  failure cannot return uninitialized success. DIV-275 separately obtains a
  full Unicode result before OEM-byte sizing and conversion in system,
  Windows, temp and search-path calls. It preserves separate required-size
  versus successful-byte counts, terminators, and SearchPath file-part prefix
  conversion; query-growth and allocation errors do not publish a stale path.
- DIV-276 sizes the Unicode and OEM result independently for short-path
  conversion and rejects growth. Original COMMAND `cmdpif.c` calls
  SearchPathOem and GetShortPathNameOem; full success includes original short
  path and cleanup, not merely conversion fixture return values.
- DIV-277 sizes the environment ANSI intermediate independently from the OEM
  destination, checks UINT16 conversion bounds, handles successful empty
  values and avoids conversion after failure. Original COMMAND and WOW wdos
  callers remain the environment behavior owners. DIV-273 corrects borrowed
  output and OEM-result cleanup/count for environment expansion; original
  SoftPC nt_pif is its caller.
- The old `CreateProcessOem` source is inside `#if 0 // unused`; no selected
  product execution should be claimed from that body. Replacing disabled
  source with a new launcher is outside this conversion cohort.

The current selected x86 worker map (`ntvdm.exe` SHA256
`D3437091D920A74FF72FDC8ED023C6207642682A7739C45751E4F03A8C1D53E5`, map
SHA256 `641E5C33430445ECC73885176250194BE936549654E14864074BD0B7D92D3CE2`)
lists the four SearchPath/GetEnvironment/GetShortPath/Expand functions from
`original-mvdm-host-oemuni:process.obj`. This establishes extraction, not
which caller ran or proof of every export.

## Existing evidence and remaining verification

T420 S37 records actual-source CP932 unequal-byte-count and buffer-size
fixtures, allocation/conversion failures with cleanup, real DOS direct/nested
COMMAND environment witnesses and historical package regressions. It also
explicitly bounds direct consumers: SearchPath/short path through PIF, env
through COMMAND and WOW, expansion via nt_pif; some host helper checks have
no matching guest or WOW execution evidence. Source fixture or controlled
CP932 endpoint is not proof of behavior on every host ACP or DBCS input.

A finite current-artifact matrix should bind success/short buffer, missing
terminator, changed-between-query growth, conversion and allocation failure,
borrowed versus owned destination, and exact required-size/error result to
the actual PIF/environment callers. Independently verify package-root system
and temp path resolution, non-ASCII DBCS prefixes and WOW env caller where
admitted. T420 historical per-case receipts should be matched to current
source/binary hashes before reuse; no old receipt is newly run here. Guest
media remain unchanged. This identifies evidence gaps, not a newly reproduced
fault or a reason to add a second OEM conversion provider.
