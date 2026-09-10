# M0 T396 S9 — WOW private hard-error contract audit 001

## Question

What response contract is required at the exact `wow:private-hard-error`
frontier, and can the current unavailable adapter be replaced by a simple
success or fixed-choice stub?

## Reached boundary

The complete-archive S8 run reached the existing adapter
`NtRaiseHardError` implementation and recorded:

```text
MVDM-SESSION-TERMINATION origin=wow:private-hard-error code=0x00000078
```

The adapter deliberately marks the session backend unavailable and terminates
it with `ERROR_CALL_NOT_IMPLEMENTED`. This is the direct reason the run never
loads `WOW32.DLL`; it is not a CPU, BIOS, BOP, or app-launch failure.

## Original transport contract

Original `softpc.new/host/src/nt_error.c:WOWpSysErrorBox` constructs an NT4
hard-error request with:

| Field | Original value / ownership |
| --- | --- |
| status | `STATUS_VDM_HARD_ERROR | 0x10000000` |
| parameter count | 4 |
| Unicode mask | bits 2 and 3 |
| parameters 0--1 | packed button descriptors |
| parameters 2--3 | stack-owned `UNICODE_STRING` title and message |
| response options | 0 |
| returned response | broker value, consumed before the Unicode strings are freed |

It maps returned broker values `1`, `2`, and `3` respectively to
`RMB_ABORT`, `RMB_RETRY`, and `RMB_IGNORE`; every other status/value is
deliberately treated as `RMB_ABORT`. Thus `NtRaiseHardError` cannot safely
return a made-up success, zero, or a universal fixed response.

## Reached response consumers

| Original caller | Button contract | Consumed result |
| --- | --- | --- |
| `ErrorDialogBoxThread` → `WowErrorDialogEvents` | close/retry/ignore derived from `ERRORDIALOGINFO` | close invokes original WOW hung-task termination; retry/ignore populate the worker reply |
| `WOWSysErrorBox` export | three caller-supplied `SEB_*` descriptors | returns the raw 1/2/3 result to WOW32 callers |
| `wow32.c:W32Exception` | close, optional debug/cancel, ignore | 2 starts debugger; 3 continues/handles the exception; other values force task exit |
| `wudlg.c:WU32SysErrorBox` | three values from 16-bit `SYSERRORBOX16` | returns the selected response directly to the 16-bit caller |
| `wuser.c:WU32SetEventHook` | close/ignore | only 3 preserves the task; every other value sets `RET_FORCETASKEXIT` |

The source search found one original `NtRaiseHardError` invocation, but its
single transport feeds all five response-sensitive paths above. The `ULONG`
parameter array is original x86 data; this T396 product is x86, so its
pointer-sized ABI is not broadened by this audit.

## Ownership and admission result

The original operation is a private NT4 CSRSS hard-error broker, not public
`MessageBox` and not a generic `NtRaiseHardError` emulation. The product needs
a narrow, explicit response broker with an owned presentation/automation
policy, copied input lifetime, and an exact 1/2/3 output. It must be selected
only for the `STATUS_VDM_HARD_ERROR` four-parameter form reached here.

S9 admits no implementation yet. In particular it rejects replacing the
current unavailable path with a fabricated abort, retry, ignore, success, or
CSRSS-wide emulation. The next implementation packet must first choose the
response/presentation policy and prove how it reaches the caller without the
current session-longjmp controlled stop.

## Existing broker and source-recovery boundary

The current `broker` component cannot be repurposed as this response path.
Its version-one `broker_wire_message` carries only a fixed numeric
`payload_kind`/`payload_value`; its three operations are registration,
disconnect and one-way notification.  It has neither a copied text payload,
a synchronous reply operation, nor a response-owner interface.  The separate
`broker_base_vdm_record` is likewise a bounded launch-record exchange, not a
UI or hard-error protocol.  Using either object to carry the stack-owned
`UNICODE_STRING` pointers from `WOWpSysErrorBox` would violate both their
defined roles and the copied-value broker rule.

The mandatory recovery ladder is therefore:

| Rung | Disposition | Evidence |
| --- | --- | --- |
| Original source reuse | Retain `WOWpSysErrorBox` and its exact `NtRaiseHardError` ABI/call order.  The original server endpoint itself is not composable. | Its peer is NT4 CSRSS private hard-error transport, an explicit source-policy stopping boundary. |
| Smallest same-shaped facade | Candidate only: the existing adapter export may decode **only** this four-parameter `STATUS_VDM_HARD_ERROR` form while the two `UNICODE_STRING` values are live, validate lengths, and copy title/message plus the three 16-bit button descriptors into a new package-specific record. | It preserves original stack lifetime, argument order, `NT_SUCCESS` failure rule, and the caller-visible 1/2/3 response domain without exporting a native pointer. |
| External-code intrusion | Rejected. | No adopted external component supplies the private CSRSS broker, and importing one would widen the stopping boundary rather than preserve the reached contract. |
| Newly authored behavior | Deferred pending a product-owned response policy. | A record transport can be newly authored only after its visible response owner and headless behavior have been selected; it cannot choose a response itself. |

The finite candidate boundary is consequently a **package-specific WOW
hard-error request/reply record**, not an extension of generic `NOTIFY` and
not an app call from `adapter-mvdm-host-out/win32`.  It would contain fixed
width version/size/request/session fields; exactly three copied 16-bit button
descriptors; bounded copied text lengths and UTF-16 payloads; and a response
enumeration restricted to `1`, `2`, or `3`.  The adapter owns x86 parameter
decoding and copying; the broker-facing response owner owns presentation or
declared automation; the adapter maps only a validated reply back to the
original `ULONG` response.  No native pointer, `UNICODE_STRING`, guest value,
window handle, or session mapping token may cross the boundary.

This defines the technical boundary but deliberately does not admit it for
implementation: selecting which visible owner may issue close/retry/ignore,
and what happens when no interactive owner exists, is product behavior.  The
original source has no standalone modern-console answer because CSRSS owned
that policy.  Until that choice is made, the existing controlled-stop path is
the only non-fabricating behavior.

## Closure correction: selected visible presentation policy

The owner selected visible, synchronous modern Win32 presentation for this
and every other source-defined error-dialog interaction. This is not a new
generic dialog policy: each source family retains its own button descriptors,
default selection and result domain.

The audit found a direct original presentation starting point that narrows the
former candidate boundary further. `nt_error.c:ErrorDialogBoxThread` already
uses the original `ERRORPANEL` Win32 dialog resource with
`ErrorDialogEvents`; `resource.rc` supplies its terminate/retry/ignore
controls and the source maps them to `RMB_ABORT`, `RMB_RETRY`, and
`RMB_IGNORE`. Only its `VDMForWOW` branch routes through
`WowErrorDialogEvents` to the unavailable private service. The first S10
decision is therefore to attempt original dialog/resource reuse at that branch
before authoring any new presentation record. `WOWpSysErrorBox` remains a
distinct caller contract: its `SEB_*` descriptors and raw `1`/`2`/`3` return
must be preserved, even where its labels differ from `ERRORPANEL`.

S9 is closed. S10 owns the complete per-family dialog inventory and the
source-first implementation proof; no S9 audit result itself claims runtime
recovery.
