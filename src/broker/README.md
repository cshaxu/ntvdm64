# broker

Versioned, fixed-width cross-process coordination contract. The legacy wire/
registry modules below remain in-process contract fixtures. S3's rpc_security
module supplies the reusable native authentication boundary; it is currently
exercised by real RPC fixtures, not yet by the three product executables.

`rpc_security.c/.h` is new finite modern transport glue, not original BaseSrv
policy. NT4 CSR port/process authentication cannot be reused without the
excluded CSR runtime. The admitted local RPC route instead requires WINNT
packet privacy and matching OS token logon LUID/session. Scope is captured from
the owning process, never copied from a command. Native state in this private
header is not wire data. Every impersonated path reverts; failed RPC reversion
denies access and falls back to public RevertToSelf, with fail-fast if neither
can restore the thread. That exceptional failure branch remains unexercised.

The same module validates a process attachment at request entry by repeating
authorization, querying the runtime's local client PID and comparing it with
GetProcessId on the attached live process. A valid handle to a different
same-user process is not caller identity. The input remains RPC-owned; a future
registry must retain its own reference before return and assign a generation,
not keep a borrowed handle or use PID alone. This is peer identity, not authority
to register an arbitrary worker on behalf of that peer.

`vdm_receipt.c/.h` supplies the admitted receiver-local stream/wait retention
mechanic. The original CSR target-local HANDLE delivery cannot cross the
fixed-width standalone protocol; source policy remains in BaseSrv. Authenticated
typed file/event endpoints may retain a same-access reference and issue a
generation-scoped numeric receipt. Failed acquisition publishes no ID; IDs do
not wrap or get reused within a generation; revocation is repeat-safe and drain
releases outstanding references. Console objects are excluded from this path.
The owner must serialize receipt operations and resource use/revocation; a
resolved HANDLE is borrowed until revocation, never serialized. Initialization
requires fresh/drained storage and a fresh trusted generation. This is not a
generic remote handle service, task policy, or authentication layer.
The real RPC fixture exercises retention/use/drain, but final wire receipt
acknowledgement, source-shaped duplication binding, alias/reply-loss handling
and concurrent endpoint integration remain unfinished. It is not yet selected
by a published product executable. Details are in the
[S3 entry evidence](../../docs/etc/evidence/m0-t412-s3-entry-integration.md).

`vdm_delivery.c/.h` is the pending-acquisition journal required by original
BaseSrv's partial stream update behavior. Reserve bookkeeping before RPC,
acknowledge only a validated generation/receipt, and commit at original
successful ownership publication. Rollback revokes only acknowledged receipts,
retains failed revocations for retry, and leaves unknown replies pending. The
explicit forget operation requires proven non-delivery or recipient rundown;
it must never discard an indeterminate live-peer receipt. Endpoint contexts
and journal storage must outlive pending work. Operations are owner-serialized;
callbacks must not mutate/reenter that journal. This is resource cleanup only,
not a new task transaction or a replacement of original service ordering.
The focused local test passes; real transport integration/rundown is pending.

The formal x86 `opennt-broker-owners` target also builds `broker-transport.lib`
from rpc_security, vdm_receipt and vdm_delivery using native SDK declarations
and /MT. Its manifest records source hashes and object membership. Resource,
delivery and original-lifecycle tests link this archive and check map providers,
rather than privately recompiling those production units. This is a shared
build input for the pending product links, not a completed broker executable.

`vdm_payload.c/.h` defines only the eight copied variable-buffer spans from
original CheckVDM/GetNextVDMCommand: command, application, PIF, directory,
environment, desktop, title and reserved text. Native CSR capture pointers
cannot be reused as cross-process values. Each fixed-width span separately
preserves pointer presence, original capacity/required length and copied byte
count. Canonical contiguous offsets reject overlap/trailing bytes; overflow
and short encode buffers fail without writing output. No task slots, length
negotiation policy, pointers or handles are serialized. Inputs must not overlap
output. This x86 little-endian payload fragment is in broker-transport.lib and
has formal-provider tests, but is not a full/versioned command envelope or
native-message decoder. Operation/direction/string semantics, startup scalar
state, resource IDs and authentication remain required integration work.

M0 T272 S5 disposition register:

- `wire.c` and `wire.h`: `new neutral contract`.  There is no reusable
  project-local broker source.  The fixed-size record deliberately excludes
  native pointers, handles, local surrogate IDs and guest pointers.
- `broker.c` and `broker.h`: `new neutral registry`.  It gives stable,
  monotonic broker IDs, binds each client to an already-authenticated fixed
  user key, and removes the record on disconnect.  A later transport must
  authenticate the user key using public OS facilities before dispatch; it
  cannot manufacture a cross-user route.
- `base_vdm_record.c` and `base_vdm_record.h`: `new source-shaped record
  boundary`. They are the fixed-width DOS subset of the original BaseClient /
  BaseSrv `VDMINFO` exchange: copied request/result fields, same-record
  pending/no-command state, and disconnect ownership. They deliberately omit
  CSR capture buffers, raw `HANDLE`s, guest pointers and local mapping IDs.
  The current state core is transport-neutral; `adapter-mvdm-host-out/basesrv`
  is the only permitted original-call binding, and a later public pipe/event
  transport may use this exact record contract.

Project-owned per-user cross-process coordination component. Command records
contain only versioned copied values. Required native resources use only the
separate authenticated OS-managed attachment exception, never command fields.
