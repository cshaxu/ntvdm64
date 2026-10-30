# SOL bounded stack-word sampling

The current ten-image package completed an Options interaction while the
owned game thread was sampled at nominal 25 ms intervals. Each diagnostic
sample captured its x86 control registers and up to 128 bytes at ESP while
suspended, then resumed it before output. The retained manifest identifies
the staged package; source hashes and map hashes identify the observer and
symbol inputs. Failure to resume terminates the owned probe so stage cleanup
can terminate its processes. The probe also ran the normal close witness.

There were 127 program-counter samples. The additional raw stack analysis
found 39 cases where EBP pointed into the captured range and the next word
could be treated as a candidate return address. Eight such samples combine
an ntdll PC with a nearest-map candidate in
`wow_window_words_collect_retired`; that function scans handle slots under
individual shared-lock acquisitions and is called on borrow-scope exit and
before graph refresh. One sample associates `bsearch` with
`copy_window_observation`. These observations identify a worthwhile adapter
path to measure, not a proven stack trace: optimized code can omit frame
pointers, stack words can be data, symbols are nearest-map matches, and the
sampler observes only one thread and perturbs scheduling. No latency share,
root cause or correctness defect follows from these counts alone.

The independent Windows PowerShell 5 test successfully staged the same ten
images with a fresh package identity, validated all recorded hashes and removed
the owned stage. It did not start product processes.
