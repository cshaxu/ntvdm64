# Window table mutation and graph owner audit

The recorded call-site inventory covers direct project-source references to
the window table publication, retirement, adoption cancellation and detach
functions. Ordinary creation enters `wow_window_creation_enroll_internal`
under the runtime lock before `wow_window_words_publish_owner`; explicit
creation that enters a task callback reacquires its inherited lock before
enrollment and restores the callback's original state on exit. Its acceptance
or veto path enters the same runtime owner before rollback. Native subclass
adoption enters the runtime owner before publishing or cancelling an owner.
Terminal retirement enters that owner around its table mutation and releases
the publication lock before original finalizer callouts. The graph refresh
now enters the runtime owner before scanning the table.

The sole production `wow_window_words_detach` call is the creation path's
failed acquisition immediately after attachment and before that path publishes
an owner. `detach_matching` itself can also retire an existing owner and only
acquires the publication lock; the identified production caller's enclosing
creation path is therefore material. This is a bounded call-chain finding,
not an assertion that indirect calls, abnormal native thread death or every
future caller meet the same contract.

The shared object table also contains menu entries. Hook message menu refresh
uses `menu_publish`, which enters the runtime owner before its menu-tree
publication and retirement. Native-return refresh occurs inside the native
return owner; original task cleanup reaches menu cleanup inside task retirement.
This source audit does not prove every menu mutation or native callout's
concurrency behavior. The selected creation/unwind, dialog rollback and
graph-owner tests listed in `selected-results.txt` passed; the real shared
guest hook/Winmine witness is [here](../graph-owner-mine-cbt/review.md).
Input hashes and the exact direct call-site list are retained. No code or
guest image changed in this audit, and no causal claim about historical 1400
is made.
