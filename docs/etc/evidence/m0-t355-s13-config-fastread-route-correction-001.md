# M0 T355 S13 — CONFIG fast-read route correction

## Question

Does the absence of the S8 `demRead` observer prove that original CONFIG
processing never reached DOS read, or does the selected CPU40/x86 guest use a
different original read route?

## Inputs

- The immutable T355 S8/S10/S12 fixed-container evidence and its BOP dispatch
  record, which includes `50:42` after the successful temporary-file open,
  seek and temporary-CDS activity.
- Original NTDOS `doskrnl/dos/handle.asm::$READ` and `FastOrSlow`.
- Selected MVDM mirror `dos/dem/demdisp.c` and `demhndl.c`.
- Original OpenNT kernel VDM `base/ntos/vdm/x86/rdwr.c::NTFastDOSIO`.

## Original route

`sysconf.asm::doconf` invokes ordinary DOS `AH=3Fh` after its temporary-CDS
work.  In `handle.asm::$READ`, a non-pipe SFT with the selected
`MIPS_BIT_MASK` clear takes this route:

```text
FastOrSlow -> SVC_DEMFASTREAD (50:42) -> CF clear: dor2
                                         CF set:   HRDSVC DEMREAD (50:16)
```

Therefore, the absence of a `demRead` (`50:16`) observation does **not** show
that DOS read was not dispatched.  It only shows that the carry-set slow-read
fallback was not taken.  The existing BOP dispatch record's `50:42` is direct
positive evidence that the read-side fast route was reached.

The apparent contradiction in the original user-mode DEM table is intentional.
`demdisp.c` maps `SVC_DEMFASTREAD` to `demNotYetImplemented`, whose visible
body clears carry.  On the original NT4 x86 product that entry is bypassed by
the kernel VDM handler `rdwr.c::NTFastDOSIO`; the source comment states that
the BOP is handled in kernel for performance.  For normal files it accepts the
same DOS ABI as `demRead`:

- `AX:BP`: host file handle;
- `DS:DX`: DOS buffer;
- `CX`: byte count;
- `BX:SI` plus ZF: optional current file position.

It performs synchronous native file I/O, returns the completed byte count in
low `AX` with carry clear, returns EOF as zero bytes with carry clear, and
sets carry for error or console/standard-handle cases.  The original kernel
body directly uses `PVDM_TIB`, trap-frame registers and `NtReadFile`; it
cannot be linked into the non-invasive modern user-mode product unchanged.

## Current binding and correction

The selected CPU40/x86 product has no NT4 kernel VDM interceptor.  Its
`MS_bop_0 -> DemDispatch` route therefore reaches the visible table entry.
That entry currently clears carry without transferring bytes or defining the
returned byte count.  It makes `$READ` take `dor2` instead of its original
carry-set `DEMREAD` fallback.  This is a source-owned missing kernel-fast-I/O
composition, not a `tempcds`, drive-IOCTL, parser or guest-only continuation
failure.

Consequently, these predecessor records remain valid bounded facts but their
former inference is superseded:

- S8 correctly established that `demRead` itself was not reached; it did not
  establish that DOS did not issue a read.
- S10 correctly established successful EOF seek.
- S12 correctly established successful temporary-CDS removable-media IOCTL.
- None of the three proves or disproves the `tempcds` return independently;
  the observed `50:42` instead identifies the next directly reached owner.

## Selected successor

The bounded successor is **M0 T355 S14 — original kernel FastRead composition
recovery**.  It must recover the normal-file `50:42` contract from `rdwr.c`
without importing kernel VDM.  It will preserve the original service ABI and
carry/AX/EOF rules, resolve the opaque host resource through the session host
resource mapping manager, and use a synchronous checked guest-memory lease
for the `DS:DX` transfer.  Console, standard-handle, pipe, private
`PVDM_TIB`, trap-frame, IRQL and native-kernel mechanics remain outside the
user-mode closure and must retain a source-shaped carry/fallback disposition.

No observer, runtime run, guest mutation or provider implementation is part
of S13.

## Confidence

High.  The guest source supplies the carry-controlled branch, the durable BOP
record supplies `50:42`, the current table supplies the erroneous CF-clear
user-mode outcome, and the original kernel source explains why that table
entry is not the complete NT4 product behavior.
