# Part 2: DOS country-message build inputs

These 13 owned MVDM source paths all match both frozen originals. The original messages recipe creates COUNTRY.idx from COUNTRY.msg and defaults to usa-ms only when COUNTRY is unset; DOS make.inc changes the message directory for an explicit country. These rows are locale-specific source inputs, not duplicate translations. The captured x86 worker/WOW32 graphs have no exact path. The listed counts are a literal message-record heuristic, not proof of valid or complete translations. ZIP same-name observations do not prove what strings are compiled into guest images.

| Owned key | Matched records | ZIP entries by basename | Disposition and boundary |
| --- | ---: | --- | --- |
| `dos/v86/messages/denmark.msg` | 1021 | none | Historical parameterized guest message input; **pending** localized build selection and runtime behavior. |
| `dos/v86/messages/finland.msg` | 1021 | none | Historical parameterized guest message input; **pending** localized build selection and runtime behavior. |
| `dos/v86/messages/france.msg` | 1021 | none | Historical parameterized guest message input; **pending** localized build selection and runtime behavior. |
| `dos/v86/messages/germany.msg` | 1021 | none | Historical parameterized guest message input; **pending** localized build selection and runtime behavior. |
| `dos/v86/messages/holland.msg` | 1021 | none | Historical parameterized guest message input; **pending** localized build selection and runtime behavior. |
| `dos/v86/messages/italy.msg` | 1020 | none | Historical parameterized guest message input; **pending** localized build selection and runtime behavior. |
| `dos/v86/messages/japan.msg` | 1350 | none | Historical parameterized guest message input; **pending** localized build selection and runtime behavior. |
| `dos/v86/messages/nether.msg` | 1000 | none | Historical parameterized guest message input; **pending** localized build selection and runtime behavior. |
| `dos/v86/messages/norway.msg` | 1021 | none | Historical parameterized guest message input; **pending** localized build selection and runtime behavior. |
| `dos/v86/messages/portugal.msg` | 1021 | none | Historical parameterized guest message input; **pending** localized build selection and runtime behavior. |
| `dos/v86/messages/spain.msg` | 1020 | none | Historical parameterized guest message input; **pending** localized build selection and runtime behavior. |
| `dos/v86/messages/sweden.msg` | 1012 | none | Historical parameterized guest message input; **pending** localized build selection and runtime behavior. |
| `dos/v86/messages/usa-ibm.msg` | 1078 | none | Historical parameterized guest message input; **pending** localized build selection and runtime behavior. |

Do not interpret these pending decisions as excluding locales. Existing part-2 fax and original shard-2 locale reports cover separate data; no guest, build, product or shared ledger changed. Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-2/dos-locales-observer.py` and compare the JSON.
