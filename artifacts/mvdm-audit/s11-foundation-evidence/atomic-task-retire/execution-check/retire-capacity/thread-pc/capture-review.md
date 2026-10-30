# Repeated native window observation

`wow_user_native_call_end` may publish the guest-visible window view before
continuation. `refresh_graph` gathers the desktop and enrolled window roots,
then `project_graph_attempt` observes their relations. For every node sent to
`capture_window`, the latter calls `GetWindowInfo` for the stored geometry,
style and atom, then calls `wow_user_window_sample_links`; that public helper
calls `GetWindowInfo` a second time as a validity check before querying the
relations. `capture_window` also rechecks the original thread and process IDs
after gathering class fields. The second `WINDOWINFO` is not copied into the
published observation.

This is a concrete repeated host call in the project adapter, not a finding
about the original CCPU. Prior diagnostics measured 128–192 whole graph
projections in portions of an Options phase, but did not count the number of
nodes on this exact capture path or measure the two `GetWindowInfo` calls.
Consequently their aggregate cost and relevance to the observed seconds-long
delay are not established.

The helper is also used independently, including by lifecycle tests; its
public validation cannot simply be removed. A private relation-only helper
could share the first successful observation with `capture_window`, leaving
the public helper unchanged. That would move one invalid-window failure
opportunity earlier, however: a window may be destroyed or its handle reused
between the original two checks, and the final thread/process comparison is
not a complete generation check if reuse occurs in the same thread. The
approved external-window snapshot boundary does not alone prove all local
window lifetime cases safe. No implementation change is adopted on this
evidence. Any candidate needs an explicit identity/retirement argument,
independent helper coverage and a matched measurement showing a useful
effect or a net code reduction before integration.
