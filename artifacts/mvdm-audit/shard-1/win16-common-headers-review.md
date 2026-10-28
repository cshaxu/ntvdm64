# S10 shard 1: Win16 common C header family

Nine pending `wow16/inc` headers were grouped by the original guest build interface. All match both primary OpenNT reference trees by SHA-256. Per-file complete hashes and bounded direct basename includes are in `win16-common-headers-evidence.json`, reproduced with `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/shard-1/win16-common-headers-observer.py`. A direct basename include alone does not prove this exact header resolves: same-name local headers, compiler include order and conditional preprocessing matter. This scan is limited to current WOW16 C/H/INC/CPP; no guest compile, runtime or new Ninja dep scan was performed.

| File | Actual association and remaining boundary |
| --- | --- |
| commdlg.h | Common-dialog API/types; 13 basename include sites, some have local shadows. Prior `shared-guest-include-followup.md` positively traces WRITE's selected commdlg.obj to this exact header; it does not prove every common-dialog operation. |
| drivinit.h | Explicitly marked obsolete and includes print.h; basename sites metarec.c and WRITE diachgpr.c. Prior follow-up notes these objects not selected by reviewed respective original object lists; alternate build consumers remain pending. |
| io.h | CRT low-level I/O declarations; two basename includes. Prior follow-up resolves selected DRWATSON drwatson.c through original -I../inc; guest execution not proven. |
| stdarg.h | CRT varargs macros; one basename include at DRWATSON drwatson.c, exact original include search resolution documented in prior follow-up. Does not prove actual debug functionality. |
| stdio.h | CRT standard I/O declarations; eleven basename includes, including selected DRWATSON getsym.c per prior follow-up. Other basename sites require their own path-resolution evidence. |
| style.h | Historical window style constants; zero direct includes in this bounded C-family scan. This does not mean windows have no styles or that other resource/asm/build variants omit it. |
| userproc.h | USER/drop-object declarations; zero direct includes in bounded scan. Actual guest API/drop behavior not tested. |
| w32sys.h | Win32s interface declarations; zero direct includes in bounded scan. Original KERNEL variants and related historical module audit are separate; not proof Win32s behavior exists or is absent. |
| wife.h | Far East font environment definitions; zero direct includes in bounded scan. Locale/font behavior and external consumers unverified. |

Prior `shared-guest-include-followup.md` and its evidence provide exact selected historical build chains where noted; negative results here do not override them or prove safe removal. No product, guest, shared ledger or build tree modified.
