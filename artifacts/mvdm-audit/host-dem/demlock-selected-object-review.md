# DEM lock selected-object failure and lifecycle probe

The guest DOS source `dos/v86/doskrnl/dos/lock.asm` checks the requested operation, translates the DOS handle to its SFT host handle and forwards `SVC_DEMLOCKOPER`; `demdisp.c` dispatches it to `demLockOper`. The latter combines BX:BP into the Windows file handle, CX:DX into the offset, SI:DI into the length, then delegates lock/unlock to Windows. It clears guest CF on success or calls `demClientError` on failure. The guest owns DOS handle validity, the host OS owns file and lock lifetime, and DEM is responsible for translating arguments and forwarding errors. The guest-source trace alone does not prove source-to-installed-guest provenance.

Current `src/mvdm/dos/dem/demlock.c` and OpenNT's version both SHA-256 `7716AB3BE08F3532A60E3B05B2214BF1F8CB9F66F362B5F7F555DC3CD2755A71`. A dedicated x86 executable linked the existing selected object `build/.ninja/msvc-x86-release/worker/obj/dem/demlock.obj` SHA-256 `AD3E46A85624561D249A0FA31B475C8F34781A817A1B4973FBFBC3A6005BFD48` with `demlock-selected-object-fixture.c`; MinGW i686 GCC `-Wl,--gc-sections` reported `Warning: corrupt .drectve at end of def file` but returned success. The source fixture is SHA-256 `480FA993A421CD2908EC4C3870066B3B3A41B9B02216C29CC9FF5385362C3474`.

Two independent handles to one disposable temporary file were opened with sharing. The first successfully locked one byte; the second failed with a lock violation. The first unlocked, after which the second could lock and unlock; an invalid-handle lock also reached the error seam. The executable exited 0 and printed:

```text
competing lock cf=1 error=33 calls=1
invalid handle cf=1 error=6 calls=2
```

Win32 locking was real. Register accessors and `demClientError` were fixture seams; the seam records `GetLastError` and sets CF rather than exercising original DEM error translation. This confirms selected-object argument flow and two failure calls with same-process independent handles, not guest execution, cross-process contention, original AX mapping, lock cleanup on guest termination, or binding of this object to the installed release. Existing historical fixture and guest-source review are separate evidence in `nine-way/part-8/dem-lock-guest-host-review.md`. No product, guest image, or shared ledger was changed. Keep remaining lifecycle and integration requirements open.
