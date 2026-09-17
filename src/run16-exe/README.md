# run16

`main.c` is the public CreateProcess-shaped CLI. It discovers the requested
image, invokes the selected original classifier, starts/connects BaseSrv,
creates and registers a suspended worker, then waits through the original
parent-completion route. `console_probe.c` is its short-lived private Console
membership helper. Neither file owns BaseSrv DOS/WOW policy or worker state.
