# Original SoftPC host utility declarations — shard 3

Four pending paths from current `shard-3/file-review.csv`: `softpc.new/base/inc/{host_cfg,host_pth,host_sig,host_str}.h`. `host-utility-interfaces-observer.py` and captured JSON compare exact bytes to both reference editions and normalize each retained Ninja C-dependency entry relative to the worker build root. All four headers match both originals. No product, guest, shared ledger or build state was modified.

| file | proposed status | role, current observation and limit |
|---|---|---|
| `host_cfg.h` | excluded, empty guard-only file role | Contains only comments and `_HOST_CONFIG_H` include guard; original `base/inc/config.h:417` includes it. Correctly normalized retained dependency snapshot shows **55** object edges, so it is included broadly despite containing no configuration declarations. Exclusion does not apply to config.h, configuration providers or runtime behavior; an explicit test of the guard macro elsewhere could still affect preprocessing. |
| `host_pth.h` | pending | Declares original HostPathAppend*/HostPathMakeTempFilePath interfaces. Text search of the examined SoftPC source found those identifiers in this header, not an independently located selected provider; normalized snapshot has no exact header edge. Alternative configurations or other source roots and actual behavior remain unverified. |
| `host_sig.h` | pending | Declares historical host_signal interface, with ANSI conditional prototype. No exact edge in this snapshot; this is not proof that signal behavior has no owner or is out of scope. |
| `host_str.h` | pending | Declares HostStrdup. No exact edge in this snapshot; provider/alternative-profile use and allocation/free ownership require independent proof. |

The dependency file's SHA-256 and complete normalized object matches are captured. Snapshot absence is a bounded observation, not proof of non-use in other configurations; it is not used to claim broad functionality exclusion. No runtime execution, failure or lifecycle test was performed, and no restoration percentage is inferred.
