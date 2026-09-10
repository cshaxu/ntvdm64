# T198 S94 source-built normal-return observation 001

## Result

The one permitted r2 native run linked the current MSVC x64 `/MT` composition
with the S93 source-built COMMAND.COM and SHARE.EXE arrays. It reached a
controlled stop at `0BB8:1B10`; the copied next BOP identity is `54:02`.
It did not reach SHARE execution, `50:3C`, or `54:11`.

## Attribution

This is the fixture bridge's generic unhandled-BOP stop. It records the copied
selector/service after composition declines a four-byte BOP, then returns typed
STOP. It is neither a watchdog nor a DOS process result.

OpenNT maps service 02 to `SVC_CMDCOMSPEC` and dispatches it to `cmdComSpec`.
On its first call, the original handler reads the NUL-terminated string at
DS:DX, forms `COMSPEC=<string>` host state, and sets AL; later calls return.

The repository already contains a bounded source-derived companion for the
whole bootstrap pair: `54:02` gathers no more than 64 checked bytes and forms
the fixed environment entry; `54:0F` writes that entry to checked guest RAM.
Neither endpoint is a current `boot_namespace_composition` route. Composition
therefore correctly declines `54:02` today.

## Follow-up

No provider changed and no second run is allowed by S94. The next work audits
the complete COMMAND bootstrap component (`54:02` and `54:0F`) through the
global composition rather than adding an isolated service recognizer.
