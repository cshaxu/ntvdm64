# Adapter source-reuse audit at CSR and kernel-VDM stopping boundaries

## Question

Although CSR/CSRSS, NTDLL CSR transport and kernel VDM are not admissible host
runtime packages, can small original OpenNT source fragments still reduce the
amount of adapter-owned behavior while preserving the required boundary?

## Result

Yes, but only as explicitly selected **source-derived adapter fragments**. No
fragment below becomes a linked CSRSS, NTDLL CSR or kernel-VDM component.

### NTDLL CSR client transport

- `base/ntdll/csrutil.c` contains a useful, bounded algorithmic fragment:
  `CsrAllocateCaptureBuffer`, `CsrAllocateMessagePointer`,
  `CsrCaptureMessageBuffer`, `CsrCaptureMessageString` and the matching free
  order. It defines capture sizing, message-pointer offset registration,
  aligned copy order and cleanup order used by the original VDM client.
- It cannot be directly linked: `CsrClientCallServer` translates pointers by
  `CsrPortMemoryRemoteDelta`, calls `NtRequestWaitReplyPort`, and uses CSR port
  heap/global/TEB state. The original source also uses 32-bit `PULONG` pointer
  adjustment, which is not a valid x64 native-pointer contract.
- A future `adapter-opennt-host` may therefore rehost only the selected
  capture-layout/copy/cleanup fragment with copied fixed-width records and
  session-owned mappings. It must retain the original function spelling,
  message ordering and allocation failure result where the selected `vdm.c`
  client observes them. It must not expose native pointers, invent a port or
  recreate NTDLL CSR transport.

### CSRSS server substrate

- `base/subsys/csr/server/wait.c` contains a reusable *ordering reference* for
  `CsrInitializeWait`, `CsrCreateWait`, `CsrNotifyWait` and destruction:
  create record, attach one waiting context, notify once, release captured
  arguments, then remove/release in deterministic order.
- It cannot be directly linked: each wait block stores `CSR_THREAD`/
  `CSR_PROCESS`, calls `NtReplyPort`, shares global CSR wait/process locks and
  depends on CSRSS capture ownership. `process.c` additionally owns a global
  process/thread database, server-DLL callbacks, exception ports and process
  security/lifecycle; importing it would be rebuilding CSRSS.
- A future `broker` or `adapter-opennt-host` may use the selected wait-state
  ordering as a source-derived, one-VDM-session queue algorithm only after its
  records are converted to copied broker/session IDs. It may not import
  `process.c`, use a global CSR process table or enumerate arbitrary processes.
  Simple refcount/allocation helpers are too small to justify source import and
  should be ordinary bounded implementation with an explicit source reference.

### Kernel VDM

- `base/ntos/vdm/vdm.c` and `x86/vdmentry.c` preserve valuable declaration and
  result evidence: `NtVdmControl` service discrimination, `VDMQUERYDIRINFO`
  layout, and source service/failure directions.
- Their bodies are not candidates for source-derived reuse. Even the small
  query-directory path probes user pages, references kernel file objects,
  builds IRPs/MDLs, raises/lowers IRQL and waits on kernel events. The x86
  dispatcher additionally calls `Ke386*`, accesses current kernel-thread and
  process state, and controls hardware VDM execution.
- `mvdm-platform-abi` retains exact declarations. The monitor/SoftPC adapter
  must implement only individually admitted services with public modern APIs
  or Bochs mechanics, preserving source result/failure contracts. It must not
  import any kernel source body.

## Implementation consequence

The static-closure task may select **two narrowly named adapter fragments**:

1. `adapter-opennt-host` capture-layout fragment from `csrutil.c` for the Base
   VDM client subset; and
2. a `broker` wait-order fragment from `wait.c` only if multi-caller
   `GetNextVDMCommand`/WOW notification is admitted.

Both need their own declaration, changed-layout, mapping and failure rows
before code is copied. No third generic compatibility component is justified.

## Evidence

- `O:\repos.external\opennt\base\ntdll\csrutil.c:25-238,267-723`
- `O:\repos.external\opennt\base\subsys\csr\server\wait.c:26-253`
- `O:\repos.external\opennt\base\subsys\csr\server\process.c:184-500,943-1504`
- `O:\repos.external\opennt\base\ntos\vdm\vdm.c:49-498`
- `O:\repos.external\opennt\base\ntos\vdm\x86\vdmentry.c:51-136`
