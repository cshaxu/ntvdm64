import { createHash } from "node:crypto";
import { readdirSync, readFileSync, writeFileSync } from "node:fs";
import { extname, join, relative, resolve } from "node:path";

const root = resolve(process.argv[2] ?? process.cwd());
const docsRoot = join(root, "docs");
const output = join(docsRoot, "etc", "operations", "document-inventory.md");

function classification(file) {
  if (["README.md", "STATUS.md", "QUEUE.md", "TODO.md"].includes(file)) return "principal control document";
  if (file.startsWith("rules/")) return "current rule authority";
  if (/^design\/(GOAL|ARCHITECTURE|CODING|UI|ROADMAP)\.md$/.test(file)) return "current design authority";
  if (file.startsWith("history/")) return "closed task history";
  if (file.startsWith("etc/")) return "indexed supporting material";
  return "retained pre-governance evidence";
}

function collect(directory, entries = []) {
  for (const entry of readdirSync(directory, { withFileTypes: true })) {
    const full = join(directory, entry.name);
    if (entry.isDirectory()) collect(full, entries);
    else if ([".md", ".json", ".tsv"].includes(extname(entry.name)) && full !== output) entries.push(full);
  }
  return entries;
}

const entries = collect(docsRoot).sort().map((full) => {
  const file = relative(docsRoot, full).replaceAll("\\", "/");
  return [file, classification(file), createHash("sha256").update(readFileSync(full)).digest("hex")];
});
const timestamp = new Date().toISOString().replace(/\.\d{3}Z$/, "Z");
const lines = [
  "# Documentation Inventory", "", "## Purpose", "",
  "This generated inventory is the migration-control record for every Markdown,",
  "JSON, and TSV documentation record",
  "document under docs/. It applies the ntvdm64 fixed-topology rule without",
  "silently moving or deleting retained evidence. Regenerate it with",
  "tools/governance/Export-DocumentationInventory.mjs whenever any governed document",
  "changes; the documentation gate rejects missing, added, renamed, or",
  "content-divergent entries.", "", "## Snapshot", "",
  `| Documentation records | ${entries.length} |`, "| --- | --- |",
  `| Generated UTC | ${timestamp} |`, "", "## Entries", "",
  "| Path | Classification | SHA-256 |", "| --- | --- | --- |",
  ...entries.map(([file, kind, hash]) => `| ${file} | ${kind} | ${hash} |`), ""
];
writeFileSync(output, lines.join("\n"));
console.log(`Wrote documentation inventory for ${entries.length} documentation files.`);
