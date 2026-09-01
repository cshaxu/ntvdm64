import crypto from "node:crypto";
import fs from "node:fs";
import path from "node:path";

const root = path.resolve(process.argv[2] ?? process.cwd());
const canonicalRoot = process.env.OPENNT45_MVDM_ROOT ?? "O:/repos.external/opennt-4.5/nt/private/mvdm";
const output = path.join(root, "docs/etc/operations/m0-t335-s1-wow-source-profile-ledger.tsv");
const oldLedger = path.join(root, "docs/etc/operations/m0-t291-s1-wow-file-subdomain-disposition-ledger.tsv");
const packageRoots = ["wow32", "fax"];

function hash(file) {
  return crypto.createHash("sha256").update(fs.readFileSync(file)).digest("hex").toUpperCase();
}

function walk(directory, result) {
  for (const entry of fs.readdirSync(directory, { withFileTypes: true })) {
    const child = path.join(directory, entry.name);
    if (entry.isDirectory()) walk(child, result);
    else result.push(child);
  }
}

function parseSourceNames(file) {
  const text = fs.readFileSync(file, "utf8");
  const match = text.match(/^SOURCES\s*=\s*([\s\S]*?)(?=^\s*(?:!IF|!ENDIF|[A-Z][A-Z0-9_]*\s*=)|(?![\s\S]))/m);
  if (!match) return new Set();
  return new Set([...match[1].matchAll(/\b([A-Za-z0-9_]+\.(?:c|rc|asm))\b/g)].map(item => item[1].toLowerCase()));
}

function parseOldDisposition() {
  const result = new Map();
  const lines = fs.readFileSync(oldLedger, "utf8").replace(/^\uFEFF/, "").trimEnd().split(/\r?\n/);
  const fields = line => [...line.matchAll(/"([^"]*)"/g)].map(match => match[1]);
  const headings = fields(lines.shift());
  const sourceIndex = headings.indexOf("source_path");
  const dispositionIndex = headings.indexOf("final_product_disposition");
  for (const line of lines) {
    const row = fields(line);
    result.set(row[sourceIndex], row[dispositionIndex]);
  }
  return result;
}

const manifests = new Map([
  ["wow32", new Set([...parseSourceNames(path.join(root, "src/mvdm-host/wow32/sources")), ...parseSourceNames(path.join(root, "src/mvdm-host/wow32/i386/sources"))])],
  ["fax/wowfax", parseSourceNames(path.join(root, "src/mvdm-host/fax/wowfax/sources"))],
  ["fax/wowfaxui", parseSourceNames(path.join(root, "src/mvdm-host/fax/wowfaxui/sources"))]
]);
const inherited = parseOldDisposition();
const files = [];
for (const packageRoot of packageRoots) walk(path.join(root, "src/mvdm-host", packageRoot), files);

function manifestState(relative) {
  const normalized = relative.replaceAll("\\", "/");
  for (const [directory, names] of manifests) {
    if (normalized.startsWith(directory + "/") && names.has(path.posix.basename(normalized).toLowerCase())) {
      return /\.(c|asm)$/i.test(normalized) ? "selected-original-source" : "selected-original-resource";
    }
  }
  if (/\.(obj|pch|res)$/i.test(normalized)) return "mirror-intermediate-or-artifact";
  if (/\.(h|inc|def|mac)$/i.test(normalized)) return "declaration-or-build-carrier";
  return "unselected-source-or-product-carrier";
}

function defaultDisposition(relative, state) {
  if (inherited.has(relative)) return "inherits-" + inherited.get(relative);
  if (state === "mirror-intermediate-or-artifact" || state === "selected-original-resource") return "not-host-runtime";
  if (state === "declaration-or-build-carrier") return "binding-only";
  return "requires-S1-owner-review";
}

const quote = value => `"${String(value).replaceAll('"', '""')}"`;
const headings = ["audit_id", "source_path", "file_kind", "manifest_state", "mirror_identity", "current_sha256", "canonical_source_path", "canonical_sha256", "inherited_or_current_disposition", "S1_note"];
const rows = files.sort().map((file, index) => {
  const relative = path.relative(path.join(root, "src/mvdm-host"), file).replaceAll("\\", "/");
  const canonical = path.join(canonicalRoot, relative);
  const currentHash = hash(file);
  const exists = fs.existsSync(canonical);
  const canonicalHash = exists ? hash(canonical) : "";
  const state = manifestState(relative);
  const kind = path.extname(relative).toLowerCase() || "none";
  return [
    `T335-WOW-${String(index + 1).padStart(3, "0")}`,
    relative,
    kind,
    state,
    exists ? (currentHash === canonicalHash ? "byte-exact" : "divergent") : "canonical-path-not-found",
    currentHash,
    exists ? path.join(canonicalRoot, relative).replaceAll("\\", "/") : "",
    canonicalHash,
    defaultDisposition(relative, state),
    "T335/S1 profile rebaseline; source/body enablement requires a later owner-cohort decision"
  ];
});

fs.writeFileSync(output, "\uFEFF" + [headings, ...rows].map(row => row.map(quote).join("\t")).join("\n") + "\n", "utf8");
const count = (column, value) => rows.filter(row => row[column] === value).length;
console.log(`Wrote ${rows.length} WOW32/FAX rows: ${path.relative(root, output)}`);
console.log(`Identity: ${count(4, "byte-exact")} byte-exact, ${count(4, "divergent")} divergent, ${count(4, "canonical-path-not-found")} canonical-path-not-found.`);
console.log(`Selection: ${count(3, "selected-original-source")} source, ${count(3, "selected-original-resource")} resource, ${count(3, "declaration-or-build-carrier")} declaration/build, ${count(3, "mirror-intermediate-or-artifact")} artifact, ${count(3, "unselected-source-or-product-carrier")} other.`);
