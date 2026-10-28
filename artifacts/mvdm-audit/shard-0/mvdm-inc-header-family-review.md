# Shared MVDM declaration family and released owners

The [source/dependency observer](mvdm-inc-header-family-observer.py) and [receipt](mvdm-inc-header-family-observations.json) compare 17 original shared headers against both reference editions and record exact VALID x86 worker and WOW32 object dependencies. The [owner observer](mvdm-inc-header-owner-observer.py) and [receipt](mvdm-inc-header-owner-observations.json) bind one dependent object's function body per selected header to the delivered NTVDM, VDMREDIR or WOW32 image. It verifies installed and release hashes and records each object, component, function and relocation-masked body binding.

Fifteen declaration roles have that selected-object evidence: `apistruc.h`, `bop.h`, `dbgsvc.h`, `doswow.h`, `exterr.h`, `netb.h`, `sharewow.h`, `vrdlctab.h`, `vrmisc.h`, `vrmslot.h`, `vrnetapi.h`, `wowclip.h`, `wowcmdlg.h`, `wowinfo.h` and `wowth.h`. Broad precompiled-header dependencies are not evidence that every declaration is enabled or every target is called. These remain partial file roles; per-service dispatch, guest identity, resource ownership, error and task teardown remain open in S11–S17.

`wowddeml.h` has a textual include but no VALID dependency in the captured graphs, and `wowint.h` has neither; both stay pending. This does not exclude DDE or interrupts or other configurations. No product or guest bytes were modified.
