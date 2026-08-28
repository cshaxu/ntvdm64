import fs from 'node:fs';
import path from 'node:path';

const root = path.resolve(process.argv[2] ?? '.');
const architecture = process.argv[3] ?? 'x86';
if (!['x86', 'x64'].includes(architecture)) throw new Error('Architecture must be x86 or x64');
/* Read the freshly generated formal linker log.  The previous sidecar input
 * could survive a regenerated graph and report stale architecture-specific
 * hook forms after a source-shaped binding had entered both candidates. */
const input = path.join(root,
  `build/M0-T310/S2/softpc/${architecture}/original-softpc-forced-closure.dll.log`);
const sourceLedger = path.join(root, 'docs/etc/operations/zero-ledger1-softpc-disposition-ledger.tsv');
const output = path.join(root, `docs/etc/operations/m0-t310-s2-${architecture}-forced-link-ledger.tsv`);

function parseTsv(line) {
  const fields = [];
  let value = '';
  let quoted = false;
  for (let index = 0; index < line.length; index += 1) {
    const character = line[index];
    if (character === '"') {
      if (quoted && line[index + 1] === '"') { value += '"'; index += 1; }
      else { quoted = !quoted; }
    } else if (character === '\t' && !quoted) { fields.push(value); value = ''; }
    else { value += character; }
  }
  fields.push(value);
  return fields;
}

function quote(value) {
  return `"${String(value).replaceAll('"', '""')}"`;
}

function undecorate(symbol) {
  return symbol.replace(/^_+/, '').replace(/@\d+$/, '');
}

const crtForms = new Set([
  'acrt_iob_func', 'stdio_common_vfprintf', 'stdio_common_vsprintf',
  'free', 'malloc', 'longjmp', 'memcpy', 'memset', 'strcat', 'strcpy',
  'atan2', 'cos', 'fabs', 'log', 'pow', 'sin', 'sqrt', 'tan', 'ceil',
  'floor', 'controlfp', 'setjmp3', 'stricmp', 'getenv', 'strcmp', 'strtol',
  'isdigit', 'isxdigit', 'isspace', 'toupper', 'isgraph', 'errno', 'strlen',
  'strchr', 'tolower', 'strncmp', 'strtoul'
]);

const originalSoftpcMachineForms = new Set([
  'BIOS', 'ica_hw_interrupt', 'Ios_in_adapter_table', 'Ios_inb_function',
  'Ios_inw_function', 'Ios_out_adapter_table', 'Ios_outb_function',
  'Ios_outw_function', 'read_pointers', 'host_swint_hook', 'host_exint_hook',
  'host_sas_init', 'host_sas_term', 'rom_init', 'LIM_b_write', 'LIM_w_write',
  'lim_page_frame_init', 'host_timer_event', 'dispatch_q_event', 'ica_intack',
  'host_ica_lock', 'host_ica_unlock', 'InitializeIcaLock'
]);
const originalHostControlForms = new Set([
  'achES', 'BWVKey', 'ConsoleInit', 'ConsoleInitialised', 'DeleteConfigFiles',
  'FdiskTerminatePDB', 'FloppyTerminatePDB', 'host_com_close_all',
  'host_idle_init', 'host_lpt_close_all', 'host_nls_get_msg',
  'host_stream_io_enabled', 'hWndConsole', 'idle_ctl', 'init_host_uis',
  'InitSound', 'MouseAttachMenuItem', 'MouseDetachMenuItem', 'nt_keybd_funcs',
  'nt_remove_event_thread', 'nt_start_event_thread', 'nt_video_funcs',
  'pW32HungAppNotifyThread', 'ResumeTimerThread', 'sc', 'ShowStartGlass',
  'stdoutRedirected', 'SuspendTimerThread', 'szDoomMsg', 'szSysErrMsg',
  'the_mouse_funcs', 'TerminateHeartBeat', 'terminate', 'ThreadInfo',
  'trace_file', 'working_keybd_funcs', 'working_mouse_funcs',
  'working_video_funcs'
]);
const originalProviderForms = new Set([
  'CMDInit', 'DBGInit', 'DemInit', 'GetPIFConfigFiles', 'io_init', 'reset',
  'XMSInit', 'xmsReleaseUMBNotify'
]);
const x86PatchHookForms = new Set([
  'EDL_fast_bop', 'c_VirtualiseInstruction', 'c_sas_touch', 'VdmSetPhysRecStructs'
]);

const sourceRows = fs.readFileSync(sourceLedger, 'utf8').split(/\r?\n/).filter(Boolean);
const sourceHeader = parseTsv(sourceRows.shift());
const sourceColumn = Object.fromEntries(sourceHeader.map((name, index) => [name, index]));
const definitions = new Map();
for (const row of sourceRows) {
  const fields = parseTsv(row);
  const symbol = fields[sourceColumn.symbol];
  if (!symbol) continue;
  const key = symbol.toLowerCase();
  const candidate = `${fields[sourceColumn.source_path]}:${fields[sourceColumn.source_line]} (${fields[sourceColumn.ledger1_disposition]} -> ${fields[sourceColumn.replacement_owner]})`;
  definitions.set(key, [...(definitions.get(key) ?? []), candidate]);
}

const pattern = /^(?<library>[^\(]+)\((?<object>[^\)]+)\).*symbol (?<symbol>\S+)(?: referenced in function (?<function>.+))?$/;
const grouped = new Map();
for (const line of fs.readFileSync(input, 'utf8').split(/\r?\n/)) {
  const match = line.match(pattern);
  if (!match?.groups) continue;
  const { library, object, symbol, function: caller = '' } = match.groups;
  const key = `${library}|${object}|${symbol}|${caller}`;
  grouped.set(key, { library, object, symbol, caller, occurrences: (grouped.get(key)?.occurrences ?? 0) + 1 });
}

function disposition(entry) {
  const normalized = undecorate(entry.symbol);
  if (crtForms.has(normalized)) return ['modern-crt-link', 'MSVC /MT CRT import; no OpenNT source or adapter required'];
  if (x86PatchHookForms.has(normalized)) return ['debugbreak-patch-default', 'fmstubs.c preserves its original immediate debugger-break default (x86 int 3; x64 __debugbreak); a mapping-backed replacement remains disabled pending an owner-approved runtime contract'];
  if (normalized === 'GetPerfCounterUsecs') return ['select-original-softpc-source', 'softpc.new/host/src/nt_timer.c is the original provider; fmstubs.c remains evidence-only and does not replace that provider'];
  if (originalSoftpcMachineForms.has(normalized)) return ['select-original-softpc-machine-source', 'original SoftPC machine source family must be selected by physical owner; do not replace the mechanical body with a local stub'];
  if (originalHostControlForms.has(normalized)) return ['select-original-softpc-host-control-source', 'original SoftPC host-control source family must be selected before a modern public-API binding is considered'];
  if (originalProviderForms.has(normalized)) return ['select-original-mvdm-provider-source', 'select the original MVDM provider package and preserve its entry before adding any adapter binding'];
  const candidates = definitions.get(normalized.toLowerCase()) ?? [];
  if (candidates.length) return ['select-original-mvdm-source', 'physical original source definition is available; select its owner package before any replacement'];
  if (/^(Nt|GetNextVDMCommand|SafeLoadLibrary|ExitVDM|opennt_support_current_teb)/.test(normalized)) return ['adapter-win32-or-host-boundary', 'modern public Win32/NT facade or named OpenNT host boundary required; do not invent a replacement in SoftPC'];
  return ['exact-owner-review', 'no selected original definition found by the current zero-degree ledger; preserve unresolved form for source-first follow-up'];
}

const rows = [...grouped.values()].map((entry) => {
  const normalized = undecorate(entry.symbol);
  const candidates = definitions.get(normalized.toLowerCase()) ?? [];
  const [owner, rule] = disposition(entry);
  return {
    ...entry, normalized, owner, rule,
    candidates: candidates.length ? candidates.join(' | ') : ''
  };
}).sort((left, right) => left.owner.localeCompare(right.owner) || left.symbol.localeCompare(right.symbol) || left.object.localeCompare(right.object));

const header = ['architecture', 'library', 'object', 'caller', 'linker_symbol', 'normalized_symbol', 'occurrences', 'source_first_disposition', 'source_candidates', 'binding_rule'];
const rendered = [header.join('\t'), ...rows.map((entry) => [
  architecture, entry.library, entry.object, entry.caller, entry.symbol, entry.normalized,
  entry.occurrences, entry.owner, entry.candidates, entry.rule
].map(quote).join('\t'))];
fs.writeFileSync(output, `${rendered.join('\n')}\n`);
console.log(`Wrote ${rows.length} physical unresolved forms to ${path.relative(root, output)}`);
