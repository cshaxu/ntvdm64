# T484 S3 P171 retire expired diagnostic branches

Owner requires retrospective deletion of obsolete probes, including ones already
migrated. Review B0101 and B0110, their actual callers, native targets and P74/P75/
P82 evidence. This is test/tool retirement, not a production or guest change.

B0101 keeps its current message-transaction diagnostic. The maintained Win16
message runner accepts TransactionDiagnosticManifest and exercises the matching
BeforeCallback/AfterCallback boundary. Delete only P21Control and its55-line
CMD builder: that branch required a fixed historical DLL hash, a fixed Git TU
and sealed pre-CMake object/link commands. P75 already established the matching
objects were disposed. No current suite calls this historical comparison, and
rebuilding an obsolete provider is not a current acceptance requirement. Do not
mix current objects into that old control or claim the historical positive passed.

B0110 is the independent CONTRACE TSR and its T436 BAT/PIF preparation script.
It captures DOS CON-open outcomes and output bytes for manual startup diagnosis;
it has no current test runner or automated assertions. Only historical evidence
references its generated reports. Retire the preparation script,208-line authored
ASM probe, native target and CMD selection together. Keep current Win31 matrix,
DOSONLY, DOS observation and real product integration tests. Absence of a suite
caller alone is not the reason: this is an old manually interpreted diagnostic,
not an active regression contract. It was not an original guest component.

Net source reduction:331 lines across six files. No replacement framework or
new diagnostic state is added. Existing historical evidence remains indexed.

Background PID27176, build/t484-s3-expired-diagnostics/run.ps1, verifies the
retained current diagnostic builder and original-TU byte identity, rejects the
removed P21 switch before output creation, rejects the removed CMD target with
exit2, builds the remaining win31-dosonly authored fixture, then runs cached full
host build and11 raw/22 published hash checks. No new actual guest callback-fault
run or Windows boot is claimed; the retained diagnostic implementation is unchanged.

PID27176 passes all checks in77.8 seconds. All11 raw inputs and22 published
hashes remain unchanged; no redundant publication or new Full run is claimed.
B0101 and B0110 now have dispositions:311/355 complete,44 remaining. The
original355 count does not increase for deletion of the subsequently created
CMD control tool. Documentation governance is closure-result.json in the same
owned batch; current diagnostic copies are disposed after the run, retaining
manifests, input hashes and logs. Stable native caches remain intact.
