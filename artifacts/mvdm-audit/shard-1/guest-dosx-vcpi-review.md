# Shard 1: pending DOSX VCPI and related guest includes

All six scoped files were `待核验` in the current shard ledger and match both frozen OpenNT editions. The observer also verifies both original editions of the DOSX makefile, two link responses and cited guest consumers. Four existing host Ninja VALID dependency logs are searched after resolving each path against its build directory; none contains these six exact guest paths. All file/graph/package hashes are in the JSON.

| File | Original role and checked selection evidence |
| --- | --- |
| `dpmi/dxvcpi.asm` | Resident guest VCPI detection, maintenance and cleanup code, enclosed by `if VCPI`. The original makefile places its object in `VCPIOBJ` under `!if 0`, and neither checked DOSX response lists it. This shows a nonselected role in this recipe, not absence from every historical VCPI build. |
| `dpmi/dxvcpibt.asm` | Initialization-time VCPI setup and detection source, also enclosed by `if VCPI`; original comments describe its discardable startup lifetime. Its object has the same disabled `VCPIOBJ` status and is absent from the two checked link responses. |
| `dpmi/dxvcpi.inc` | Shared VCPI data and definitions. It is included by original 486 `dxboot.asm` and `dxstrt.asm`, both of whose objects are in the two DOSX link responses. It cannot be classified unused merely because dedicated VCPI objects are omitted; preprocessed active declarations are not established here. |
| `dpmi/netbios.inc` | NetBIOS declarations directly included by `dxnetbio.asm`; `dxnetbio.obj` is in both checked link responses. This is original source/recipe evidence, not proof guest network operations work. |
| `dpmi/segdefs.inc` | Guest segment declarations directly included by the checked 486 `dxboot.asm` and `dxstrt.asm`; both objects appear in the link responses. Actual macro expansion and guest layout still need separate evidence. |
| `dpmi/smartdrv.inc` | Original SMARTDRV-related declarations directly included by those same two selected source files; actual runtime cache/driver use is not established. |

The original makefile defines `VCPI = -DVCPI=1` but the checked makefile does not reference `$(VCPI)` in its assembler commands; `gendefs.inc` defaults VCPI to zero when not predefined. This supports the stated default recipe boundary, not an assertion about external macro overrides or a preprocessed current guest.

The source-tree `dpmi/486/DOSX.EXE`, exact installed `O:/winnt/system32/DOSX.EXE` and `WINNT/SYSTEM32/DOSX.EXE` in `assets/winnt.zip` have the same SHA-256 `C5AF29A29ABF167B243DAABF877459E8278B8C9A339BF8E1E2576EAD5F6CEEFF` (see JSON for the authoritative unformatted value). Package identity does not prove that the retained response built this binary, that these declarations were active, or that VCPI, NetBIOS or SMARTDRV behavior passed.

Replay `python artifacts/mvdm-audit/shard-1/guest-dosx-vcpi-observer.py`. No product/guest was executed or modified; no compiler, build tree or shared ledger was changed. Keep runtime and alternate-profile boundaries explicit.
