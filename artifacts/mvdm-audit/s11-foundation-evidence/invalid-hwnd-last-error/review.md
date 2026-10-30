# Native invalid-window error control

On this Windows host, a controlled x86 native probe set last-error to 777,
created and destroyed an owned window, and queried its old HWND with
GetWindowThreadProcessId. It returned zero and last-error 1400. An arbitrary
invalid HWND produced the same result. This confirms that a dead HWND can
produce 1400 through the first call of the current `capture_window`; it does
not establish which operation produced the historical SOL failure or whether
that failure involved a stale identity. It does not claim behavior on other
Windows versions or prove guest behavior. The result contains an ephemeral
native handle. No project binary or guest was changed.
