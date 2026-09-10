# T224 S2 COMMAND Stream-Session Provider

## Result

S2 replaces the former ambient `stream_child` initializer with one
COMMAND-owned `stream_session`.  Initialization now constructs only versioned
empty state.  Standard streams are captured exactly once through the explicit
CLI composition admission after the COMMAND mutation profile has selected
Direct.  Readonly cannot enter that admission path.

## Source Relation And Boundary

OpenNT's `cmdmisc.c:356-358` and `cmdredir.c:183-315` establish the original
bootstrap snapshot and `54:06` handle-return contract.  The historical direct
composition remains rejected for the S1-recorded CCPU/SAS, `VDMInfo`, 32-bit
`pRdrInfo`, SoftPC notification and legacy worker/termination dependencies.

The source-derived seam preserves the only admitted portion of that contract:

| Concern | S2 behavior |
| --- | --- |
| lifecycle owner | COMMAND launch/execution package, not Bochs or a BOP leaf |
| initialization | no `GetStdHandle` call and zero available-stream mask |
| Direct admission | CLI composition invokes one post-profile capture method |
| guest result for `54:06` | fixed-width opaque generation-and-slot token in `BX:CX`; `AX:DX` zero; no `HANDLE` or pointer crosses the boundary |
| Readonly | no capture; `54:06` returns the retained CF-set zero terminal |
| future Overlay/Virtual | their policy results never satisfy `DIRECT_HOST`; no fallback or capture is installed |
| retained exclusions | `54:08/0A`, `50:47/48`, workers, temporary files and pipe cleanup remain unchanged and deferred |

## Verification

The source-built MSVC x64 `/MT` focused fixture
`tests/bx-vdm/bx_ntvdm_command_launch_execution_provider_v1_test.c` was built
and run through `Invoke-T198S16BootNamespaceVdmX64Probe.ps1` in disposable
`build/M0-T224-S2/005-command-stream-session-token`.

It passed (`linkExitCode=0`, `runExitCode=0`) and proves:

1. provider initialization exposes no captured standard handle;
2. Direct-only explicit admission captures all three CLI standard streams;
3. `54:06` returns a `0xB...` opaque token rather than the stored private
   handle value;
4. a token from an earlier session is rejected by the later session, while a
   complete current-session `STD_HANDLES` record is accepted; and
5. Readonly admission is refused, leaves the stream mask empty and receives
   the CF-set `54:06` terminal.

The larger adapter-only `boot-namespace` fixture compiled and linked but its
run reached the pre-existing `124` timeout in the out-of-scope Direct
`54:08` child-execution path.  It is neither evidence for S2 nor a reason to
extend S2 into child execution.

## Follow-up

The next admitted T224 subpackage is the original-owner DEM pipe/SFT
continuation pair (`50:47/48`), including its EOF, size and no-pipe terminals.
It must consume this session boundary rather than expose a private handle or
start a trace-selected individual service implementation.