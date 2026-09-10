# M0 T282 S2 — COMMAND declaration carriers

Import the exact original OpenNT `public/internal/base/inc/vdmapi.h` and
`public/internal/windows/inc/pif.h` into the platform-ABI mirror and make the
COMMAND static surface select them. This is declaration recovery only.

No `GetNextVDMCommand`, PIF parser, process launch, BOP dispatcher, provider,
or guest path may be implemented or enabled. The existing empty adapter
`vdmapi.h` placeholder must not shadow the exact carrier. A successful build
remains a selector-disabled static archive only.

Verify source hashes against the pinned OpenNT sources and probe both MSVC x64
and x86. Any remaining missing function is recorded under its named session,
adapter-win32, adapter-softpc, Redirector or specialist owner; it is not
implemented in this declaration S.
