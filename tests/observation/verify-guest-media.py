"""Verify immutable guest inputs in an isolated product test package."""
import hashlib
import json
from pathlib import Path, PurePosixPath
import sys


def main():
    if len(sys.argv) != 3:
        raise SystemExit("usage: verify-guest-media.py PACKAGE_ROOT MANIFEST")
    root = Path(sys.argv[1]).resolve(strict=True)
    manifest = json.loads(Path(sys.argv[2]).read_text(encoding="utf-8"))
    if manifest.get("schema") != "ntvdm64.guest-archive.v1" or not manifest.get("files"):
        raise ValueError("invalid or empty guest manifest")
    configurable = {"system.ini", "system32/autoexec.nt", "system32/config.nt"}
    failures = []
    checked = 0
    seen = set()
    for entry in manifest["files"]:
        path = PurePosixPath(entry["path"])
        if len(path.parts) < 2 or path.parts[0].casefold() != "winnt" or any(
            part in (".", "..") or "\\" in part or ":" in part for part in path.parts
        ):
            raise ValueError(f"invalid guest manifest path: {path}")
        relative = PurePosixPath(*path.parts[1:])
        key = str(relative).casefold()
        if key in seen:
            raise ValueError(f"duplicate guest manifest path: {path}")
        seen.add(key)
        target = root.joinpath(*relative.parts)
        if not target.is_file():
            failures.append(f"missing: {relative}")
            continue
        if key in configurable:
            continue
        digest = hashlib.sha256(target.read_bytes()).hexdigest()
        if digest.casefold() != entry["sha256"].casefold():
            failures.append(f"hash mismatch: {relative}")
            continue
        checked += 1
    if failures:
        print("\n".join(failures), file=sys.stderr)
        raise SystemExit(1)
    print(f"PASS guest media: {checked} immutable files; configuration excluded")


if __name__ == "__main__":
    main()
