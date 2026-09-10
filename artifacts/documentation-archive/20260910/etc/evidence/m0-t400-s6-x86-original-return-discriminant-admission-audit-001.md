# M0 T400 S6 — x86 original-return discriminant admission audit

The existing Console observer ran against staged x86 `O:\t398\ntvdm32.exe` with
its external CPU40 map sampler and no product diagnostics. The retained report
is `O:\t398\t400-s6-cpu40-live-002.txt`. It preserved the ordinary low-memory
dialog, loaded WOW32, and recorded 276 readable CPU40 samples (268 coherent):

```text
CS=0x021f base=0x00008920 IP=0x0000aebc
SAS start=0x02f40000 length=0x00900000 CR0=0x00000011
```

This proves the x86 CPU40/SAS map reader operates externally without decoder
transport, but it cannot attribute that selector to WRITE. WRITE's map names
`FInitMemory` as logical `0005:0000`, `FCreateRgbp` as `0005:088c`, and
`FSetScreenConstants` as `0006:0102`; no current ordinary observer maps those
logical segments to the current task's LDT selectors. Treating `0x021f` as
segment 5 would therefore be selector guessing.

The existing VDMDBG fixture can query the module selector but creates an
isolated `DEBUG_ONLY_THIS_PROCESS` child and handles `STATUS_VDM_EVENT`; it is
not ordinary evidence. The exact-return boundary requires diagnostic-only
selector discovery followed separately by ordinary selector reading. No repair,
guest mutation, decoder transport, or x64 work is selected.
