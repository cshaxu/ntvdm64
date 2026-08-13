# T190 S4 COMMAND bootstrap derivative closure 001

Date: 2026-08-12  
Packet: M0 T190 S4  
Disposition: exact derivative source-built; no bundle or guest execution.

The corrected generator prepared
`artifacts/build/current/t190-command-bootstrap-derivative-r2` from the
pinned T189 S5 engine. Its manifest declares exactly three allowed object
rules: generic `cpu/exception.o`, adapter runtime, and adapter COMSPEC
bootstrap service.  It records the retained T189 boot-file and provider
object hashes.

The exact MSVC/x86 `nmake` command succeeded and linked
`ntdos64-t190-command-bootstrap.exe`.

| Output | SHA-256 |
| --- | --- |
| Engine | `03C8FFD23FA015B480C02B42250511527450ADE5DCC51F289D4249D617439BCD` |
| Map | `4094B760840214EBB2CEA1588B7AB4B14FBA85A4F92C5025F63BEFAE2BCEE179` |
| Runtime object | `DB01984468F8E2ACA77F3E558832E543E59451DC6FBFC92A9ED0C90E9BA8F798` |
| COMSPEC object | `42C618851193B24C06B3D1771CF053C09D254F9E0F77C7E96069435470EDCE25` |
| exception object | `7A6CDBB1D91ABAC1F187867DF5A964EC55C69BD9BA59EEA73FF9786BAC7E8353` |

NMake rebuilt the two changed adapter objects and linked.  The declared
exception rule did not run because its copied source hash is unchanged from
the valid T189 base and its object was up-to-date; this is a retained verified
object, not a stale adapter implementation.  No archive, device, CLI, guest
or additional adapter object was built.  Retained LNK4042, LNK4098 and
LNK4217 warnings are unchanged.  A separate S is required to create a
manifest-bound bundle and another one before any runtime observation.
