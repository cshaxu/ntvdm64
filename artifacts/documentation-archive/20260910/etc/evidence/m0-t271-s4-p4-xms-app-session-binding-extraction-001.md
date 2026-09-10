# M0 T271 S4 P4 — XMS app session-binding extraction

## Question

Does the non-original XMS bind/reset lifecycle belong to the OpenNT BOP mirror
or to final application composition?

## Result

`xms_runtime_session.{c,h}` has moved from `opennt-bop/ingress/` to
`app/xms_session_binding.{c,h}` and its app-owned API is now
`app_xms_session_bind` / `app_xms_session_reset`.

The code only creates, binds, unbinds and resets the one session record around
the original XMS package. It does not decode a selector, inspect a service
byte, change the XMS table or implement a provider. Therefore it is final
composition, not mirror source.

## Verification

Formal root `build/M0-T271-S4/r002` compiled the helper into `app.lib`, not
`opennt-bop.lib`, and both XMS source/A20 fixtures returned zero.

## Boundary

The application does not yet automatically enable an XMS capacity from every
engine request: the existing request's guest-RAM geometry is not itself an
admitted OpenNT XMS/EMS configuration contract. This move preserves that
explicit admission boundary rather than silently deriving a new XMS profile.
