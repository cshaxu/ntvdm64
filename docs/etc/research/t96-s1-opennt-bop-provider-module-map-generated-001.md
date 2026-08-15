# OpenNT BOP Provider and Module Map

Generated source mapping only; no row declares an enabled provider or a successful guest operation.

| Family | Services | Original entry | Current disposition |
| --- | ---: | --- | --- |
| BOP_DOS | 74 | MS_bop_0 -> DemDispatch | mapped-not-enabled |
| BOP_CMD | 18 | MS_bop_4 -> CmdDispatch | mapped-not-enabled |
| BOP_XMS | 12 | MS_bop_2 -> XMSDispatch | mapped-not-enabled |
| BOP_DPMI | 25 | MS_bop_3 -> DpmiDispatch | mapped-not-enabled |
| BOP_REDIR | 49 | MS_bop_5 -> VrDispatch (historically lazy-loaded) | mapped-not-enabled |
| BOP_DEBUGGER | 16 | MS_bop_6 -> DBGDispatch | mapped-not-enabled |

Top-level selectors: 16
Historical BIOS selector definitions: 41
Historical BIOS table rows: 349

Inventory JSON SHA-256: 893eb8d98a476a4d8a7ae996635b2b2bd24706594de3666c2e664a1fb97e5c28
Map JSON SHA-256: f368d0b56420fc2a6ad3b30ca82993acb8d969cc608861e317f234bdc795d792