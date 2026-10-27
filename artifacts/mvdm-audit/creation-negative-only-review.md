# Failure-only window creation observation

This is a diagnostic supplement to the [candidate review](candidate-runtime-review.md)
and [earlier comparisons](candidate-runtime-followup-review.md). It is not a
runtime correction, release qualification, or a new restored capability.

The earlier diagnostic intercepted every SetLastError call in three
project-owned translation-unit copies. Although it logged only error 5023,
its extra helper call also affected ordinary successful paths. This version
removes that global interception. It observes explicit ERROR_INVALID_STATE
failure assignments and failed creation-scope error captures only, preserving
the supplied error, function result, and incoming last error. It makes no extra
guest-memory reads and changes no production source or cached object.

The [identity and result manifest](creation-negative-only/observations.json)
binds the complete private builder, runner, input/output hashes, process owner
records, package manifest and actual result. The normal WOW32 provider hash
remained C97C512C6F6BD6F16A6FB68F340760F551CD2186B5262B1A6AE892FC8A06BC58.
The test used the existing SOL Options probe through the established
desktop-capable modern PowerShell launch and a helper-owned private desktop.
No Computer Use or foreground interaction was used.

The one bounded observation passed modal creation, close and parent recovery.
No NegativeStateFailure record appeared. This does not identify the failed
invariant in either ordinary candidate failure. Even failure-only observation
changes the DLL's code/layout; its passing outcome cannot qualify the ordinary
candidate or establish a timing, CPU, handle-reuse or instrumentation cause.
Do not discard the retained production failures or remove publication guards
on this basis. The next causal comparison must preserve the ordinary provider
or independently reproduce a specific violated identity/state contract.

A subsequent [ordinary production control](creation-negative-only/ordinary-control-r3-observations.json)
also passed Options, close and parent recovery without any diagnostic DLL or
worker replacement. It used the retained candidate manifest and checked-in
runner. There was no bindingError=5023 in its creation trace. The two earlier
production failures remain valid, but production failure is not universal
across these observations and cannot be attributed to instrumentation alone.
The complete parent environment was not frozen across runs; do not infer a
proven timing, handle-reuse or other cause from this difference. The candidate
remains unqualified until the failed invariant is understood and verified.
