# M0 T277 S6 — Redirector and WOW source-form boundary plan

S6 closes the 17 work items left after the S3–S5 package reviews: five
original `vdmredir` callers and twelve original `wow32` callers.

- Redirector `getenv` stays a CRT binding. `_inlineConvertAddress` must use
  the session mapping manager rather than packed host pointers; `DbgOut`
  belongs to the original Redirector debug facility.
- WOW `host_simulate` reuses S3's single Bochs mechanical boundary;
  debugger notifications name `adapter-debugger`; MCI debug calls remain WOW
  provider-local; `W32Dispatch` names `adapter-wow`; and serial shadow/handle
  calls retain the original `nt_com` shape through session resource mapping.

No provider, protocol, pipe, process launch, WOW bridge, debugger, serial
endpoint or machine path is enabled. The resulting ledger is source-form
closure evidence for the 325-row future-host worklist only.
