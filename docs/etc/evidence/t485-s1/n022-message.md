# N022 clmsg.c candidate review

Base revision b194dbc8f. XP clmsg blob a46279e99539cd7780f574cadfba70b4f0e05c1d;
OpenNT blob 4c1b7ae390dc7d15b8db98f8a36d5e8ddf446387. Original source bytes
are retained locally; paired server.c/user.h/usercli.h remain OpenNT-baselined
until their full-file migration. No guest or winnt.zip modification.

Retained XP default facade/worker, mouse-key helper, APPCOMMAND/XBUTTON,
RTL context-menu coordinates, packed shell rectangle semantics, listbox
callback pointer widths and CPD signature. Restored harmless unexpanded
BEGIN/END_CALLWINPROC macros and public IME key forwarding.

Residual boundaries: original caller include/message prototypes; existing
translation-unit selections; native USER thread dispatch/activation/theme
state (XP63-251,280-1270,2143-2259,2288-2587); native IMM and UI state
(XP1763-2132 and its private local declarations); and diagnostic line mapping.
The omitted dispatch bodies require GetClientInfo/CTIF, private CPD handle
validation and original native queue/server objects. This product uses the
existing scoped native dispatch and WOW callbacks, not a copied USER server.
Full USER/Win32k is a documented stopping boundary, not merely unused code.
Original DBG/LAME_BUTTON/BUILD_WOW6432/GENERIC_INPUT guards retain #error.

Two behavior adaptations: retain the existing post-WM_MOVE destroyed/fnid
check (existing real-body fixture destroys the child in that callback), and
send WM_QUERYDRAGICON's final request to the existing native default service.
The latter preserves the previous delivered provider while retaining XP's
Win40/server-procedure gates. Native LoadIconW cannot load a packed guest
module; importing the original private shared cursor cache is not this finite
client boundary. This does NOT claim XP guest resource-1 drag-icon fallback.
No new icon cache, state machine, callback registry or resource parser.

Bindings capture existing BitCount/gray brush into the same call-owned XP
shape. Native IMM/UI messages use the existing synchronous native owner.
GetCPD uses the already-imported original w32w64 type definitions. Existing
x86 native CPD provider ABI remains unchanged. No duplicated type layout.

Cached full build passed; runtime36 dual13-image26hash publication and Full24/24
passed. Controls37/38 first pass; old test fixed WM_PRINT rather than XP
WM_UAHINIT as maximum message. That case alone passed retry. The existing
default-state fixture was unregistered; wired it to real state/brush/table
providers and the existing cohort. It passed, extending the cohort to39.

Final redundant #line/EOF cleanup changes no runtime behavior. Rebuild passed;
raw WOW32 differs at only4 bytes in COFF/debug timestamps, proven by complete
byte comparison after masking only named PE metadata fields. Final runtime37
is dual-published. Final default-state/default-body and real SOL Options passed; they were
verified before receipt finalization; unchanged Full gates were not repeated.
Current static nonerror census:1329files,781exact,792same/537different,
7821points/126901lines; delta0files/0exact/0same/0different,+14points/+7lines.
clmsg:1/2033 OpenNT ->15/1997 XP. Paired server remains OpenNT:1/2967->1/3010.
usercli/user declaration changes remain inside their existing whole-file
byte-difference blocks. Baselines1271XP/58OpenNT. LF unmerges old raw block;
these are original-relative differences, never source-total-line changes.

Final minimality review removed the redundant directive and restored XP EOF.
No guest source or binary modified; no second provider state machine.

[Receipt](n022-message-result.json), [census](n022-message-census.json).
