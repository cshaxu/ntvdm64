#!/usr/bin/env python3
"""Measure retained original source lines, not functional completeness."""
import csv
import difflib
import hashlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
AUDIT = ROOT / "artifacts/mvdm-audit"
ORIGINAL = Path("O:/repos.external/OpenNT/base/mvdm")


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def source_lines(path):
    """Remove comments and blank lines; preserve quoted text and code spacing."""
    assembly = path.suffix.lower() in (".asm", ".def")
    block = False
    result = []
    for line in path.read_bytes().decode("latin1").splitlines():
        output = []
        quote = None
        escaped = False
        i = 0
        while i < len(line):
            ch = line[i]
            nxt = line[i + 1] if i + 1 < len(line) else ""
            if block:
                if ch == "*" and nxt == "/":
                    block = False
                    i += 2
                else:
                    i += 1
                continue
            if quote:
                output.append(ch)
                if escaped:
                    escaped = False
                elif ch == "\\":
                    escaped = True
                elif ch == quote:
                    quote = None
                i += 1
                continue
            if ch in ('"', "'"):
                quote = ch
                output.append(ch)
                i += 1
            elif assembly and ch == ";":
                break
            elif not assembly and ch == "/" and nxt == "/":
                break
            elif not assembly and ch == "/" and nxt == "*":
                block = True
                i += 2
            else:
                output.append(ch)
                i += 1
        code = "".join(output).strip()
        if code:
            result.append(code)
    return result


def main():
    with (AUDIT / "combined-initial-status.csv").open(encoding="utf8", newline="") as f:
        partial = {row["relative_path"] for row in csv.DictReader(f) if row["status"] == "partial"}
    with (AUDIT / "frozen-file-reconciliation.csv").open(encoding="utf8", newline="") as f:
        frozen = [row for row in csv.DictReader(f) if row["relative_path"] in partial]
    if len(frozen) != len(partial):
        raise ValueError("Partial source set is missing or duplicated")
    records = []
    for row in frozen:
        relative = row["relative_path"]
        if row["selected_comparison_edition"] != "OpenNT":
            raise ValueError(f"Unreviewed source edition: {relative}")
        original = ORIGINAL / relative
        current = ROOT / "src/mvdm" / relative
        if digest(original) != row["opennt_sha256"].upper() or digest(current) != row["current_sha256"].upper():
            raise ValueError(f"Frozen source identity changed: {relative}")
        before, after = source_lines(original), source_lines(current)
        matcher = difflib.SequenceMatcher(None, before, after, autojunk=True)
        matched = sum(size for _, _, size in matcher.get_matching_blocks())
        records.append({"relative_path": relative, "original_sha256": digest(original),
                        "current_sha256": digest(current), "original_code_lines": len(before),
                        "matched_code_lines": matched,
                        "text_retention_percent": f"{100 * matched / len(before):.2f}" if before else "",
                        "limitation": "Text retention only; active branches and observable behavior require separate proof."})
    if not records:
        raise ValueError("No partial sources")
    with (AUDIT / "partial-code-retention.csv").open("w", encoding="utf8", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=records[0].keys())
        writer.writeheader()
        writer.writerows(records)
    print(f"Measured {len(records)} partial-source files; this is not functional coverage")


if __name__ == "__main__":
    main()
