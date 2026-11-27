# S12 P10 five-file checkpoint audit

Files26-30 are reviewed in canonical DIR order. All175frozen points have a
conclusion:149restored,26evidence-backed residuals remain for owner decision.
Final runtime11 Full23/23 passed; see s12-p10-result.json. Commit/push pending.

| File | Before nonerror points/lines | After | Restored frozen points | Residual |
| --- | ---: | ---: | ---: | ---: |
| config.c | 1/1983 | 19/20 | 37 | 1 |
| nt_bop.c | 1/1795 | 22/44 | 1 | 15 |
| nt_com.c | 114/287 | 7/8 | 89 | 1 |
| nt_cpu.c | 1/543 | 9/11 | 0 | 2 |
| nt_emm.c | 1/1674 | 17/70 | 22 | 7 |
| Total | 118/6282 | 74/153 | 149 | 26 |

S12 current census671/19668;reduction132points/12141lines from803/31809.
Whole-file CR/format recovery exposes distributed real differences; this is
the owner's approved formatting exception, not new behavior. All five C token
sequences and original exclusion counts are unchanged. Existing exclusions
remain23/32/9/10/5 respectively. No original guest was modified or rebuilt.

## Residual ownership and original restoration boundaries

- config: the one wow_registry.h include binds GetVDMSize(size/wowsize) and
  LPT configuration reads to the package NTVDM.REG provider in B. Deleting
  it changes to host HKLM. No original configuration algorithm is copied.
- BOP: the worker calls an actual /Gz WOW DLL; W32Init(BOOL) must use WINAPI.
  FARPROC and MYFARPROC retain distinct declared export boundaries. WOW's
  existing B page-domain provider supplies CR3/PG/client-desktop/GDT context,
  then original W32Init/dispatch runs. COMMAND's B ingress synchronously
  enters the registered VM callback only for V86 services8/10; no queue added.
- BOP memory: sas_load reaches bios_read_byte, which checks CPL3 user access,
  V86 access bits and invokes BIOS guest handling on translation failure.
  VdmMapFlat instead uses supervisor translation and returns NULL on failure;
  an unchecked raw dereference is not the same contract. V86 also has PE=1,
  requiring !VM before treating a pointer as a protected selector. HMODULE
  cleanup uses FreeLibrary rather than CloseHandle.
- COM: host_com_ioctl's arg is long; sprintf uses %ld. All other frozen
  differences are original formatting; no serial queue or retry change.
- CPU: selected generated/x86/prod/gdpvar.h offsets are560+3 and3096+3,
  not original631/4355. Named GLOBAL_InNanoCpu/GLOBAL_stackIsBig lvalues
  follow the current selected layout rather than adding another offset table.
- EMS: an alias span can consist of reversed host pages. Existing
  test/mvdm-host/original_external_memory_test.c creates that exact case and
  calls real host_copy_con_to_con both directions. Direct contiguous memcpy
  cannot cross it correctly. Existing three em_* adapters preserve the
  original contiguous path and delegate aliased spans to the owning guest
  copy/move; failures propagate and exchange temporaries are freed. No new
  memory algorithm, adapter, queue or state machine was introduced.

## Verification status

Cached full build25776 passed. Actual per-TU product flags produced503unique
original logical-line witnesses,zero drift. The first checker wrongly used
Python splitlines for the original nt_bop.c containing two form-feed bytes;
compiler lines are LF-based. Corrected checker only,kept initial failure under
build/t485-s12/p10/final/initial-*;no product rebuild. Runtime11 was staged and
hash-verified at both release locations. Full owner17924 passed all23gates; no retry or repeated passing gate. No full rerun is authorized
for unchanged code; retry only failed gates and execute outstanding gates once.

## Frozen dispositions

Scale below is the frozen point's original added/deleted size, not the current
file's aggregate difference. Details and source chains are synchronized in
points.csv and pending-decisions.csv. Final verification is complete; commit/push pending.

| Point | File | XP line | Frozen scale | Disposition |
| --- | --- | ---: | --- | --- |
| HXP-R-03184 | config.c | 26 | +1/-0 | residual-owner-decision |
| HXP-R-03185 | config.c | 123 | +2/-2 | restored-local |
| HXP-R-03186 | config.c | 131 | +2/-2 | restored-local |
| HXP-R-03187 | config.c | 141 | +1/-1 | restored-local |
| HXP-R-03188 | config.c | 165 | +1/-1 | restored-local |
| HXP-R-03189 | config.c | 169 | +1/-1 | restored-local |
| HXP-R-03190 | config.c | 176 | +1/-1 | restored-local |
| HXP-R-03191 | config.c | 232 | +9/-9 | restored-local |
| HXP-R-03192 | config.c | 242 | +2/-2 | restored-local |
| HXP-R-03193 | config.c | 247 | +1/-1 | restored-local |
| HXP-R-03194 | config.c | 249 | +1/-1 | restored-local |
| HXP-R-03195 | config.c | 314 | +2/-2 | restored-local |
| HXP-R-03196 | config.c | 317 | +2/-2 | restored-local |
| HXP-R-03197 | config.c | 601 | +2/-2 | restored-local |
| HXP-R-03198 | config.c | 619 | +1/-1 | restored-local |
| HXP-R-03199 | config.c | 622 | +1/-1 | restored-local |
| HXP-R-03200 | config.c | 640 | +9/-9 | restored-local |
| HXP-R-03201 | config.c | 650 | +23/-23 | restored-local |
| HXP-R-03202 | config.c | 681 | +1/-1 | restored-local |
| HXP-R-03203 | config.c | 685 | +1/-1 | restored-local |
| HXP-R-03204 | config.c | 728 | +3/-3 | restored-local |
| HXP-R-03205 | config.c | 771 | +1/-1 | restored-local |
| HXP-R-03206 | config.c | 774 | +1/-1 | restored-local |
| HXP-R-03207 | config.c | 778 | +2/-2 | restored-local |
| HXP-R-03208 | config.c | 783 | +3/-3 | restored-local |
| HXP-R-03209 | config.c | 790 | +2/-2 | restored-local |
| HXP-R-03210 | config.c | 813 | +1/-1 | restored-local |
| HXP-R-03211 | config.c | 815 | +10/-10 | restored-local |
| HXP-R-03212 | config.c | 834 | +1/-1 | restored-local |
| HXP-R-03213 | config.c | 923 | +1/-1 | restored-local |
| HXP-R-03214 | config.c | 930 | +4/-4 | restored-local |
| HXP-R-03215 | config.c | 996 | +1/-1 | restored-local |
| HXP-R-03216 | config.c | 1000 | +2/-2 | restored-local |
| HXP-R-03217 | config.c | 1043 | +1/-1 | restored-local |
| HXP-R-03218 | config.c | 1057 | +9/-9 | restored-local |
| HXP-R-03219 | config.c | 1485 | +3/-3 | restored-local |
| HXP-R-03220 | config.c | 1489 | +1/-1 | restored-local |
| HXP-R-03221 | config.c | 1498 | +4/-4 | restored-local |
| HXP-R-03223 | nt_bop.c | 73 | +1/-0 | residual-owner-decision |
| HXP-R-03224 | nt_bop.c | 105 | +1/-1 | residual-owner-decision |
| HXP-R-03225 | nt_bop.c | 173 | +2/-2 | residual-owner-decision |
| HXP-R-03226 | nt_bop.c | 181 | +4/-0 | residual-owner-decision |
| HXP-R-03227 | nt_bop.c | 203 | +1/-1 | residual-owner-decision |
| HXP-R-03228 | nt_bop.c | 257 | +1/-1 | residual-owner-decision |
| HXP-R-03229 | nt_bop.c | 292 | +1/-1 | restored-local |
| HXP-R-03230 | nt_bop.c | 332 | +1/-1 | residual-owner-decision |
| HXP-R-03231 | nt_bop.c | 334 | +1/-0 | residual-owner-decision |
| HXP-R-03232 | nt_bop.c | 335 | +1/-1 | residual-owner-decision |
| HXP-R-03233 | nt_bop.c | 337 | +1/-0 | residual-owner-decision |
| HXP-R-03234 | nt_bop.c | 466 | +1/-1 | residual-owner-decision |
| HXP-R-03235 | nt_bop.c | 583 | +2/-1 | residual-owner-decision |
| HXP-R-03236 | nt_bop.c | 607 | +1/-1 | residual-owner-decision |
| HXP-R-03237 | nt_bop.c | 893 | +1/-1 | residual-owner-decision |
| HXP-R-03238 | nt_bop.c | 912 | +1/-1 | residual-owner-decision |
| HXP-R-03249 | nt_com.c | 12 | +2/-2 | restored-local |
| HXP-R-03250 | nt_com.c | 15 | +1/-1 | restored-local |
| HXP-R-03251 | nt_com.c | 62 | +1/-1 | restored-local |
| HXP-R-03252 | nt_com.c | 82 | +1/-1 | restored-local |
| HXP-R-03253 | nt_com.c | 99 | +1/-1 | restored-local |
| HXP-R-03254 | nt_com.c | 102 | +3/-3 | restored-local |
| HXP-R-03255 | nt_com.c | 117 | +2/-2 | restored-local |
| HXP-R-03256 | nt_com.c | 120 | +4/-4 | restored-local |
| HXP-R-03257 | nt_com.c | 125 | +2/-2 | restored-local |
| HXP-R-03258 | nt_com.c | 130 | +1/-1 | restored-local |
| HXP-R-03259 | nt_com.c | 133 | +6/-6 | restored-local |
| HXP-R-03260 | nt_com.c | 151 | +3/-3 | restored-local |
| HXP-R-03261 | nt_com.c | 155 | +1/-1 | restored-local |
| HXP-R-03262 | nt_com.c | 157 | +1/-1 | restored-local |
| HXP-R-03263 | nt_com.c | 161 | +1/-1 | restored-local |
| HXP-R-03264 | nt_com.c | 165 | +1/-1 | restored-local |
| HXP-R-03265 | nt_com.c | 167 | +1/-1 | restored-local |
| HXP-R-03266 | nt_com.c | 171 | +1/-1 | restored-local |
| HXP-R-03267 | nt_com.c | 176 | +2/-2 | restored-local |
| HXP-R-03268 | nt_com.c | 179 | +2/-2 | restored-local |
| HXP-R-03269 | nt_com.c | 184 | +2/-2 | restored-local |
| HXP-R-03270 | nt_com.c | 187 | +1/-1 | restored-local |
| HXP-R-03271 | nt_com.c | 191 | +2/-2 | restored-local |
| HXP-R-03272 | nt_com.c | 240 | +4/-4 | restored-local |
| HXP-R-03273 | nt_com.c | 245 | +2/-2 | restored-local |
| HXP-R-03274 | nt_com.c | 298 | +1/-1 | restored-local |
| HXP-R-03275 | nt_com.c | 300 | +1/-1 | restored-local |
| HXP-R-03276 | nt_com.c | 304 | +1/-1 | restored-local |
| HXP-R-03277 | nt_com.c | 309 | +1/-1 | restored-local |
| HXP-R-03278 | nt_com.c | 314 | +1/-1 | restored-local |
| HXP-R-03279 | nt_com.c | 325 | +2/-2 | restored-local |
| HXP-R-03280 | nt_com.c | 350 | +2/-2 | restored-local |
| HXP-R-03281 | nt_com.c | 414 | +1/-1 | restored-local |
| HXP-R-03282 | nt_com.c | 417 | +2/-2 | restored-local |
| HXP-R-03283 | nt_com.c | 446 | +1/-1 | restored-local |
| HXP-R-03284 | nt_com.c | 461 | +3/-3 | restored-local |
| HXP-R-03285 | nt_com.c | 475 | +1/-1 | restored-local |
| HXP-R-03286 | nt_com.c | 480 | +1/-1 | restored-local |
| HXP-R-03287 | nt_com.c | 482 | +1/-1 | restored-local |
| HXP-R-03288 | nt_com.c | 490 | +1/-1 | restored-local |
| HXP-R-03289 | nt_com.c | 503 | +1/-1 | restored-local |
| HXP-R-03290 | nt_com.c | 514 | +1/-1 | restored-local |
| HXP-R-03291 | nt_com.c | 525 | +1/-1 | restored-local |
| HXP-R-03292 | nt_com.c | 541 | +1/-1 | restored-local |
| HXP-R-03293 | nt_com.c | 546 | +5/-5 | restored-local |
| HXP-R-03294 | nt_com.c | 553 | +2/-2 | restored-local |
| HXP-R-03295 | nt_com.c | 591 | +1/-1 | restored-local |
| HXP-R-03296 | nt_com.c | 595 | +1/-1 | restored-local |
| HXP-R-03297 | nt_com.c | 599 | +1/-1 | restored-local |
| HXP-R-03298 | nt_com.c | 607 | +1/-1 | restored-local |
| HXP-R-03299 | nt_com.c | 628 | +1/-1 | restored-local |
| HXP-R-03300 | nt_com.c | 660 | +2/-2 | restored-local |
| HXP-R-03301 | nt_com.c | 712 | +1/-1 | restored-local |
| HXP-R-03302 | nt_com.c | 778 | +1/-1 | restored-local |
| HXP-R-03303 | nt_com.c | 788 | +1/-1 | restored-local |
| HXP-R-03304 | nt_com.c | 797 | +2/-2 | restored-local |
| HXP-R-03305 | nt_com.c | 800 | +1/-1 | restored-local |
| HXP-R-03306 | nt_com.c | 816 | +1/-1 | restored-local |
| HXP-R-03307 | nt_com.c | 863 | +1/-1 | restored-local |
| HXP-R-03308 | nt_com.c | 873 | +1/-1 | restored-local |
| HXP-R-03309 | nt_com.c | 905 | +6/-6 | restored-local |
| HXP-R-03310 | nt_com.c | 922 | +1/-1 | restored-local |
| HXP-R-03311 | nt_com.c | 928 | +1/-1 | restored-local |
| HXP-R-03312 | nt_com.c | 947 | +1/-1 | restored-local |
| HXP-R-03313 | nt_com.c | 976 | +1/-1 | restored-local |
| HXP-R-03314 | nt_com.c | 981 | +1/-1 | restored-local |
| HXP-R-03315 | nt_com.c | 992 | +1/-1 | residual-owner-decision |
| HXP-R-03316 | nt_com.c | 1001 | +1/-1 | restored-local |
| HXP-R-03317 | nt_com.c | 1008 | +1/-1 | restored-local |
| HXP-R-03318 | nt_com.c | 1017 | +1/-1 | restored-local |
| HXP-R-03319 | nt_com.c | 1023 | +1/-1 | restored-local |
| HXP-R-03320 | nt_com.c | 1040 | +1/-1 | restored-local |
| HXP-R-03321 | nt_com.c | 1045 | +1/-1 | restored-local |
| HXP-R-03322 | nt_com.c | 1080 | +1/-1 | restored-local |
| HXP-R-03323 | nt_com.c | 1245 | +1/-1 | restored-local |
| HXP-R-03324 | nt_com.c | 1279 | +1/-1 | restored-local |
| HXP-R-03325 | nt_com.c | 1327 | +2/-2 | restored-local |
| HXP-R-03326 | nt_com.c | 1331 | +1/-1 | restored-local |
| HXP-R-03327 | nt_com.c | 1333 | +1/-1 | restored-local |
| HXP-R-03328 | nt_com.c | 1380 | +1/-1 | restored-local |
| HXP-R-03329 | nt_com.c | 1399 | +1/-1 | restored-local |
| HXP-R-03330 | nt_com.c | 1444 | +1/-1 | restored-local |
| HXP-R-03331 | nt_com.c | 1517 | +1/-1 | restored-local |
| HXP-R-03332 | nt_com.c | 1536 | +1/-1 | restored-local |
| HXP-R-03333 | nt_com.c | 1548 | +1/-1 | restored-local |
| HXP-R-03334 | nt_com.c | 1571 | +1/-1 | restored-local |
| HXP-R-03335 | nt_com.c | 1603 | +1/-1 | restored-local |
| HXP-R-03336 | nt_com.c | 1626 | +1/-1 | restored-local |
| HXP-R-03337 | nt_com.c | 1779 | +1/-1 | restored-local |
| HXP-R-03338 | nt_com.c | 1931 | +1/-1 | restored-local |
| HXP-R-03339 | nt_cpu.c | 281 | +1/-1 | residual-owner-decision |
| HXP-R-03340 | nt_cpu.c | 299 | +1/-1 | residual-owner-decision |
| HXP-R-03350 | nt_emm.c | 60 | +2/-0 | residual-owner-decision |
| HXP-R-03351 | nt_emm.c | 81 | +1/-1 | restored-local |
| HXP-R-03352 | nt_emm.c | 128 | +32/-0 | residual-owner-decision |
| HXP-R-03353 | nt_emm.c | 135 | +1/-1 | restored-local |
| HXP-R-03354 | nt_emm.c | 196 | +1/-1 | restored-local |
| HXP-R-03355 | nt_emm.c | 232 | +1/-1 | restored-local |
| HXP-R-03356 | nt_emm.c | 243 | +1/-1 | restored-local |
| HXP-R-03357 | nt_emm.c | 263 | +1/-1 | restored-local |
| HXP-R-03358 | nt_emm.c | 309 | +1/-1 | restored-local |
| HXP-R-03359 | nt_emm.c | 331 | +1/-1 | restored-local |
| HXP-R-03360 | nt_emm.c | 341 | +1/-1 | restored-local |
| HXP-R-03361 | nt_emm.c | 395 | +1/-1 | restored-local |
| HXP-R-03362 | nt_emm.c | 435 | +1/-1 | restored-local |
| HXP-R-03363 | nt_emm.c | 504 | +1/-1 | restored-local |
| HXP-R-03364 | nt_emm.c | 567 | +1/-1 | restored-local |
| HXP-R-03365 | nt_emm.c | 572 | +1/-1 | restored-local |
| HXP-R-03366 | nt_emm.c | 580 | +1/-1 | restored-local |
| HXP-R-03367 | nt_emm.c | 594 | +2/-1 | residual-owner-decision |
| HXP-R-03368 | nt_emm.c | 603 | +1/-1 | restored-local |
| HXP-R-03369 | nt_emm.c | 618 | +2/-1 | residual-owner-decision |
| HXP-R-03370 | nt_emm.c | 627 | +1/-1 | restored-local |
| HXP-R-03371 | nt_emm.c | 642 | +2/-1 | residual-owner-decision |
| HXP-R-03372 | nt_emm.c | 651 | +1/-1 | restored-local |
| HXP-R-03373 | nt_emm.c | 682 | +1/-1 | restored-local |
| HXP-R-03374 | nt_emm.c | 694 | +1/-1 | restored-local |
| HXP-R-03375 | nt_emm.c | 714 | +6/-3 | residual-owner-decision |
| HXP-R-03376 | nt_emm.c | 727 | +1/-1 | restored-local |
| HXP-R-03377 | nt_emm.c | 747 | +5/-2 | residual-owner-decision |
| HXP-R-03378 | nt_emm.c | 760 | +1/-1 | restored-local |
