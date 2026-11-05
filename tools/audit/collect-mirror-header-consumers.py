"""Bind host mirror headers to built product-object dependency records."""
import argparse
import collections
import json
from pathlib import Path
import re
import subprocess


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("plan", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("headers", nargs="+")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    output = args.output.resolve()
    if not output.is_relative_to((root / "build/t476-s1").resolve()) or output.exists():
        raise SystemExit("output must be fresh and under build/t476-s1")
    headers = [name.replace("\\", "/").lower() for name in args.headers]
    if any(not name.startswith(("src/mvdm-host/", "src/non-mvdm/")) for name in headers):
        raise SystemExit("only host mirror headers are in scope")
    plan = json.loads(args.plan.read_text(encoding="utf-8"))
    directories = collections.defaultdict(dict)
    for row in plan["commands"]:
        directories[row["directory"]][row["output"].replace("\\", "/").lower()] = row
    consumers = {name: [] for name in headers}
    for directory, outputs in directories.items():
        data = subprocess.check_output(["ninja", "-C", directory, "-t", "deps"],
                                       text=True, errors="replace")
        current = None
        for line in data.splitlines():
            if not line.startswith(" "):
                match = re.match(r"^(.*?):\s+#deps\b", line)
                current = match[1].replace("\\", "/").lower() if match else None
            elif current in outputs:
                dependency = line.strip().replace("\\", "/").lower()
                for header in headers:
                    if dependency.endswith(header):
                        consumers[header].append(outputs[current])
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(consumers, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({name: len(rows) for name, rows in consumers.items()}))


if __name__ == "__main__":
    main()
