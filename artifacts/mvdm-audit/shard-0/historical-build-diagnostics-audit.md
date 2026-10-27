# Historical build diagnostics

`src/mvdm/build.err` (789 bytes, SHA-256
`A90DF63B19A64C9FA90A0602881BA38E461026C67641C025889DBA313EFDA14F`)
and `build.wrn` (426 bytes, SHA-256
`31AD7BFE13CC6C608149C7FF83E0B9348F1DB81E82952D27986ADB63313513E2`)
are current-only entries in the frozen source union, with no matching row in
either upstream audit catalog. Inspection shows NMAKE missing-library errors
and historical linker warnings respectively. They are diagnostic output,
not source, build instructions, executable objects or product runtime data.
The frozen selected graph also does not select them. Exclude these two files
from recovery as historical build output; retain their bytes as evidence.
This disposition says nothing about functionality or the historical link
warnings, nor does it authorize treating other build-named files alike.
