# Shard 3: BIOS time and environment helper

Two prior pending physical source roles, independently reviewed. The
[observer](support-clock-env-observer.py) and [observations](support-clock-env-observations.json)
bind both files to both original editions and audit catalogs, the real Ninja
objects and VALID dependencies, archive membership and current release image.
Both retain 100% original source bytes; no behavioral percentage follows.

| Original file | Proposed disposition | Actual owner chain and boundary |
| --- | --- | --- |
| `softpc.new/base/support/time_day.c` | **partial** | `bios.c` places the original `time_of_day` at BOP 1Ah. The observer checks the actual released BIOS vector relocation and pointer, and three non-relocated object bodies (`time_of_day`, `time_of_day_init`, `UpDateInProgress`) against the image. In NTVDM, AH=0 reads the BIOS tick and overflow values and clears overflow; AH=1 writes the ticks. Other RTC operations call the original CMOS owner. `UpDateInProgress` polls Reg A with `Sleep(0)`. Source identity and static dispatch are not proofs of clock progression, rollover, BCD/date semantics, concurrent CMOS transitions, alarm delivery, error/CF behavior or worker reset. |
| `softpc.new/base/support/get_env.c` | **excluded historical helper role** | The object is present in the static archive but none of its three functions is mapped in this released image. Whole-source search finds their sole other implementation call in `ccpupig.c`, within `#ifdef PIG`; the selected `NTVDM/PROD` graph has PIG disabled. This conclusion only excludes this debug/helper role from the selected product, not environment-variable handling: `host_def.h` supplies `host_getenv`, `host/src/config.c` uses it, and the project CRT boundary maps `getenv` to `mvdm_host_getenv`. Guest environment inheritance and errors remain independently open. |

The original `reset.c` invokes `time_of_day_init`; in the selected NTVDM
branch that function has no body. This is an original conditional branch, not
evidence that the BIOS data-area timer is initialized elsewhere: the separate
`cmos_bis.c` and timer producers need their own traced initialization and
lifecycle proof. Prior PIC-reset and RTC device probes do not independently
prove BOP 1Ah execution. Neither guest nor product images were modified.
