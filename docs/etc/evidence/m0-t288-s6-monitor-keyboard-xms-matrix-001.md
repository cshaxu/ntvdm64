# M0 T288 S6 monitor/keyboard/XMS matrix evidence

The formal original XMS matrix now links the recovered production
`mvdm_sas.c` and `mvdm_int15.c`; its former test-only `UpdateKbdInt15`
definition is removed. The renamed fixture initializes typed Bochs RAM, sets
the INT 15h IVT vector, captures it through the firmware lifecycle hook, then
invokes original `xmsNotifyHookI15` through `XMSDispatch(9)`.

The original body passes `CS:AX` to `UpdateKbdInt15`, then writes `CX` with
the XMS size. The matrix proves that result and proves the cache matches only
after the IVT is changed to the delivered `CS:AX`. Both formal x86/x64 `/MT`
Ninja executions succeeded. No BOP selector, CPU re-entry loop, keyboard
controller, PIC/IRQ shell, raw pointer, second mapper or `src.old` input is
enabled.
