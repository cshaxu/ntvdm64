"""Inventory original/current condition arms and inherited exclusions.

Sequence matching yields review leads only; reordered or changed conditions
cannot be restored automatically from a lexical match.
"""
import argparse
import collections
import difflib
import hashlib
import json
from pathlib import Path
import re


def arm_inventory(raw):
    text = raw.decode("latin-1")
    # Preserve all physical line breaks while suppressing commented directives.
    clean = re.sub(r'/\*[\s\S]*?\*/|//[^\r\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                   lambda match: re.sub(r"[^\r\n]", " ", match.group())
                   if match.group().startswith("/") else match.group(), text)
    lines = clean.split("\n")
    arms, stack, errors = [], [], []
    index, logical = 0, 1
    while index < len(lines):
        line_number = index + 1
        logical_number = logical
        line = lines[index].rstrip("\r")
        while line.rstrip().endswith("\\") and index + 1 < len(lines):
            index += 1
            logical += 1
            line = line.rstrip()[:-1] + " " + lines[index].lstrip()
        match = re.match(r"^\s*#\s*(if|ifdef|ifndef|elif|else|endif|error)\b(.*)", line)
        if match:
            directive, expression = match[1], " ".join(match[2].split())
            following = re.match(r"^\s*#\s*line\s+(\d+)\b", lines[index + 1]) if index + 1 < len(lines) else None
            body_line = int(following[1]) if following else None
            if directive in ("if", "ifdef", "ifndef"):
                parent = stack[-1]["active"] if stack else None
                arm = len(arms)
                arms.append({"line": line_number, "logical_line": logical_number, "directive": directive,
                             "expression": expression, "parent": parent, "group": arm,
                             "body_line_marker": body_line, "excluded": False})
                stack.append({"parent": parent, "active": arm, "group": arm})
            elif directive in ("elif", "else"):
                if not stack:
                    errors.append({"line": line_number, "directive": directive})
                else:
                    arm = len(arms)
                    arms.append({"line": line_number, "logical_line": logical_number, "directive": directive,
                                 "expression": expression, "parent": stack[-1]["parent"],
                                 "group": stack[-1]["group"], "body_line_marker": body_line,
                                 "excluded": False})
                    stack[-1]["active"] = arm
            elif directive == "endif":
                if stack:
                    stack.pop()
                else:
                    errors.append({"line": line_number, "directive": directive})
            elif stack:
                arms[stack[-1]["active"]]["excluded"] = True
        line_marker = re.match(r"^\s*#\s*line\s+(\d+)\b", line)
        logical = int(line_marker[1]) if line_marker else logical + 1
        index += 1
    if stack:
        errors.append({"unclosed": len(stack)})
    return arms, errors


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("identities", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    output = args.output.resolve()
    if not output.is_relative_to((root / "build/t476-s1").resolve()) or output.exists():
        raise SystemExit("output must be a fresh file under build/t476-s1")
    checkpoint = json.loads(args.identities.read_text(encoding="utf-8"))
    if checkpoint.get("not_in_census") or checkpoint.get("not_in_tree"):
        raise SystemExit("tracked C/H mirror inventory is not reconciled")
    results, counts = [], collections.Counter()
    for row in checkpoint["files"]:
        if not row["path"].startswith(("src/mvdm-host/", "src/non-mvdm/")) or not row["path"].lower().endswith((".c", ".h")):
            raise SystemExit(f"outside host C/H audit scope: {row['path']}")
        current = (root / row["path"]).read_bytes()
        original = Path(row["original"]).read_bytes()
        if (hashlib.sha256(current).hexdigest() != row["current_sha256"] or
                hashlib.sha256(original).hexdigest() != row["original_sha256"]):
            raise SystemExit(f"identity changed after checkpoint: {row['path']}")
        old, old_errors = arm_inventory(original)
        new, new_errors = arm_inventory(current)
        if old_errors or new_errors:
            counts["parser_error_files"] += 1
        token = lambda arm: (arm["directive"], arm["expression"])
        # Existing #line anchors can disambiguate repeated #if expressions;
        # treat this as static matching evidence only, not a selected path.
        current_keys = collections.defaultdict(list)
        for current_id, arm in enumerate(new):
            current_keys[(token(arm), arm["logical_line"])].append(current_id)
            if arm["body_line_marker"] and arm["body_line_marker"] - 1 != arm["logical_line"]:
                current_keys[(token(arm), arm["body_line_marker"] - 1)].append(current_id)
        pairs, used = {}, set()
        for original_id, arm in enumerate(old):
            candidates = current_keys.get((token(arm), arm["line"]), [])
            if arm["group"] != original_id and arm["group"] in pairs:
                expected_group = new[pairs[arm["group"]]]["group"]
                candidates = [candidate for candidate in candidates
                              if new[candidate]["group"] == expected_group]
            if len(candidates) == 1 and candidates[0] not in used:
                pairs[original_id] = candidates[0]
                used.add(candidates[0])
        # Within an already matched #if group, an #else/#elif need not retain
        # its former physical position after an earlier arm became #error.
        for original_id, arm in enumerate(old):
            if original_id in pairs or arm["group"] == original_id or arm["group"] not in pairs:
                continue
            expected_group = new[pairs[arm["group"]]]["group"]
            candidates = [current_id for current_id, candidate in enumerate(new)
                          if current_id not in used and candidate["group"] == expected_group
                          and token(candidate) == token(arm)]
            if len(candidates) == 1:
                pairs[original_id] = candidates[0]
                used.add(candidates[0])
        matcher = difflib.SequenceMatcher(None, list(map(token, old)), list(map(token, new)), autojunk=False)
        for i, j, length in matcher.get_matching_blocks():
            for offset in range(length):
                old_id, current_id = i + offset, j + offset
                if old_id not in pairs and current_id not in used:
                    pairs[old_id] = current_id
                    used.add(current_id)
        conflicts = []
        for old_id, current_id in pairs.items():
            old_parent = old[old_id]["parent"]
            current_parent = new[current_id]["parent"]
            if old_parent is not None and old_parent in pairs:
                expected = pairs[old_parent]
                while current_parent is not None and current_parent != expected:
                    current_parent = new[current_parent]["parent"]
                if current_parent is None:
                    conflicts.append(old_id)
            original_group = old[old_id]["group"]
            if original_group in pairs and new[pairs[original_group]]["group"] != new[current_id]["group"]:
                conflicts.append(old_id)
        conflicts = sorted(set(conflicts))
        # A suspect pairing cannot prove that an original subtree is protected.
        covered = {old_id for old_id, current_id in pairs.items()
                   if old_id not in conflicts and new[current_id]["excluded"]}
        missing, skipped = [], []
        for old_id, arm in enumerate(old):
            parent = arm["parent"]
            while parent is not None and parent not in covered:
                parent = old[parent]["parent"]
            if parent is not None:
                skipped.append(old_id)
            elif old_id not in pairs:
                missing.append(old_id)
        # Even if a repeated child expression matched somewhere else, never
        # suggest restoring it inside a previously excluded ancestor.
        conflicts = [old_id for old_id in conflicts if old_id not in skipped]
        counts["files"] += 1
        counts["original_arms"] += len(old)
        counts["current_arms"] += len(new)
        counts["current_error_arms"] += sum(arm["excluded"] for arm in new)
        counts["inside_existing_error"] += len(skipped)
        counts["unmatched_original_leads"] += len(missing)
        counts["files_with_leads"] += bool(missing)
        counts["ancestry_conflicts"] += len(conflicts)
        counts["files_with_ancestry_conflicts"] += bool(conflicts)
        results.append({"path": row["path"], "original": row["original"],
                        "original_sha256": row["original_sha256"],
                        "current_sha256": row["current_sha256"], "original_arms": old,
                        "current_arms": new, "matched": pairs, "skipped_inside_error": skipped,
                        "unmatched_original_leads": missing, "ancestry_conflicts": conflicts,
                        "parser_errors": {"original": old_errors, "current": new_errors}})
    output.write_text(json.dumps({"checkpoint_head": checkpoint["head"], "counts": dict(counts),
                                  "extra_preprocessor_files": checkpoint.get("extra_preprocessor_files", []),
                                  "assembly_condition_files": checkpoint.get("assembly_condition_files", []),
                                  "extra_unresolved": checkpoint.get("extra_unresolved", []),
                                  "files": results}, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(dict(counts)))


if __name__ == "__main__":
    main()
