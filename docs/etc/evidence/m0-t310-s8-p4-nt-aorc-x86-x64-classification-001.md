# M0 T310 S8 P4: nt_aorc CCPU Register-Vector ABI Classification

## Question

Do the selected CCPU register wrapper and common function-vector declarations
carry a verified, identical x86/x64 call ABI?

## Inputs and procedure

- Read the reached CCPU provider declarations in softpc.new/base/ccpu386/cpu4gen.h
  and the selected table in softpc.new/host/src/nt_cprgs.c.
- Read nt_aorc.c wrapper definitions and the public common slots in host_cpu.h.
- Force formal Ninja rebuilds of obj/host/nt_aorc.obj and obj/host/nt_cprgs.obj
  on x64 and x86.

## Observations

- The original CCPU providers use exact IU8, IU16, IU32, IBOOL and ISM32
  contracts; the selected table had already begun to express the same shapes.
- The common header still erased those parameter and result types through empty
  function-pointer lists, while the direct facade used K and R forms. Segment
  wrappers additionally defaulted their result to int.
- The repair retains every callback name, table order, wrapper body and guest
  operation. It states the selected source contracts in the facade and common
  header only.
- The remaining C4244 records are fixed-width guest/register narrowing
  questions. They appear identically on x64 and x86 and are not host-width,
  pointer or callback ABI defects.

## Result

- Records CLASS-019024 through CLASS-019061 and CLASS-019063 are closed by
  MVDM-HOST-DIV-102.
- CLASS-019062 remains visible as a non-x86/x64 source-range warning.
- Formal x64 and x86 rebuilds passed for both reached objects; no warning
  suppression, mapping-manager identity, or new runtime logic was added.
