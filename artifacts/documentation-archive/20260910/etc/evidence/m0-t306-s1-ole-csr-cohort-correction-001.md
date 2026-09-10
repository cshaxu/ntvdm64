# M0 T306 S1 P5 — OLE false candidate and CSR hard-boundary correction

## Question

Are the two `ReadByte` source candidates really OLEAUT dependencies, and what
is the exact modern product disposition of the one reached CSR function?

## Inputs

- The two `ReadByte` and one `CsrIdentifyAlertableThread` rows in
  [the signature-seed package ledger](../operations/m0-t306-s1-signature-seed-package-ledger.tsv).
- `mvdm-host/bde/bde.h:27`, `bde/bdeutil.c:300`,
  `vdmexts/vdmexts.h:43`, and `vdmexts/util.c:315`.
- Candidate source `com/oleaut32/typelib/fstream.cxx:130` and
  `base/ntdll/csrtask.c:51`.
- The direct WOW call site `mvdm-host/wow32/wow32.c:1450`.

## Observation

The two MVDM `ReadByte` call forms are macros, not calls to
`STREAM::ReadByte(BYTE *)`: their headers expand each use to the local,
two-argument `read_byte(ULONG, BOOL)` functions. The earlier OLEAUT candidate
was selected by spelling after the physical signature gate but before macro
expansion. The receiver, parameter count and owner all differ, so importing
OLEAUT would be an unrelated and incorrect dependency.

`CsrIdentifyAlertableThread` is an actual direct WOW call. Its original NTDLL
body creates a CSR message, takes `NtCurrentTeb()->ClientId` and performs
`CsrClientCallServer` using `CsrPortHandle`; it is not a public Win32 API and
cannot become a local function without recreating the CSRSS registration
product shell.

## Decision

Return both `ReadByte` rows to the selected MVDM closure and select no OLE
package. Keep the immutable P2 ledger as input evidence, but use the companion
[correction ledger](../operations/m0-t306-s1-ole-csr-cohort-ledger.tsv) as the
current final disposition.

Classify `CsrIdentifyAlertableThread` as a non-invasive NTDLL CSR hard
boundary. The future monitor/debugger/WOW owner may preserve the source
call-site failure/branch semantics through a named adapter, but it must not
import the NTDLL CSR transport or claim that current public Win32 reproduces
alertable-thread registration.

## Follow-up

The package-selection algorithm must apply macro/arity/receiver checks before
promoting a same-spelling source definition to a non-MVDM import candidate.
This correction removes the only OLE cohort from T306's proposed adoption
worklist.
