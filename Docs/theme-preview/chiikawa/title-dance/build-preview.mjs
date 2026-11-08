// Render the real Lua motion samples in a transparent browser canvas, then encode GIF.
// Run from the repo root: node Docs/theme-preview/chiikawa/title-dance/build-preview.mjs /path/to/bundled/lua
import fs from 'node:fs';import os from 'node:os';import path from 'node:path';import crypto from 'node:crypto';
import {spawn,spawnSync} from 'node:child_process';import {fileURLToPath,pathToFileURL} from 'node:url';
const dir=path.dirname(fileURLToPath(import.meta.url));const root=path.resolve(dir,'../../../..');
const lua=process.argv[2];if(!lua)throw Error('Pass the bundled Lua interpreter path.');
const output=spawnSync(lua,[path.join(dir,'export-motion.lua')],{cwd:root,encoding:'utf8'});if(output.status!==0)throw Error(output.stderr);
fs.writeFileSync(path.join(dir,'motion-data.js'),output.stdout);
const data=JSON.parse(output.stdout.slice('window.titleDanceData='.length).replace(/;\s*$/,''));
const base=path.join(root,'out/title-dance/browser');fs.mkdirSync(base,{recursive:true});
const profile=fs.mkdtempSync(path.join(os.tmpdir(),'stepmania-title-dance-'));
const chrome=spawn(process.env.TITLE_DANCE_CHROME||'/usr/bin/google-chrome',['--headless=new','--no-sandbox','--disable-background-networking','--disable-extensions','--no-first-run','--hide-scrollbars','--allow-file-access-from-files','--remote-debugging-port=0','--user-data-dir='+profile,'about:blank'],{stdio:'ignore'});
const delay=ms=>new Promise(r=>setTimeout(r,ms));let ws;
try{
 const portFile=path.join(profile,'DevToolsActivePort');
 for(let i=0;!fs.existsSync(portFile)&&i<100;i++)await delay(100);
 if(!fs.existsSync(portFile))throw Error('Chrome did not start.');
 const port=fs.readFileSync(portFile,'utf8').split('\n')[0];
 const tabs=await (await fetch('http://127.0.0.1:'+port+'/json/list')).json();
 ws=new WebSocket(tabs.find(t=>t.type==='page').webSocketDebuggerUrl);
 await new Promise((resolve,reject)=>{ws.onopen=resolve;ws.onerror=reject;});
 let sequence=0;const pending=new Map();
 ws.onmessage=e=>{const m=JSON.parse(e.data);if(pending.has(m.id)){const p=pending.get(m.id);pending.delete(m.id);m.error?p.reject(Error(JSON.stringify(m.error))):p.resolve(m.result);}};
 const call=(method,params={})=>new Promise((resolve,reject)=>{const id=++sequence;pending.set(id,{resolve,reject});ws.send(JSON.stringify({id,method,params}));});
 await call('Page.enable');await call('Runtime.enable');
 await call('Emulation.setDeviceMetricsOverride',{width:data.width,height:data.height,deviceScaleFactor:1,mobile:false});
 await call('Emulation.setDefaultBackgroundColorOverride',{color:{r:0,g:0,b:0,a:0}});
 await call('Page.navigate',{url:pathToFileURL(path.join(dir,'preview.html')).href+'?export=1'});
 let ready=false;
 for(let i=0;i<100&&!ready;i++){const r=await call('Runtime.evaluate',{expression:'window.titleDanceReady===true',returnByValue:true});ready=r.result.value===true;if(!ready)await delay(100);}
 if(!ready)throw Error('Preview assets did not load.');
 for(let i=0;i<=data.frames.length;i++){
  await call('Runtime.evaluate',{expression:'window.renderTitleDanceFrame('+i+')'});
  const shot=await call('Page.captureScreenshot',{format:'png',captureBeyondViewport:false});
  fs.writeFileSync(path.join(base,'frame-'+String(i).padStart(3,'0')+'.png'),Buffer.from(shot.data,'base64'));
 }
 const hash=p=>crypto.createHash('sha256').update(fs.readFileSync(p)).digest('hex');
 if(hash(path.join(base,'frame-000.png'))!==hash(path.join(base,'frame-'+String(data.frames.length).padStart(3,'0')+'.png')))throw Error('Rendered loop endpoint does not match.');
 const result=spawnSync('ffmpeg',['-v','error','-y','-framerate',String(data.fps),'-i',path.join(base,'frame-%03d.png'),'-filter_complex','[0:v]split[a][b];[a]palettegen=reserve_transparent=1:stats_mode=full[p];[b][p]paletteuse=alpha_threshold=128:dither=sierra2_4a','-frames:v',String(data.frames.length),'-loop','0','-gifflags','-offsetting',path.join(dir,'title-dance.gif')],{encoding:'utf8'});
 if(result.status!==0)throw Error(result.stderr);
 fs.copyFileSync(path.join(base,'frame-000.png'),path.join(dir,'title-dance-still.png'));
 const record=p=>({file:path.relative(root,p),sha256:hash(p),bytes:fs.statSync(p).size});
 const manifest={duration_seconds:data.duration,nominal_fps:data.fps,frames:data.frames.length,width:data.width,height:data.height,loop:'infinite',transparency:'GIF palette alpha; native PNG uses full alpha',rendered_endpoint_matches_start:true,rendered_endpoint_sha256:hash(path.join(base,'frame-000.png')),inputs:[path.join(root,'Themes/chiikawa/Scripts/07 TitleDance.lua'),path.join(root,'Themes/chiikawa/Art/title-dancers 4x3.png'),path.join(root,'Themes/chiikawa/Art/title-dance-pads.png'),...['export-motion.lua','motion-data.js','preview.html','build-preview.mjs'].map(p=>path.join(dir,p))].map(record),outputs:['title-dance.gif','title-dance-still.png'].map(p=>record(path.join(dir,p)))};
 fs.writeFileSync(path.join(dir,'preview-manifest.json'),JSON.stringify(manifest,null,2)+'\n');
 console.log('Exported '+data.frames.length+' frames, '+data.duration+' seconds, transparent looping GIF. Exact rendered loop endpoint matches.');
}finally{if(ws)ws.close();chrome.kill('SIGTERM');await delay(300);fs.rmSync(profile,{recursive:true,force:true});}
