import fs from 'node:fs';
import path from 'node:path';

const root = path.resolve(process.argv[2] ?? '.');
const architecture = process.argv[3] ?? 'x86';
const phase = process.argv[4] ?? 's2';
if (!['x86', 'x64'].includes(architecture)) throw new Error('Architecture must be x86 or x64');
if (!['s2', 's4-p4', 's4-p5', 's7-p2'].includes(phase)) throw new Error('Phase must be s2, s4-p4, s4-p5 or s7-p2');
/* Read the freshly generated formal linker log.  The previous sidecar input
 * could survive a regenerated graph and report stale architecture-specific
 * hook forms after a source-shaped binding had entered both candidates. */
const buildDirectory = phase === 's7-p2'
  ? `build/M0-T310/S7/machine/${architecture}`
  : `build/M0-T310/S2/softpc/${architecture}`;
const input = path.join(root, buildDirectory, 'original-softpc-forced-closure.dll.log');
const sourceLedger = path.join(root, 'docs/etc/operations/zero-ledger1-softpc-disposition-ledger.tsv');
const output = path.join(root, `docs/etc/operations/m0-t310-${phase}-${architecture}-forced-link-ledger.tsv`);

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
  'host_ica_lock', 'host_ica_unlock', 'InitializeIcaLock',
  'c_effective_addr', 'ClearInstanceDataMarking',
  'Currently_emulated_video_mode', 'DelayIrqLine', 'handle_ok',
  'host_bop_table', 'ica_clear_int', 'idetect', 'KbdHdwFull', 'PCDisplay',
  'pCharPollsPerTick', 'pICounter', 'pMinConsecutiveTicks', 'restore_map',
  'stream_io_enabled', 'timer_gate_func', 'TimerInt08Off', 'TimerInt08Seg',
  'TimerInt1COff', 'TimerInt1CSeg', 'valid_modes', 'vd_mode_table',
  'video_adapter', 'VirtualX', 'VirtualY'
]);
const originalSoftpcBopForms = new Set([
  'MS_bop_0', 'MS_bop_1', 'MS_bop_2', 'MS_bop_3', 'MS_bop_4', 'MS_bop_5',
  'MS_bop_6', 'MS_bop_7', 'MS_bop_8', 'MS_bop_9', 'MS_bop_A', 'MS_bop_B',
  'MS_bop_C', 'MS_bop_D', 'MS_bop_E', 'MS_bop_F'
]);
const originalXmsForms = new Set(['xmsDisableA20Wrapping', 'xmsEnableA20Wrapping']);
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
  'working_video_funcs', 'host_alarm', 'host_memset', 'SPC_Product_Name'
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
  if (x86PatchHookForms.has(normalized)) return ['debugbreak-patch-default', 'fmstubs.c preserves its original immediate debugger-break default through __debugbreak() on both x86 and x64; no replacement implementation is retained'];
  if (normalized === 'GetPerfCounterUsecs') return ['select-original-softpc-source', 'softpc.new/host/src/nt_timer.c is the original provider; fmstubs.c remains evidence-only and does not replace that provider'];
  if (originalSoftpcMachineForms.has(normalized)) return ['select-original-softpc-machine-source', 'select the physical original SoftPC machine owner for the SoftPC-selected path; do not substitute Bochs or a local stub'];
  if (originalSoftpcBopForms.has(normalized)) return ['select-original-softpc-bop-source', 'select the original SoftPC host BOP owner and preserve its source dispatch contract; do not create a parallel BOP handler'];
  if (originalXmsForms.has(normalized)) return ['select-original-xms-source', 'select the original XMS provider and retain its source control flow before adding only the required memory/A20 adapter seam'];
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
