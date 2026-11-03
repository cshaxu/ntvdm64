# S13: non-PROD and non-NTVDM original diagnostic mappings

Ten sources received original diagnostic mappings for 76 previously excluded
arms. Original nested conditions inside already excluded outer arms remain
excluded as a whole. The original flat conditional structure of `nt_wcom.c`
also completed review. See [guard changes](s13-r664-guard-line-audit.json)
and [per-file review](s13-r664-flat-guard-review.json). Other groups remain
pending; full supported-context enumeration remains open.

The initial image comparison detected one non-timestamp byte in `ntvdm.exe`.
The change was traced to the compiled `__LINE__` argument of the serial error
report in `_nt_poll_comms`. An explicit `#line` restores original OpenNT line
1465; the previous release reported 1466. [Evidence](s13-r664b-line-macro-evidence.json)
records the symbol, object, file offset and the original source line. The final
[comparison](s13-r664b-build-image-compare.json) retains that one substantive
byte change, with PE timestamps separately identified.

Cached x86, x86-extra and x64 builds passed. The x86 guard suite passed 181
controls and 76 isolated MSVC line controls passed. The exact final candidate
passed the Full selected product gates, including Console 17/17 and Window
17/17; DOS4GW NESticle reached an active graphics window without a gameplay or
pixel assertion. All ten images were published and hash-verified in both
destinations. Guest images were neither modified nor rebuilt.
[Counts](s13-r664-counts.json) keep removed files separate.
