import fs from "node:fs";
import path from "node:path";

const root = path.resolve(process.argv[2] ?? process.cwd());
const sourcePath = path.join(root, "src", "mvdm-host", "vdmredir", "vrnetapi.c");
const source = fs.readFileSync(sourcePath, "utf8");

function functionBody(name) {
  const start = source.indexOf(`\n${name}(`);
  if (start < 0) throw new Error(`missing original provider ${name}`);
  const open = source.indexOf("{", start);
  if (open < 0) throw new Error(`missing body for ${name}`);
  let depth = 0;
  for (let index = open; index < source.length; index += 1) {
    if (source[index] === "{") depth += 1;
    if (source[index] === "}" && --depth === 0) return source.slice(open, index + 1);
  }
  throw new Error(`unterminated body for ${name}`);
}

for (const name of [
  "VrGetDomainName",
  "VrGetLogonServer",
  "VrNetGetDCName",
  "VrNetWkstaSetInfo"
]) {
  const body = functionBody(name);
  if (!body.includes("SET_ERROR(ERROR_NOT_SUPPORTED);")) {
    throw new Error(`${name} does not retain ERROR_NOT_SUPPORTED`);
  }
  if (/\b(?:NetWksta|XsNet|RxNet|RxpTransactSmb|VrRemoteApi)\w*\s*\(/.test(body)) {
    throw new Error(`${name} gained a network provider call`);
  }
}

console.log("Redirector source-shaped unavailable contracts verified.");
