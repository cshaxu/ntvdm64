# T478 S44 original window sizing investigation

Status: locally verified and closed; test/evidence-only delivery.

## Question and inputs

The inherited T473 handoff reports requested154x240 and observed237x240.
S43 independently observes restored237x240 but does not record its requested
size in the same run. Its fixed class-brush background does not prove sizing.
Baseline is487ce265c and package22e35db2-13bb-419d-ba68-7a169cc939e1.

## Procedure and observed source facts

Read the original OpenNT windows/core/ntuser/kernel/minmax.c and createw.c
from O:/repos.external/OpenNT, and current WOW message/creation bindings.
ISV_InitMinMaxInfo (minmax.c lines75-80) initializes captioned non-tool
windows from CXMINTRACK/CYMINTRACK. ISV_ValidateMinMaxInfo (lines128-200)
validates borders, caption buttons, icon and caption text space and takes
the maximum with the application-returned minimum. xxxAdjustSize
(createw.c lines1150-1189) calls this path for tiled OR sizeable windows,
then clamps requested size to the validated minimum/maximum.
Thus a non-resizable ordinary captioned window is not exempt.

Current WM32GetMinMaxInfo in src/mvdm-host/wow32/wmdisp32.c allocates five
POINT16 entries and converts both directions. The creation-menu callback
in wow_window_words_binding.c handles early menu association; its presence
is not evidence of a width override. No code change is justified yet.

## Initial source-only interpretation (superseded by runtime evidence below)

High confidence in the original minimum-size rule, not yet in the runtime
cause of237. A requested154 alone is insufficient to classify widening as a
bug. Next collect same-run requested/actual sizes, native system metrics and
WM_GETMINMAXINFO/WM_WINDOWPOSCHANGING values before and after guest handling.
Do not hardcode154, suppress valid native minimums, or claim a modern pixel
metric equals the historical metric. No new tests or product build in this
checkpoint; existing S43 release remains untouched.

## Same-run runtime finding

The diagnostic-only WOW32 build inserts read-only size observations around the
existing DispatchClientMessage calls and after native creation. The recipe is
`tests/observation/build-wow-sizing-diagnostic.ps1`; its two product-source
backups are restored byte-for-byte in finally. Its DLL is separately marked
non-publishable. The ordinary WOW32 cache was subsequently rebuilt successfully
from restored source; no production source or mirror difference is retained.

In geometry-trace.txt, WINMINE HWND01EA03B6 is created with request154x240,
style0x20CA0000 (including WS_MINIMIZE). Before a later WINDOWPOSCHANGING the
request is154x240; afterward it is237x240. Nested GETMINMAXINFO is136x39 on
both sides. Thus the conversion does not overwrite the minimum with237.
Restoring the window produces237x240, and flag/reset/open/close plus normal
launcher exit all pass. The original W32CreateWindow reads dwStyle directly
from the Win16 argument and passes it to pfnCsCreateWindowEx; no minimized-style
insertion was found in the selected host creation path.

A separate native USER-only control has no WOW or guest dependency. At96DPI,
SM_CXMINTRACK is136 and SM_CXMINIMIZED is237. Ordinary captioned windows
requested154x240 remain154x240. The exact minimized creation style0x20CA0000
followed by MoveWindow154x240 and restore yields237x240. This reproduces the
same behavior outside the product and matches original xxxAdjustSize's
WFMINIMIZED choice of MMI_MINSIZE rather than MMI_MINTRACK. Original
ISV_ValidateMinMaxInfo resets MMI_MINSIZE from CXMINIMIZED/CYMINIMIZED.
The maintained native runner records all four cases and owned-window cleanup;
it does not assert that every Windows installation must use237 pixels.

An explicit STARTF_USESHOWWINDOW/SW_SHOWNORMAL run using the test observer's
MVDM_OBSERVER_STARTUP_SHOW=normal option still creates WINMINE with0x20CA0000
and reproduces the same chain (HWND003D0B16). Therefore the initial hypothesis
that the observer's hidden startup alone causes this is not supported.
The startup option affects only the observer's child; default behavior remains
unchanged. It neither patches guest code nor overrides creation styles.

## Commands, failed attempts and limits

Run the diagnostic recipe on a fresh build output, then use the existing
verify-wow-mine-gameplay.ps1 against a separately staged diagnostic package
with MVDM_SIZING_TRACE set. The exact build/stamp/run commands and package
identity are retained in build/t478-s44/diagnostic.ps1 and geometry.ps1.
Use verify-native-window-sizing.ps1 with the MSVC compiler driver and a fresh
BuildRoot for the native comparison. The observer uses MSVC x64 /MT /std:c11,
/Isrc and user32/gdi32/advapi32/dbghelp libraries. The normal comparison is
normal-r4.ps1; earlier compile attempts missing the include path and C11 mode
failed before runtime. The earlier normal-r3 run lacked the observer option
because a newline-sensitive edit did not apply; it is another default-start
observation, never counted as explicit-normal evidence. Logs remain intact.

The selected real runtime remains x86 /MT CCPU40. No physical historical NT4
desktop was run; this is evidence of preserved sizing control flow with modern
native metrics, not pixel identity across Windows versions. No DPI policy,
system metric, creation style or requested application dimension is overridden.
No general WINMINE visual/difficulty or all-app compatibility claim follows.

## Disposition

The inherited width symptom has a demonstrated native sizing cause, not an
established missing host connection. Preserve the original minimized-size
rule and modern USER ownership. Do not add a WINMINE exception or a replacement
nonclient window manager to force154. S43's separate black-brush defect remains
fixed. T478's remaining inventory/handoff reconciliation is still outstanding.

## Delivery verification

The unchanged published S43 package was independently exercised with explicit
normal startup and the updated observer: flag, F2 reset, left-click cell,
WM_CLOSE and normal launcher exit pass. All11 assets/release image hashes
still equal package22e35db2-13bb-419d-ba68-7a169cc939e1. Its prior Full16/16
result is reused because no production or guest bytes changed; this S does
not claim a fresh Full run or publish the diagnostic DLL. The existing
DOS4GW external-fixture exclusion is unchanged. No O:/winnt write or push.

The [receipt](../../../artifacts/mvdm-audit/t478-final/sizing/results.json)
pins native/diagnostic sources, original USER source identities, restored
product-source hashes and the unchanged release. Adjacent text files preserve
both same-run traces, native comparison and final release gameplay/exit.
The normal-release reproduction command is build/t478-s44/release-test.ps1;
its underlying maintained entries are verify-native-window-sizing.ps1 and
verify-wow-mine-gameplay.ps1, with MVDM_OBSERVER_STARTUP_SHOW=normal. The
native diagnostic is observational, not an additional product capability.
