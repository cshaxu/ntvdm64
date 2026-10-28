"""Reuse the original GDI table verifier for the core owner."""
import json
import runpy


def inspect():
    return runpy.run_path('artifacts/mvdm-audit/shard-2/parallel-review-third-observer.py')['inspect'](('wgdi',))


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
