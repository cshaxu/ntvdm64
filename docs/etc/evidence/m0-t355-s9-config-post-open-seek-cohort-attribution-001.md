# M0 T355 S9 — CONFIG post-open seek/pre-read cohort attribution

## S8 input

S8 observed a successful original DEM open of the generated temporary CONFIG
file and no `demRead` entry.  That bounded the unresolved original path to the
instructions between `doconf` open and DOS read; it did not prove a failed or
short read.

## Original source walk

`mvdm-softpc-firmware/softpc.new/bios/sysconf.asm::doconf` performs the
following exact sequence after successful INT 21h open:

1. saves returned handle in `BX`;
2. invokes INT 21h `AH=42h`, `AL=2`, `CX:DX=0` to seek to EOF and stores the
   returned `AX` in `count`;
3. invokes INT 21h `AH=42h`, `AL=0`, `DX=0` to reset the file pointer;
4. computes and relocates `confbot`, including `pararound` and `tempcds`;
5. sets `DS:ES=confbot`, `CX=count`, and invokes INT 21h `AH=3Fh` read.

The original DOS `doskrnl/dos/handle.asm::LSeekDisp` routes only the EOF form
(`AL=2`) through `LSeekEOF`, which extracts the SFT NT handle and invokes
`HRDSVC SVC_DEMCHGFILEPTR`.  The reset-to-start form (`AL=0`) is a guest-local
`LSeekStore` update and does not call the DEM provider.  The host owner is
therefore exactly `dos/dem/demdisp.c::SVC_DEMCHGFILEPTR` →
`dos/dem/demhndl.c::demChgFilePtr` → public `SetFilePointer`.

## Result and next seam

The only immediate cross-host predecessor of the absent read is the original
EOF file-pointer transaction.  Its source ABI is:

```text
entry: AX:BP = opaque host file handle; CX:DX = offset; BL = origin (2)
exit success: CF=0; DX:AX = absolute position
exit failure: CF=1; AX = original client error
```

`demChgFilePtr` already retains this original provider/body and public Win32
binding.  S9 does not claim that it succeeds at runtime.  The next bounded
work is a default-off scalar observation of this one original service in the
same container, followed by its source-defined continuation; no parser,
guest, media, CPU, or DEM semantic modification is selected.
