/* Recover an omitted generated CCPU-to-C-VID binding from original metadata. */
import fs from 'node:fs';
import path from 'node:path';

const [output, sourceRoot] = process.argv.slice(2);
if (!output || !sourceRoot) throw new Error('usage: GenerateCvidcCpuBinding.mjs <output> <softpc-base-root>');
const metadata = fs.readFileSync(path.join(sourceRoot, 'cvidc', 'c2cpusad.h'), 'utf8');
const groups = [...metadata.matchAll(/char\s*\*\s*(\w+)\[\]\s*=\s*\{([\s\S]*?)\};/g)];
const slots = Object.fromEntries(groups.slice(0, 2).map(([, name, body]) => [
  name, [...body.matchAll(/"([A-Za-z0-9_]+)"/g)].map((match) => match[1]),
]));
const publicSpecial = {
  Simulate: 'c_cpu_simulate', Interrupt: 'c_cpu_interrupt', ClearHwInt: 'c_cpu_clearHwInt', EndOfApplication: 'c_cpu_EOA_hook', Terminate: 'c_cpu_terminate', Initialise: 'c_cpu_init',
  SetQuickEventCount: 'c_cpu_q_ev_set_count', GetQuickEventCount: 'c_cpu_q_ev_get_count', CalcQuickEventInstTime: 'c_cpu_calc_q_ev_inst_for_time', InitIOS: 'c_cpu_init_ios_in',
  DefineInb: 'c_cpu_define_inb', DefineInw: 'c_cpu_define_inw', DefineInd: 'c_cpu_define_ind', DefineOutb: 'c_cpu_define_outb', DefineOutw: 'c_cpu_define_outw', DefineOutd: 'c_cpu_define_outd',
  GetJumpCalibrateVal: 'mvdm_cvidc_get_jump_calibration', GetJumpInitialVal: 'mvdm_cvidc_get_jump_restart', SetJumpInitialVal: 'mvdm_cvidc_set_jump_restart', SetEOIEnable: 'setEOIEnableAddr',
  SetAddProfileData: 'setAddProfileDataPtr', SetMaxProfileData: 'setMaxProfileDataAddr', GetAddProfileDataAddr: 'getAddProfileDataAddr', PurgeLostIretHookLine: 'PurgeLostIretHookLine',
  ClearHwInt: '0', InitIOS: '0', DefineInb: '0', DefineInw: '0', DefineInd: '0', DefineOutb: '0', DefineOutw: '0', DefineOutd: '0',
  SetSTATUS: 'mvdm_cvidc_set_status', SetAC: '0', GetAC: '0', GetET: '0', GetNE: '0', GetWP: '0',
};
const privateSpecial = {
  InitNanoCpu: 'c_InitNanoCpu', PrepareBlocksToCompile: 'c_PrepareBlocksToCompile',
  GrowRecPool: '0', BpiCompileBPI: '0', FmDeleteAllStructures: '0',
  GetSadInfoTable: '0', GetCpuState: '0', SetCpuState: '0', SetRegConstraint: '0', TrashIntelRegisters: '0',
  InitNanoCpu: '0', PrepareBlocksToCompile: '0', ConstraintsFromUnivEpcPtr: '0', ConstraintsFromUnivHandle: '0',
};
const provider = (name, special) => special[name] ?? `c_${name[0].toLowerCase()}${name.slice(1)}`;
const emit = (macro, names, special) => {
  const slash = String.fromCharCode(92);
  return `#define ${macro}(X) ${slash}\n${names.map((name, index) => `    X(${name}, ${provider(name, special)})${index + 1 === names.length ? '' : ` ${slash}\n`}`).join('')}\n`;
};
const videoHeader = fs.readFileSync(path.join(sourceRoot, 'cvidc', 'evidgen.h'), 'utf8');
// The C-VID binder consumes the selected original CPU40 ABI without inheriting
// its feature macros. Recover declarations and the SAS enum from the original
// headers, rather than maintaining a second handwritten interface.
const cpuHeader = fs.readFileSync(path.join(sourceRoot, 'cvidc', 'cpu4gen.h'), 'utf8');
const sasHeader = fs.readFileSync(path.join(sourceRoot, 'inc', 'sas.h'), 'utf8');
const cpuDeclarations = [...cpuHeader.matchAll(/^\s*IMPORT[^\r\n;]*\bc_[A-Za-z0-9_]+[^\r\n;]*;/gm)].map(m => m[0].trim());
const sasEnum = sasHeader.match(/typedef\s+enum\s*\{\s*SAS_RAM\s*,[\s\S]*?\}\s*SAS_MEM_TYPE\s*;/)?.[0];
if (cpuDeclarations.length !== 197 || !sasEnum) throw new Error('Original CCPU/SAS selected declarations changed');
const accessorSource = fs.readFileSync(path.join(sourceRoot, 'cpu', 'src', 'evid', 'vglob.c'), 'utf8');
const accessors = [...accessorSource.matchAll(/setVideo(\w+) IFN1\(([^,]+), value\)/g)];
if (accessors.length !== 38) throw new Error('Original video accessor profile changed');
const videoCases = [];
const setters = ['sinit011.c', 'sinit012.c', 'sinit013.c'].map(n => fs.readFileSync(path.join(sourceRoot, 'cvidc', n), 'utf8')).join('\n');
for (const [, name, rawType] of accessors) {
  const type = rawType.trim();
  const slotType = videoHeader.match(new RegExp('([\\w]+(?:\\s*\\*)?)\\s*\\(\\*GetVideo' + name + '\\)'))?.[1].trim();
  if (!slotType) throw new Error(`Missing original VideoVector slot: ${name}`);
  const body = setters.match(new RegExp('case\\s+S_\\d+_CiSetVideo' + name + '_\\w+_id\\s*:[\\s\\S]*?(?=case\\s|$)'))?.[0];
  const offset = body?.match(/&\(r20\)\)\s*=\s*\(IS32\)\((\d+)\)/)?.[1];
  if (!offset) throw new Error(`Missing independent generated GDP offset: ${name}`);
  videoCases.push(`CHECK_FIELD(${name}, ${type}, ${offset});`);
}
const text = [
  '/* Generated from original cvidc/c2cpusad.h. Do not edit. */\n',
  `/* public=${slots.CpuVectorNames.length}; private=${slots.CpuPrivateVectorNames.length} */\n`,
  emit('MVDM_CVIDC_CPU_PUBLIC_SLOTS', slots.CpuVectorNames, publicSpecial),
  emit('MVDM_CVIDC_CPU_PRIVATE_SLOTS', slots.CpuPrivateVectorNames, privateSpecial),
].join('');
fs.mkdirSync(path.dirname(output), { recursive: true });
if (!fs.existsSync(output) || fs.readFileSync(output, 'utf8') !== text) fs.writeFileSync(output, text);
const providersOutput = path.join(path.dirname(output), 'cvidc_selected_providers.inc');
const providersText = '/* Original selected CPU/SAS declarations; generated, do not edit. */\n' + sasEnum + '\n#include "sas4gen.h"\n' + cpuDeclarations.join('\n') + '\n';
if (!fs.existsSync(providersOutput) || fs.readFileSync(providersOutput, 'utf8') !== providersText) fs.writeFileSync(providersOutput, providersText);
// Text presentation consumes the original selected V7 declarations. Emit
// source-derived subsets so the authored adapter needs no device profile.
for (const [name, input] of [
  ['gvi', path.join(sourceRoot, 'inc', 'gvi.h')],
  ['egagraph', path.join(sourceRoot, 'inc', 'egagraph.h')],
  ['nt_graph', path.join(sourceRoot, '..', 'host', 'inc', 'nt_graph.h')],
]) {
  const original = fs.readFileSync(input, 'utf8');
  let count = 0;
  const selected = original.replace(/^[ \t]*#ifdef[ \t]+V7VGA[ \t]*\r?\n([\s\S]*?)^[ \t]*#endif[^\r\n]*(?:\r?\n|$)/gm, (_, body) => {
    if (/^[ \t]*#\s*(?:if|ifdef|ifndef|elif)\b/m.test(body)) throw new Error(`Nested V7 condition in ${input}`);
    ++count;
    return body.split(/^[ \t]*#else[^\r\n]*\r?\n/m)[0];
  });
  if (count !== 2) throw new Error(`Unexpected V7 condition count in ${input}: ${count}`);
  const destination = path.join(path.dirname(output), `text_video_${name}.h`);
  if (!fs.existsSync(destination) || fs.readFileSync(destination, 'utf8') !== selected) fs.writeFileSync(destination, selected);
}
const graphics = fs.readFileSync(path.join(sourceRoot, 'inc', 'gfx_upd.h'), 'utf8');
const graphicsPrefix = graphics.match(/^[\s\S]*?}\s*UPDATE_ALG\s*;/)?.[0];
const ports = fs.readFileSync(path.join(sourceRoot, 'inc', 'egaports.h'), 'utf8');
const selectedPlane = ports.match(/#ifdef[ \t]+V7VGA[\s\S]*?^(#define[ \t]+EGA_PLANE_SIZE[ \t]+[^\r\n]+)/m)?.[1];
if (!selectedPlane) throw new Error('Original V7 plane size changed');
const planeOutput = path.join(path.dirname(output), 'v7_selected_plane.h');
if (!fs.existsSync(planeOutput) || fs.readFileSync(planeOutput, 'utf8') !== selectedPlane + '\n') fs.writeFileSync(planeOutput, selectedPlane + '\n');
const selectedPorts = [
  /^\s*extern\s+byte\s*\*EGA_planes\s*;/gm,
  /^\s*#define\s+FONT_BASE_ADDR\s+[^\r\n]+/gm,
  /^\s*#define\s+FONT_MAX_HEIGHT\s+[^\r\n]+/gm,
].map(pattern => {
  const matches = [...ports.matchAll(pattern)];
  if (matches.length !== 1) throw new Error('Original V7 font interface changed');
  return matches[0][0].trim();
});
if (!graphicsPrefix) throw new Error('Original VGA update interface changed');
const textGraphicsOutput = path.join(path.dirname(output), 'text_video_gfx.h');
const textGraphics = graphicsPrefix + '\nextern UPDATE_ALG update_alg;\n' + selectedPorts.join('\n') + '\n';
if (!fs.existsSync(textGraphicsOutput) || fs.readFileSync(textGraphicsOutput, 'utf8') !== textGraphics) fs.writeFileSync(textGraphicsOutput, textGraphics);
const casesOutput = path.join(path.dirname(output), 'cvidc_accessor_cases.inc');
const casesText = '/* Test offsets extracted from original generated setters, not vglob field layout. */\n' + videoCases.join('\n') + '\n';
if (!fs.existsSync(casesOutput) || fs.readFileSync(casesOutput, 'utf8') !== casesText) fs.writeFileSync(casesOutput, casesText);
