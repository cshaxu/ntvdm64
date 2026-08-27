# M0 T291 S3 P2 — WOW task/frame projection

## Scope

The original `CURRENTPTD()` macro retrieves a full NT4 WOW `TD` from a
private TEB field.  The callback path reaches only `vpStack`, `vpCBStack`,
`dwFlags` bit `TDF_INITCALLBACKSTACK` and `htask16`.  The full TD also holds
native thread/window/GDI/process pointers and therefore cannot become a
session record on either host width.

## Result

`adapter-mvdm-host-out/monitor` now owns a bound-session copied projection of
only those callback-reached numeric fields.  It reproduces the original
`CallBack16` branch: first callback subtracts from `vpCBStack` and sets the
flag; later callback subtracts from `vpStack`; both results are word aligned.
It permits no process-global current task and no native pointer in the
projection.

## Verification

The formal fixture passed for MSVC x86 and x64. It proves no-current-task
rejection, duplicate bind rejection, both original stack-selection branches,
alignment, source-shaped stack restoration and clear/unbound behavior.
Neither a WOW provider body nor a guest CPU loop enters this fixture.

## Follow-up

P3 will combine this numeric projection with P1's bounded lease scope and the
typed SoftPC/Bochs run boundary to implement the one-session callback
transaction.
