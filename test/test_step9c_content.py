"""Pinned map geometry and actual Lua content decisions across controlled seeds.

Usage: DONOR_CLONE LUA_EXE OUTPUT_DIRECTORY. Native map/RNG side effects are
covered separately by the C generator and packaged traversal tests.
"""
from pathlib import Path
import json,re,subprocess,sys
repo=Path(__file__).resolve().parents[1]
donor,lua,out=map(Path,sys.argv[1:4]);out.mkdir(parents=True,exist_ok=True)
pin='17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'
source=subprocess.check_output(['git','show',pin+':dnethack-3.4.3/dat/chaos2.des'],cwd=donor).decode()
blocks={re.match(r'"([^"]+)"',s)[1]:s for s in re.split(r'^MAZE: ',source,flags=re.M)[1:]}
assert set(blocks)=={'chalv2','ossa1','mith1','mith2','mith3','cat1','cat2','cat3'}
fields=['id','coord','buc','spe','material','anarchic','montype','eroded','name']
harness='''local state
local function roll(n) state=(state*48271)%2147483647;return state%n end
function percent(n) return roll(100)<n end
function shuffle(t) for i=#t,2,-1 do local j=roll(i)+1;t[i],t[j]=t[j],t[i] end end
nh={rn2=function() return 0 end}
local function emit(kind,t)
 local values={kind}
 for _,key in ipairs({"id","coord","buc","spe","material","anarchic","montype","eroded","name"}) do
  local v=t[key]
  if key=="coord" and v then v=table.concat(v,",") end
  if kind=="object" and t.id=="statue" and key~="id" and key~="coord" and key~="buc" then v=nil end
  values[#values+1]=v==nil and "" or tostring(v):lower()
 end
 print(table.concat(values,"\\t"))
end
des=setmetatable({object=function(t)emit("object",t)end,monster=function(t)emit("monster",t)end},
 {__index=function()return function()end end})
for seed=1,128 do
 state=seed;math.randomseed(seed);print("SEED "..seed);dofile(arg[1])
end
'''
(out/'capture.lua').write_text(harness)
for name,block in blocks.items():
    local=(repo/'dat'/f'{name}.lua').read_text()
    expected_maps=['\n'.join(re.sub(r'^\d','',line).replace('w','Q') for line in m.splitlines())
                   for m in re.findall(r'^MAP\n(.*?)^ENDMAP',block,re.M|re.S)]
    actual_maps=[m.rstrip('\n') for m in re.findall(r'map=\[=\[\n(.*?)\n\]=\]',local,re.S)]
    assert expected_maps==actual_maps,(name,'geometry')
    clean='\n'.join(line for line in block.splitlines() if not line.lstrip().startswith('#'))
    flags=re.search(r'^FLAGS:\s*(.*)',clean,re.M)[1].replace(' ','').split(',')
    assert re.findall(r'"([^"]+)"',re.search(r'des.level_flags\(([^)]*)\)',local)[1])==flags
    geometry=re.findall(r'^GEOMETRY:\s*(\w+),\s*(\w+)',clean,re.M)
    assert re.findall(r'des.map\(\{halign="(\w+)",valign="(\w+)"',local)==geometry
    init=re.search(r'^INIT_MAP:(.*)',clean,re.M)
    local_init=re.findall(r'des.level_init\(\{style="mines",(.*?)\}\)',local)
    assert len(local_init)==bool(init)
    if init:
        fg,bg,smooth,joined,lit,walled=[s.strip().strip("'") for s in init[1].split(',')]
        expected_init={'fg':fg.replace('w','Q'),'bg':bg.replace('w','Q'),
                       'smoothed':smooth,'joined':joined,'lit':'true' if lit=='lit' else 'false','walled':walled}
        actual_init=dict(re.findall(r'(\w+)=("[^"]*"|\w+)',local_init[0]))
        assert {k:v.strip('"') for k,v in actual_init.items()}==expected_init,(name,'init')
    def numbers(value):return ','.join(str(int(n)) for n in value.split(','))
    expected_portals=[]
    for kind,region,exclude,target in re.findall(r'^(BRANCH|PORTAL):\s*\(([^)]+)\),\s*\(([^)]+)\)(?:,\s*"([^"]+)")?',clean,re.M):
        expected_portals.append((numbers(region),numbers(exclude),kind.lower(),target))
    actual_portals=re.findall(r'des.levregion\(\{region=\{([^}]+)\},exclude=\{([^}]+)\},type="([^"]+)"(?:,name="([^"]+)")?\}\)',local)
    assert actual_portals==expected_portals,(name,'portals')
    for donor_op,local_op in [('STAIR','stair'),('MAZEWALK','mazewalk')]:
        expected=[(direction,numbers(coord)) for coord,direction in re.findall(r'^'+donor_op+r':\s*\(([^)]+)\),\s*(\w+)',clean,re.M)]
        calls=re.findall(r'des.'+local_op+r'\(\{([^\n]+)\}\)',local)
        actual=[(re.search(r'dir="(\w+)"',call)[1],re.search(r'coord=\{([^}]+)\}',call)[1]) for call in calls]
        assert actual==expected,(name,donor_op)
    expected_doors=[(state,numbers(coord)) for state,coord in re.findall(r'^DOOR:\s*(\w+),\s*\(([^)]+)\)',clean,re.M)]
    assert re.findall(r'des.door\("(\w+)",([0-9,]+)\)',local)==expected_doors,(name,'doors')
    expected_shops=[numbers(coord) for coord in re.findall(r'^REGION:\s*\(([^)]+)\),lit,"shop"',clean,re.M)]
    actual_shops=re.findall(r'des.region\(\{region=\{([^}]+)\},lit=1,type=shops\[math.random\(1,4\)\],filled=1\}\)',local)
    assert actual_shops==expected_shops,(name,'shop regions')
    statements=[];inside=False
    for line in block.splitlines():
        if line=='MAP':inside=True
        if inside:
            if line=='ENDMAP':inside=False
            continue
        line=line.strip()
        if not line or line.startswith('#'):continue
        if re.match(r'(OBJECT|MONSTER|RANDOM_PLACES)',line):statements.append(line)
    actual=subprocess.check_output([str(lua),str(out/'capture.lua'),str(repo/'dat'/f'{name}.lua')],text=True)
    (out/f'{name}.txt').write_text(actual)
    runs=re.split(r'^SEED \d+\n',actual,flags=re.M)[1:]
    assert len(runs)==128
    for seed,run in enumerate(runs,1):
        state=seed;places=[];expected=[]
        def roll(n):
            global state
            state=state*48271%2147483647
            return state%n
        for statement in statements:
            if statement.startswith('RANDOM_PLACES:'):
                places=[','.join(str(int(v)) for v in p.split(',')) for p in re.findall(r'\(([^)]+)\)',statement)]
                for i in range(len(places),1,-1):
                    j=roll(i);places[i-1],places[j]=places[j],places[i-1]
                continue
            match=re.match(r'(OBJECT|MONSTER)(?:\[(\d+)%\])?:(.*)',statement);assert match,statement
            kind,prob,body=match.groups()
            if prob and roll(100)>=int(prob):continue
            names=re.findall(r'"([^"]+)"',body)
            entry={};kind=kind.lower()
            if names:entry['id']=names[0]
            elif kind=='object' and "'-'" in body:entry['id']='ceramic tile'
            coordinates=re.search(r'\((\d+),(\d+)\)',body)
            if coordinates:entry['coord']=','.join(str(int(x)) for x in coordinates.groups())
            place=re.search(r'place\[(\d+)\]',body)
            if place:entry['coord']=places[int(place[1])]
            for buc in ['uncursed','blessed','cursed']:
                if re.search(r'\b'+buc+r'\b',body):entry['buc']=buc;break
            if len(names)>1:entry['name']=names[1]
            if re.search(r',\s*0\s*,',body):entry['spe']='0'
            item=entry.get('id')
            if item=='gloves':entry['id']='leather gloves'
            elif item=='metal anarchic quarterstaff':entry.update(id='quarterstaff',material='metal',anarchic='true')
            elif item=='cracked stone mask of a human':entry.update(id='mask',material='mineral',montype='human',eroded='1')
            elif item=='statue':entry={k:v for k,v in entry.items() if k in ['id','coord','buc']}
            expected.append('\t'.join([kind]+[entry.get(k,'').lower() for k in fields]))
        assert run.splitlines()==expected,(name,seed,run.splitlines(),expected)
    print('PASS pinned geometry/flags/init/connectors/shops and 128 actual Lua content runs:',name)
