import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';

// Reuse the project's C lexical boundary audit: comments/string bodies are
// blanked without changing offsets. This checks a reviewed project-only slice,
// not provenance of original code or correctness of the complete service.
function definitions(file) {
  const raw=fs.readFileSync(file,'utf8').replaceAll('\r\n','\n');
  const blank=s=>s.replace(/[^\n]/g,' ');
  const clean=raw.replace(/\/\*[\s\S]*?\*\/|\/\/[^\n]*|"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'/g,blank)
    .replace(/^\s*#[^\n]*/gm,blank);
  const rows=new Map();let depth=0,start=0,open=0,name='';
  for(let i=0;i<clean.length;i++) {
    if(clean[i]==='{') {
      if(!depth) {name=clean.slice(start,i).match(/\b([A-Za-z_]\w*)\s*\([^{};]*\)\s*$/)?.[1]??'';open=i;}
      depth++;
    } else if(clean[i]==='}') {
      if(--depth===0) {
        if(name) {if(rows.has(name))throw Error('Duplicate definition '+name);rows.set(name,raw.slice(open,i+1));}
        start=i+1;name='';
      }
    } else if(clean[i]===';' && !depth)start=i+1;
  }
  if(depth)throw Error('Unbalanced source '+file);
  return rows;
}
const predecessor=process.argv[2];
if(!predecessor)throw Error('Pass the retained pre-extraction source snapshot under build/');
const build=path.resolve('build')+path.sep;
if(!path.resolve(predecessor).toLowerCase().startsWith(build.toLowerCase()))throw Error('Snapshot must be under build/');
const pairs=[
  ['OpenNtBaseClientWorkerFrontendCapability','worker_frontend_capability'],
  ['OpenNtBaseClientAcquireConsoleContext','acquire_console_context'],
  ['OpenNtBaseClientBindConsoleContext','bind_console_context'],
  ['OpenNtBaseClientRegisterNativeBackend','register_native_backend'],
  ['OpenNtBaseClientWorkerShutdownEvent','worker_shutdown_event'],
  ['OpenNtBaseClientWorkerStateChanged','worker_state_changed'],
  ['take_frontend','take_frontend']
];
const before=definitions(predecessor);
const common=definitions('src/common/rpc/worker_control.c');
const facade=definitions('src/ntsrv-exe/opennt/source/base_rpc_client.c');
const normalize=s=>s.replace(/\s+/g,' ').trim();
const adapt=s=>s.replaceAll('client.','state->')
  .replace(/if\s*\(!state->connection \|\| !state->binding \|\| !state->process\)/g,
    'if(!state || !state->connection || !state->binding || !state->process)');
function verify(providers,facades) {
  for(const [prior,name] of pairs) {
    const body=providers.get('common_rpc_'+name),old=before.get(prior),forward=facades.get(prior);
    if(!body || !old || !forward)throw Error('Missing reviewed provider/facade '+name);
    if(normalize(body)!==normalize(adapt(old)))throw Error('Changed exchange contract '+name);
    if(!forward.includes('common_rpc_'+name+'(') || /RpcTryExcept|Client_[A-Za-z]|CloseHandle/.test(forward))
      throw Error('Duplicated exchange or ownership in facade '+prior);
  }
}
verify(common,facade);
let negative=0;
for(const [,name] of pairs) {
  const changed=new Map(common);
  changed.set('common_rpc_'+name,changed.get('common_rpc_'+name).replace('return error;','return ERROR_SUCCESS;'));
  try {verify(changed,facade);throw Error('Negative mutation was accepted '+name);}
  catch(error) {if(!error.message.startsWith('Changed exchange contract'))throw error;negative++;}
}
for(const [prior] of pairs) {
  const changed=new Map(facade);changed.set(prior,changed.get(prior)+'\nClient_ForbiddenDuplicate();');
  try {verify(common,changed);throw Error('Duplicate facade was accepted '+prior);}
  catch(error) {if(!error.message.startsWith('Duplicated exchange'))throw error;negative++;}
}
const hash=file=>crypto.createHash('sha256').update(fs.readFileSync(file)).digest('hex');
console.log(JSON.stringify({result:'PASS',reviewedExchanges:pairs.length,negativeControls:negative,
  predecessor:path.resolve(predecessor),predecessorSha256:hash(predecessor),
  providerSha256:hash('src/common/rpc/worker_control.c'),
  limitation:'Reviewed project adaptation only; not full source provenance, authentication or runtime proof'},null,2));
