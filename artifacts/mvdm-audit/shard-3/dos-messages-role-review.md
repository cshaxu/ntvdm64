# DOS localized message catalogs — shard 3

Scope: pending `dos/v86/messages/{denmark,finland,italy,portugal,sweden}.msg` and `state.rst`, selected from `combined-initial-status.csv` with shard 3 / pending. `dos-messages-observer.py` and captured JSON reproduce full source SHA-256 comparison against both original editions and exact-name package listing. All six files and the messages makefile are byte-identical to both references. No guest build, execution, installation or media modification occurred.

`messages/makefile:1-14` defaults COUNTRY to usa-ms and derives a corresponding index from the selected .msg. `dos/v86/make.inc:7-13,110-116` determines COUNTRY-dependent message paths and invokes historical message-building tools; `dos/v86/cmd/command/makefile:29-31` and graphics/makefile:31 are examples of consumers. The five language files contain DOS text catalogs, not standalone executables. A selected alternate-country build requires the correct historical country-directory arrangement; the source references alone do not establish which catalog was used in any shipped binary. No exact named five catalogs occur in assets/winnt.zip, which does not disprove their text being embedded into a guest binary.

| path | proposed file disposition | basis / boundary |
|---|---|---|
| `denmark.msg`, `finland.msg`, `italy.msg`, `portugal.msg`, `sweden.msg` | pending | Original alternate-language message inputs. Their use, output encoding, guest source-to-image provenance, translated message behavior and available language choices remain unproven. Do not exclude localization functionality merely because these source files are not packaged by name. |
| `state.rst` | excluded, editor-state file role only | Small historical editor state points to `f:\\src\\dos500\\messages\\usa-ibm.msg` and cursor/search settings; it is not the catalog itself. This does not exclude the referenced US catalog or localization. |

Byte identity and makefile dependencies are reproducible static evidence, not executed locale or guest behavior. No restoration percentage is asserted.
