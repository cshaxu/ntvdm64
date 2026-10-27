# Guest support owner review

Fixed shard-2 pending paths in DDEML, WIN87EM, REGEDIT, CTL3DV2 and SHERLOCK only; exact  paths and hashes are in the CSV. Selected candidate inputs require same OpenNT/current hash and original make source/object/link, resource, or direct include edge; sample tests remain test guest inputs, not production guests.
68 rows; 38 candidates; 30 pending; 0 frozen/current hash mismatches.

WIN87EM included code is conditional assembly; this proves a historical build role, not selected modern host execution or guest capability. Generated output, tools/scripts, debug objects and unlinked sources remain pending. DDEML test sources refer to their independent sample executables, not production library behavior. No guest was changed, built or executed; no host code was admitted.

Recipe SHA256 original/current:
- `wow16/ddeml/makefile` `8D8A1CBA40DB55B3C43FB97C5624877FC7084559AF568EFEABC6D58E3AFEE582` / `8D8A1CBA40DB55B3C43FB97C5624877FC7084559AF568EFEABC6D58E3AFEE582`
- `wow16/ddeml/tests/src/client/makefile.dos` `63B9B5B9C52E85D81904141DFD1354015E21262C5DA87A7D1603053FEFB50653` / `63B9B5B9C52E85D81904141DFD1354015E21262C5DA87A7D1603053FEFB50653`
- `wow16/ddeml/tests/src/server/makefile.dos` `5B5E8FF82B3F34B0E1940C0850E064858580E28E212011F1129063AD09D39934` / `5B5E8FF82B3F34B0E1940C0850E064858580E28E212011F1129063AD09D39934`
- `wow16/regedit/makefile` `6255669DED8680CF24DA6D74B49366D2F7F5AD08871CB95DE26A46D42DB339FD` / `6255669DED8680CF24DA6D74B49366D2F7F5AD08871CB95DE26A46D42DB339FD`
- `wow16/win87em/makefile` `B76EC6E35F2C1285EF0C0930C4D534CA0E0A7533F8F4C3B4E53725ABE3C907A4` / `B76EC6E35F2C1285EF0C0930C4D534CA0E0A7533F8F4C3B4E53725ABE3C907A4`
- `wow16/sherlock/makefile` `D4E0A78455E5C688D42A7D96E8C012A39074F18A3411FA23A917809A8B7199D7` / `D4E0A78455E5C688D42A7D96E8C012A39074F18A3411FA23A917809A8B7199D7`
