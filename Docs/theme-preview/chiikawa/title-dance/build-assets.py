#!/usr/bin/env python3
"""Losslessly pack the inspected AI-drawn poses; no artwork is redrawn."""
from pathlib import Path
import hashlib,json,shutil,subprocess
from PIL import Image

BASE=Path(__file__).resolve().parent
ROOT=BASE.parents[3]
ART=ROOT/'Themes/chiikawa/Art'
PACK=json.loads((BASE/'packing.json').read_text())
CELL=PACK['cell']

def sha(path): return hashlib.sha256(path.read_bytes()).hexdigest()

def build():
    filters=['[0:v]split=12'+''.join('[s%d]'%i for i in range(12))]
    frames=[]
    for row,boxes in enumerate(PACK['character_bounds']):
        for column,(left,top,right,bottom) in enumerate(boxes):
            index=row*4+column
            margin=PACK['padding']
            x,y=left-margin,top-margin
            w,h=right-left+2*margin,bottom-top+2*margin
            dx=(CELL-w)//2
            dy=PACK['baseline']-(PACK['row_baselines'][row]-y)
            assert dx>=0 and dy>=0 and dx+w<=CELL and dy+h<=CELL
            filters.append(f'[s{index}]crop={w}:{h}:{x}:{y},pad={CELL}:{CELL}:{dx}:{dy}:color=black@0[f{index}]')
            frames.append({'source_crop':[x,y,w,h],'placement':[dx,dy]})
    layout='|'.join(f'{(i%4)*CELL}_{(i//4)*CELL}' for i in range(12))
    filters.append(''.join('[f%d]'%i for i in range(12))+f'xstack=inputs=12:layout={layout}:fill=black@0[out]')
    atlas=ART/'title-dancers 4x3.png'
    subprocess.run(['ffmpeg','-v','error','-y','-i',str(BASE/'characters-source.png'),'-filter_complex',';'.join(filters),'-map','[out]','-frames:v','1','-pix_fmt','rgba',str(atlas)],check=True)
    pads=ART/'title-dance-pads.png'
    shutil.copyfile(BASE/'pads-source.png',pads)
    source=Image.open(BASE/'characters-source.png').convert('RGBA')
    packed=Image.open(atlas).convert('RGBA')
    for i,frame in enumerate(frames):
        x,y,w,h=frame['source_crop'];dx,dy=frame['placement']
        x2=(i%4)*CELL+dx;y2=(i//4)*CELL+dy
        assert source.crop((x,y,x+w,y+h)).tobytes()==packed.crop((x2,y2,x2+w,y2+h)).tobytes()
    assert packed.getchannel('A').getextrema()==(0,255)
    sources=[]
    for name,output in [('characters','exec-e16cb2d8-45d6-4239-9d16-1e4886248813.png'),('pads','exec-fbd9ed17-b757-49c7-a63d-9effc2ec6341.png')]:
        path=BASE/(name+'-source.png');im=Image.open(path)
        sources.append({'file':path.name,'generator':'built-in image_gen','source_output':output,'prompt':name+'-prompt.txt','sha256':sha(path),'size':list(im.size),'mode':im.mode,'alpha':list(im.getchannel('A').getextrema())})
    manifest={'sources':sources,'reference':'../images/01-character-trio.png','characters':['Chiikawa','Hachiware','Usagi'],'runtime_assets':[{'file':str(p.relative_to(ROOT)),'sha256':sha(p),'bytes':p.stat().st_size} for p in [atlas,pads]],'packing':{'cell':CELL,'baseline':PACK['baseline'],'frames':frames,'validation':'Every packed crop equals its source pixels, including alpha; pads copied unchanged'},'animation':'Four drawn poses per character, deterministic 3.2-second stepping/hopping clock; stationary pads; alpha retained','rebuild':'python3 Docs/theme-preview/chiikawa/title-dance/build-assets.py'}
    (BASE/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
    print('Packed 12 transparent poses with exact source-pixel preservation; copied the three cartoon pads unchanged.')

if __name__=='__main__': build()
