/* Step 15A: actual engine, RNG, Lua, apply dispatch and native codecs. */
#include "hack.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
extern void init_isaac64(unsigned long, int (*)(int));
extern void step15_generate(void);
extern void step15b_test_main(void);
extern void step15b_window_setup(void);
extern int step15_forge_result;
extern void step15_invoke_position(coordxy, coordxy, int);
static void free_level(void) {
    NHFILE *f=get_freeing_nhfile();
    if(iflags.purge_monsters)dmonsfree();
    savelev(f,-1);close_nhfile(f);
}
static int messages;
static void message(const char *s) { if (strstr(s, "no forge operations")) ++messages; }
static void file_mode(NHFILE *f, int mode, int fd) {
    assert(fd >= 0); init_nhfile(f);f->ftype=NHF_LEVELFILE;f->fnidx=historical;
    f->fd=fd; f->mode=mode; f->structlevel=TRUE;
    f->fieldlevel=FALSE; f->addinfo=FALSE;
}
static void eligible_tests(void) {
    int i, next, dls[]={19,20,199,200};
    s_level fake={0}; branch br={0};
    s_level *old=svs.sp_levchn; branch *oldbr=svb.branches;
    u.uz.dnum=medusa_level.dnum;
    svs.sp_levchn=NULL;svb.branches=NULL;
    for(i=0;i<4;++i) { u.uz.dlevel=dls[i];assert(forge_eligible()==(i==1||i==2)); }
    u.uz.dlevel=40;
    fake.dlevel=u.uz;svs.sp_levchn=&fake;assert(!forge_eligible());svs.sp_levchn=NULL;
    br.end1=u.uz;svb.branches=&br;assert(!forge_eligible());svb.branches=NULL;
    svd.dungeons[u.uz.dnum].proto[0]='x';assert(!forge_eligible());svd.dungeons[u.uz.dnum].proto[0]=0;
    svd.dungeons[u.uz.dnum].fill_lvl[0]='x';assert(!forge_eligible());svd.dungeons[u.uz.dnum].fill_lvl[0]=0;
    u.uz.dnum=mines_dnum;assert(!forge_eligible());
    init_isaac64(151UL,rn2);next=rn2(100000);init_isaac64(151UL,rn2);
    assert(forge_generate()==-1);assert(rn2(100000)==next);
    u.uz.dnum=medusa_level.dnum;
    /* No rooms: both selected failure and missed selection consume exactly
       one draw, independent of topology. Each possible draw is observed. */
    { int seen[100]={0}, n=0, seed, draw, result;
      svn.nroom=0;
      for(seed=1;n<100;++seed) {
        init_isaac64(seed,rn2);draw=rn2(100);next=rn2(100000);
        init_isaac64(seed,rn2);result=forge_generate();
        assert(result==(draw<12?1:0));assert(rn2(100000)==next);
        if(!seen[draw]) {seen[draw]=1;++n;}
      }
    }
    svs.sp_levchn=old;svb.branches=oldbr;
    puts("PASS depth/branch/authored boundaries; exact 12/100 gate and one draw without rooms");
}
static void candidates(void) {
    int typ; struct mkroom *r=&svr.rooms[0]; struct trap *t;
    memset(r,0,sizeof *r);svn.nroom=1;r->lx=10;r->hx=15;r->ly=5;r->hy=9;
    levl[12][7].roomno=ROOMOFFSET;levl[12][7].typ=ROOM;
    assert(forge_candidate(12,7));
    for(typ=0;typ<MAX_TYPE;++typ) {levl[12][7].typ=typ;assert(forge_candidate(12,7)==(typ==ROOM));}
    levl[12][7].typ=ROOM;
    for(typ=1;typ<=SHOPBASE+20;++typ) {r->rtype=typ;assert(!forge_candidate(12,7));}r->rtype=OROOM;
    r->custom_id=CUSTOM_LIBRARY;assert(!forge_candidate(12,7));r->custom_id=0;
    r->nsubrooms=1;assert(!forge_candidate(12,7));r->nsubrooms=0;
    r->irregular=1;assert(!forge_candidate(12,7));r->irregular=0;
    r->orig_rtype=TEMPLE;assert(!forge_candidate(12,7));r->orig_rtype=OROOM;
    levl[12][7].roomno=ROOMOFFSET+svn.nroom;
    assert(!forge_candidate(12,7));levl[12][7].roomno=ROOMOFFSET;
    r->lx=13;assert(!forge_candidate(12,7));r->lx=10;
    {d_level dest=u.uz;
     stairway_add(12,7,FALSE,FALSE,&dest);assert(!forge_candidate(12,7));stairway_free_all();
     stairway_add(12,7,FALSE,TRUE,&dest);assert(!forge_candidate(12,7));stairway_free_all();}
    levl[12][7].edge=1;assert(!forge_candidate(12,7));levl[12][7].edge=0;
    levl[12][7].roomno=NO_ROOM;assert(!forge_candidate(12,7));levl[12][7].roomno=ROOMOFFSET;
    for(typ=1;typ<TRAPNUM;++typ) {
        t=maketrap(12,7,typ); if(t) { assert(!forge_candidate(12,7));deltrap(t); }
        levl[12][7].typ=ROOM;
    }
    assert(forge_candidate(12,7));
    forge_exclude_area(12,7,12,7);assert(!forge_candidate(12,7));
    puts("PASS natural candidate terrain, room types, custom/subroom/edge, scripted areas and traps");
}
static void reservoir_tests(void) {
    static const coord legal[]={{11,6},{12,8},{15,7}};
    struct mkroom *r;
    int seed, i, gate, draws[3], chosen, next, result, hits[3]={0};
    s_level *old=svs.sp_levchn;branch *oldbr=svb.branches;
    free_level();clear_level_structures();
    svs.sp_levchn=NULL;svb.branches=NULL;
    u.uz.dnum=medusa_level.dnum;u.uz.dlevel=40;
    r=&svr.rooms[0];svn.nroom=1;r->lx=10;r->hx=15;r->ly=5;r->hy=9;
    for(seed=1;seed<=1000;++seed) {
        for(i=0;i<SIZE(legal);++i) {
            levl[legal[i].x][legal[i].y].typ=ROOM;
            levl[legal[i].x][legal[i].y].roomno=ROOMOFFSET;
        }
        init_isaac64(seed,rn2);gate=rn2(100);chosen=-1;
        if(gate<12) {
            for(i=0;i<SIZE(legal);++i)draws[i]=rn2(i+1);
            /* The final zero replacement wins. Expected coordinates are
             * fixed fixture data, not discovered with forge_candidate. */
            for(i=SIZE(legal)-1;i>=0;--i) if(!draws[i]) {chosen=i;break;}
        }
        next=rn2(100000);init_isaac64(seed,rn2);
        result=forge_generate();
        assert(result==(chosen<0?0:2));assert(rn2(100000)==next);
        for(i=0;i<SIZE(legal);++i)
            assert((levl[legal[i].x][legal[i].y].typ==FORGE)==(i==chosen));
        if(chosen>=0)++hits[chosen];
    }
    assert(hits[0]&&hits[1]&&hits[2]);
    svs.sp_levchn=old;svb.branches=oldbr;
    puts("PASS 1000 exact sparse-candidate reservoir selections and native RNG tails");
}
static void scripted_room(void) {
    int x, y, candidates=0;
    nhl_sandbox_info sbi={NHL_SB_SAFE,1024*1024,0,1024*1024};
    lua_State *L=nhl_init(&sbi);assert(L);create_des_coder();
    free_level();clear_level_structures();
    assert(luaL_loadstring(L,"des.level_init({style='solidfill',fg=' '}); des.room({type='ordinary',filled=1,w=8,h=5,contents=function() end})")==LUA_OK);
    assert(lua_pcall(L,0,0,0)==LUA_OK);nhl_done(L);
    level_finalize_topology();assert(svn.nroom==1 && svr.rooms[0].rtype==OROOM);
    for(x=1;x<COLNO;++x)for(y=0;y<ROWNO;++y)candidates+=forge_candidate(x,y);
    assert(!candidates);
    puts("PASS scripted Lua ordinary room excluded by transient provenance");
}
static void provenance_tests(void) {
    nhl_sandbox_info sbi={NHL_SB_SAFE,1024*1024,0,1024*1024};
    lua_State *L=nhl_init(&sbi);
    int x,y,i,n;
    static const char *scripts[]={
        "des.room({type='ordinary',filled=1})",
        "des.room({type='ordinary',filled=1,xalign='left'})",
        "des.room({type='ordinary',filled=1,yalign='top'})"
    };
    assert(L);create_des_coder();
    for(i=0;i<SIZE(scripts);++i) {
        free_level();clear_level_structures();
        assert(luaL_loadstring(L,"des.level_init({style='solidfill',fg=' '})")==LUA_OK);
        assert(lua_pcall(L,0,0,0)==LUA_OK);
        assert(luaL_loadstring(L,scripts[i])==LUA_OK);
        assert(lua_pcall(L,0,0,0)==LUA_OK);
        level_finalize_topology();n=0;
        for(x=1;x<COLNO;++x)for(y=0;y<ROWNO;++y)n+=forge_candidate(x,y);
        if(i==0)assert(n>0);else assert(!n);
    }
    nhl_done(L);
    free_level();clear_level_structures();
    add_room(30,5,35,9,FALSE,OROOM,FALSE);
    add_room(10,5,15,9,FALSE,OROOM,FALSE);
    forge_exclude_area(12,7,12,7);
    sort_rooms();level_finalize_topology();
    assert(!forge_candidate(12,7));assert(forge_candidate(13,7));
    free_level();clear_level_structures();
    add_room(10,5,15,9,FALSE,OROOM,FALSE);level_finalize_topology();
    assert(forge_candidate(12,7));
    puts("PASS 5 provenance cases: random/aligned Lua rooms, sorted coordinates and level reset");
}
static void terrain_activation(void) {
    struct obj *h=mksobj(WAR_HAMMER,FALSE,FALSE), *o=mksobj(DAGGER,FALSE,FALSE), saved;
    struct rm terrain; int i, before;
    nhl_sandbox_info sbi = {NHL_SB_SAFE, 1024*1024, 0, 1024*1024};
    lua_State *L=nhl_init(&sbi);
    assert(L);create_des_coder();gx.xstart=gy.ystart=0;
    assert(luaL_loadstring(L,"des.terrain({ x=20, y=10, typ='f', lit=0 }); des.region({ region={19,9,21,11}, lit=0, type='ordinary' })")==LUA_OK);
    assert(lua_pcall(L,0,0,0)==LUA_OK);nhl_done(L);
    u.ux=20;u.uy=10;u.uz.dnum=medusa_level.dnum;u.uz.dlevel=40;
    assert(levl[20][10].typ==FORGE && levl[20][10].lit);
    assert(splev_chr2typ('f')==FORGE);
    assert(ACCESSIBLE(FORGE)&&SPACE_POS(FORGE)&&IS_ROOM(FORGE)&&IS_FURNITURE(FORGE));
    assert(!IS_OBSTRUCTED(FORGE)&&!IS_WALL(FORGE)&&!IS_POOL(FORGE)&&!IS_LAVA(FORGE));
    assert(!IS_FOUNTAIN(FORGE)&&!IS_SINK(FORGE)&&!IS_ALTAR(FORGE));
    assert(!strcmp(surface(20,10),"forge"));
    {char buf[BUFSZ];const char *name=dfeature_at(20,10,buf);assert(name && !strcmp(name,"forge"));}
    assert(back_to_glyph(20,10)==cmap_to_glyph(S_forge));
    assert(!strcmp(defsyms[S_forge].explanation,"forge"));
    assert(defsyms[S_forge].sym=='{' && defsyms[S_forge].color==CLR_ORANGE);
    for(i=0;i<MAX_TYPE;++i) if(i!=FORGE) {assert(!set_levltyp(20,10,i));assert(levl[20][10].typ==FORGE);}
    assert(!maketrap(20,10,PIT));assert(!maketrap(20,10,HOLE));
    assert(dig_check(&gy.youmonst,20,10)==DIGCHECK_FAIL_TOOHARD);
    assert(dig_check(NULL,20,10)==DIGCHECK_FAIL_TOOHARD);
    litroom(FALSE,NULL);assert(levl[20][10].lit);
    {struct monst shadow={0};shadow.data=&mons[PM_SHADOW];shadow.mx=20;shadow.my=10;
     m_postmove_effect(&shadow);assert(levl[20][10].lit);}
    svl.lastseentyp[20][10]=FORGE;classify_terrain();assert(iflags.terrain_typ==FORGE);
    place_object(o,20,10);assert(svl.level.objects[20][10]==o);
    h=addinv(h);h->known=h->dknown=1;
    terrain=levl[20][10];before=messages;
    for(i=0;i<5;++i) {
        HConfusion=i==1;HStun=i==2;ABASE(A_STR)=i==3?3:4;
        saved=*h;
        cmdq_add_key(CQ_CANNED,h->invlet);
        assert(doapply()==ECMD_OK); /* 15B menu cancellation is free */
        assert(!memcmp(&saved,h,sizeof saved));
        assert(!memcmp(&terrain,&levl[20][10],sizeof terrain));
        assert(svl.level.objects[20][10]==o);
    }
    assert(messages==before);
    HConfusion=HStun=0;ABASE(A_STR)=AMAX(A_STR)=18;
    assert(enhancement_set(h,OEP_FIRE,OQ_EXCEPTIONAL,FALSE));
    h->spe=-5;h->cursed=1;h->oeroded=2;saved=*h;
    cmdq_add_key(CQ_CANNED,h->invlet);assert(doapply()==ECMD_OK);assert(!memcmp(&saved,h,sizeof saved));
    h->oartifact=ART_MJOLLNIR;saved=*h;
    cmdq_add_key(CQ_CANNED,h->invlet);assert(doapply()==ECMD_OK);assert(!memcmp(&saved,h,sizeof saved));
    h->oartifact=0;
    assert(forge_interact(o)==ECMD_FAIL);
    levl[20][10].typ=ROOM;before=messages;
    cmdq_add_key(CQ_CANNED,h->invlet);assert(doapply()==ECMD_CANCEL);assert(messages==before);
    levl[20][10]=terrain;
    freeinv(h);obfree(h,NULL);
    puts("PASS Lua, display, classification, lighting, permanence, objects and actual apply gates/timing/nonmutation");
}
static void persistence(void) {
    NHFILE *f;char *id, why[BUFSZ], count;int seed;
    u.uz.dlevel=10;
    f=get_freeing_nhfile();file_mode(f,WRITING|FREEING,open("step15-level.tmp",O_CREAT|O_TRUNC|O_WRONLY|O_BINARY,_S_IREAD|_S_IWRITE));
    savelev(f,ledger_no(&u.uz));close_nhfile(f);
    f=get_freeing_nhfile();file_mode(f,READING,open("step15-level.tmp",O_RDONLY|O_BINARY));
    getlev(f,0,ledger_no(&u.uz));close_nhfile(f);
    assert(levl[20][10].typ==FORGE&&levl[20][10].lit&&svl.level.objects[20][10]);
    f=create_bonesfile(&u.uz,&id,why);assert(f);count=(char)(strlen(id)+1);f->mode=WRITING;store_version(f);
    Sfo_char(f,svn.nhuuid,"ancestor-nhuuid",sizeof svn.nhuuid);Sfo_char(f,&count,"bones_count",1);Sfo_char(f,id,"bonesid",count);
    savefruitchn(f);savelev(f,ledger_no(&u.uz));close_nhfile(f);commit_bonesfile(&u.uz);
    free_level();flags.bones=TRUE;wizard=FALSE;
    for(seed=1;seed<1000;++seed) {init_isaac64(seed,rn2);if(!rn2(3))break;}
    init_isaac64(seed,rn2);step15_forge_result=-99;mklev();
    assert(step15_forge_result==-99);assert(levl[20][10].typ==FORGE&&levl[20][10].lit);
    flags.bones=FALSE;
    puts("PASS native savelev/getlev and accepted mklev/getbones; no generation on bones");
}
static void shop_forges(void) {
    nhl_sandbox_info sbi={NHL_SB_SAFE,1024*1024,0,1024*1024};
    lua_State *L;
    free_level();
    L=nhl_init(&sbi);assert(L);create_des_coder();gx.xstart=gy.ystart=0;
    assert(luaL_loadstring(L,
        "des.level_init({style='solidfill',fg='.'}); "
        "des.terrain({x=12,y=7,typ='f'}); "
        "des.region({region={10,5,15,9},type='shop',filled=0}); "
        "des.terrain({x=13,y=7,typ='f'});") == LUA_OK);
    assert(lua_pcall(L,0,0,0)==LUA_OK);
    assert(!IS_FORGE(levl[12][7].typ)); /* region completion, before finalize */
    svr.rooms[0].irregular=TRUE;
    levl[14][7].roomno=ROOMOFFSET;levl[14][7].edge=0;
    assert(luaL_loadstring(L,"des.map({x=14,y=7,map='f.'})")==LUA_OK);
    assert(lua_pcall(L,0,0,0)==LUA_OK);nhl_done(L);
    assert(!IS_FORGE(levl[14][7].typ));
    assert(levl[14][7].roomno==ROOMOFFSET); /* rejected before map clears it */
    levl[12][7].typ=FORGE; /* final topology also repairs authored conflicts */
    level_finalize_topology();
    assert(!IS_FORGE(levl[12][7].typ));
    assert(!IS_FORGE(levl[13][7].typ));
    assert(!set_levltyp(14,7,FORGE));
    puts("PASS no forge in shops: Lua ordering, irregular map membership, terrain setter and final topology");
}
/* Exercise mutators, not only the common terrain setter.  These Lua
 * entry points historically wrote typ directly and bypassed permanence. */
static void terrain_mutators(void) {
    static const char *ops[] = {
        "des.ladder({x=20,y=10,dir='down'})",
        "des.stair({x=20,y=10,dir='down'})",
        "des.grave({x=20,y=10,text='must not replace a forge'})",
        "des.door({x=20,y=10,state='closed'})",
        "des.mazewalk({x=19,y=10,dir='east',stocked=0})",
        "des.terrain({x=20,y=10,typ='.',lit=0})",
        "des.replace_terrain({region={20,10,20,10},fromterrain='f',toterrain='.',lit=0})",
        "des.map({x=20,y=10,map='..'})"
    };
    nhl_sandbox_info sbi={NHL_SB_SAFE,1024*1024,0,1024*1024};
    lua_State *L;
    int i, failures=0;
    boolean oldwizard=wizard;
    char wish[]="fountain";
    struct obj *result;
    free_level();
    L=nhl_init(&sbi);assert(L);create_des_coder();
    for(i=0;i<SIZE(ops);++i) {
        free_level();clear_level_structures();gx.xstart=gy.ystart=0;
        assert(luaL_loadstring(L,"des.level_init({style='solidfill',fg='.'}); des.terrain({x=20,y=10,typ='f'})")==LUA_OK);
        assert(lua_pcall(L,0,0,0)==LUA_OK);
        assert(luaL_loadstring(L,ops[i])==LUA_OK);
        assert(lua_pcall(L,0,0,0)==LUA_OK);
        if(levl[20][10].typ!=FORGE || !levl[20][10].lit
           || stairway_at(20,10) || engr_at(20,10)) {
            printf("FAIL forge permanence: %s\n",ops[i]);++failures;
        }
    }
    free_level();clear_level_structures();gx.xstart=gy.ystart=0;
    assert(luaL_loadstring(L,"des.level_init({style='solidfill',fg=' '}); des.terrain({x=20,y=10,typ='f'}); des.region({region={20,10,20,10},type='ordinary',irregular=true,lit=0,filled=0})")==LUA_OK);
    assert(lua_pcall(L,0,0,0)==LUA_OK);
    if(levl[20][10].typ!=FORGE || !levl[20][10].lit) {
        puts("FAIL forge illumination: dark irregular Lua region");++failures;
    }
    nhl_done(L);
    levl[20][10].typ=FORGE;levl[20][10].lit=1;levl[21][10].typ=VWALL;
    if(create_drawbridge(20,10,DB_EAST,TRUE) || levl[20][10].typ!=FORGE) {
        puts("FAIL forge permanence: create_drawbridge");++failures;
    }
    levl[20][10].typ=FORGE;u.ux=20;u.uy=10;wizard=TRUE;
    result=readobjnam(wish,(struct obj *) 0);
    if(result && result!=&hands_obj) obfree(result,NULL);
    wizard=oldwizard;
    if(levl[20][10].typ!=FORGE) {
        puts("FAIL forge permanence: wizard terrain wish");++failures;
    }
    /* Room construction and postprocessing can follow an explicit placement.
     * Place forges at every wall/corner/interior position of a new room. */
    free_level();clear_level_structures();
    {int x,y;
     for(x=19;x<=23;++x)for(y=9;y<=13;++y) {
         levl[x][y].typ=FORGE;levl[x][y].lit=1;
     }
     add_room(20,10,22,12,FALSE,OROOM,FALSE);
     for(x=19;x<=23;++x)for(y=9;y<=13;++y)
         if(levl[x][y].typ!=FORGE) {puts("FAIL forge permanence: room construction");++failures;}}
    free_level();clear_level_structures();
    for(i=0;i<=6;++i) {
        levl[20][10].typ=FORGE;levl[20][10].lit=1;
        step15_invoke_position(20,10,i);
        if(levl[20][10].typ!=FORGE || !levl[20][10].lit || t_at(20,10)) {
            printf("FAIL forge permanence: invocation distance %d\n",i);++failures;
            if(t_at(20,10))deltrap(t_at(20,10));
        }
    }
    free_level();clear_level_structures();
    add_room(20,10,22,12,FALSE,OROOM,FALSE);
#ifdef SPECIALIZATION
    topologize(&svr.rooms[0],FALSE);
#else
    topologize(&svr.rooms[0]);
#endif
    {int x,y;for(x=20;x<=22;++x)for(y=10;y<=12;++y)levl[x][y].typ=STONE;}
    levl[21][10].typ=FORGE;levl[21][10].lit=1;
    do_mkroom(SWAMP);
    assert(svr.rooms[0].rtype==SWAMP);
    if(levl[21][10].typ!=FORGE) {puts("FAIL forge permanence: swamp conversion");++failures;}
    free_level();clear_level_structures();
    levl[20][10].typ=ROOM;
    {struct trap *t=maketrap(20,10,PIT);assert(t);
     /* Deliberately malformed legacy fixture; normal placement rejects this. */
     levl[20][10].typ=FORGE;
     if(maketrap(20,10,ARROW_TRAP) || t->ttyp!=PIT) {
         puts("FAIL forge permanence: replacing a pre-existing trap");++failures;
     }}
    assert(!failures);
    puts("PASS 45 forge permanence/lighting cases: Lua, regions, drawbridge, wishes, rooms, invocation, swamp and traps");
}
static void authored_trap_rejection(void) {
    static const char *ops[] = {
        "des.terrain({x=20,y=10,typ='f',lit=1})",
        "des.terrain({selection=selection.area(20,10,20,10),typ='f',lit=1})",
        "des.replace_terrain({region={20,10,20,10},fromterrain='.',toterrain='f',lit=1})",
        "des.map({x=20,y=10,map='f.',lit=1})"
    };
    static const int types[]={PIT,ARROW_TRAP,TELEP_TRAP};
    nhl_sandbox_info sbi={NHL_SB_SAFE,1024*1024,0,1024*1024};
    lua_State *L=nhl_init(&sbi);
    int i,j;
    assert(L);create_des_coder();
    for(i=0;i<SIZE(ops);++i)for(j=0;j<SIZE(types);++j) {
        struct trap *t, before;
        free_level();clear_level_structures();gx.xstart=gy.ystart=0;
        levl[20][10].typ=ROOM;
        t=maketrap(20,10,types[j]);assert(t);
        t->tseen=t->once=t->madeby_u=1;before=*t;
        levl[20][10].lit=0;levl[20][10].waslit=1;
        levl[20][10].flags=3;levl[20][10].horizontal=1;
        levl[20][10].roomno=ROOMOFFSET;levl[20][10].edge=1;
        assert(luaL_loadstring(L,ops[i])==LUA_OK);
        assert(lua_pcall(L,0,0,0)==LUA_OK);
        assert(levl[20][10].typ==ROOM && !levl[20][10].lit
               && levl[20][10].waslit && levl[20][10].flags==3
               && levl[20][10].horizontal && levl[20][10].roomno==ROOMOFFSET
               && levl[20][10].edge);
        assert(t_at(20,10)==t && t->ntrap==before.ntrap
               && t->tx==before.tx && t->ty==before.ty
               && t->ttyp==before.ttyp && t->tseen==before.tseen
               && t->once==before.once && t->madeby_u==before.madeby_u
               && t->dst.dnum==before.dst.dnum && t->dst.dlevel==before.dst.dlevel
               && t->launch.x==before.launch.x && t->launch.y==before.launch.y
               && t->vl.v_launch2.x==before.vl.v_launch2.x
               && t->vl.v_launch2.y==before.vl.v_launch2.y);
        if(i==3)assert(levl[21][10].typ==ROOM); /* map really loaded */
        assert(!set_levltyp_lit(20,10,FORGE,1));
        assert(set_levltyp_lit(20,10,CORR,0)); /* ordinary terrain stays allowed */
        assert(t_at(20,10)==t);
        deltrap(t);
        assert(set_levltyp_lit(20,10,FORGE,0) && levl[20][10].lit);
    }
    nhl_done(L);
    puts("PASS 12 authored trapped-square rejections: coordinate/selection/replace/map; trap and terrain metadata preserved; ordinary terrain and untrapped forge controls");
}
static void corpus(void) {
    int i, dl, selected=0, placed=0, failed=0, count, x,y, invalid=0, multiple=0;
    /* Fix the deferred Ludios junction outside the corpus depth range, so
       a case cannot turn into an excluded branch junction during generation. */
    { branch *br;
      for(br=svb.branches;br;br=br->next) {
        if(br->end1.dnum>=svn.n_dgns) {br->end1.dnum=medusa_level.dnum;br->end1.dlevel=18;}
        if(br->end2.dnum>=svn.n_dgns) {br->end2.dnum=medusa_level.dnum;br->end2.dlevel=18;}
      }
    }
    for(i=0;i<1000;++i) {
        free_level();dl=20+i%180;u.uz.dnum=medusa_level.dnum;u.uz.dlevel=dl;
        /* Distribute over actual eligible identities, excluding authored/junction
           depths. This does not generate or sample an excluded case. */
        while(!forge_eligible()) {dl=dl==199?20:dl+1;u.uz.dlevel=dl;}
        init_isaac64(150000UL+i,rn2);init_isaac64(160000UL+i,rn2_on_display_rng);
        gi.in_mklev=TRUE;step15_generate();
        assert(step15_forge_result>=0);selected+=step15_forge_result>0;
        placed+=step15_forge_result==2;failed+=step15_forge_result==1;count=0;
        for(x=1;x<COLNO;++x)for(y=0;y<ROWNO;++y) if(IS_FORGE(levl[x][y].typ)) {
            int owner=levl[x][y].roomno-ROOMOFFSET;
            struct mkroom *r;
            ++count;assert(levl[x][y].lit);
            /* Independent postcondition oracle; never calls the production
             * predicate or mutates terrain to make that predicate succeed.
             * Scripted provenance is checked separately with actual Lua. */
            if(owner<0 || owner>=svn.nroom || levl[x][y].edge
               || t_at(x,y) || stairway_at(x,y)) {++invalid;continue;}
            r=&svr.rooms[owner];
            if(r->rtype!=OROOM || r->orig_rtype!=OROOM || r->custom_id
               || r->nsubrooms || r->irregular || x<r->lx || x>r->hx
               || y<r->ly || y>r->hy)++invalid;
        }
        if(count>1)++multiple;
        assert(count==(step15_forge_result==2));
    }
    printf("CORPUS cases=1000 selections=%d placements=%d no_candidate=%d selection_rate=%.1f%% placement_rate=%.1f%% invalid=%d multiple=%d\n",selected,placed,failed,selected/10.,placed/10.,invalid,multiple);
    assert(!invalid&&!multiple&&selected==placed+failed);
}
int step15_test_main(void) {
    int i;setvbuf(stdout,NULL,_IONBF,0);
    windowprocs.win_raw_print=message;windowprocs.win_raw_print_bold=message;
    sysopt.crashreporturl=NULL;_set_error_mode(_OUT_TO_STDERR);_set_abort_behavior(0,_WRITE_ABORT_MSG|_CALL_REPORTFAULT);
    has_strong_rngseed=FALSE;init_isaac64(150001UL,rn2);init_isaac64(150002UL,rn2_on_display_rng);
    init_objects();flags.pantheon=-1;flags.initrole=flags.initrace=flags.initgend=flags.initalign=ROLE_NONE;
    Strcpy(svp.plname,"step15-probe");svp.pl_character[0]=0;role_init();init_dungeons();init_artifacts();
    svc.context.current_fruit=fruitadd("slime mold",NULL);u.ulevel=10;u.uhp=u.uhpmax=1000;u.ux=20;u.uy=10;svm.moves=1;
    l_nhcore_init();vision_init();flags.bones=FALSE;gy.youmonst.data=&mons[PM_HUMAN];u.umonnum=u.umonster=PM_HUMAN;
    for(i=0;i<A_MAX;++i)ABASE(i)=AMAX(i)=18;
    if (getenv("STEP15_CORPUS")) {corpus();return 0;}
    eligible_tests();candidates();reservoir_tests();scripted_room();
    free_level();u.uz.dnum=medusa_level.dnum;u.uz.dlevel=40;gi.in_mklev=TRUE;step15_generate();
    step15b_window_setup();terrain_activation();
    step15_forge_result=-99;level_finalize_topology();assert(step15_forge_result==-99);
    puts("PASS standalone Lua topology finalization performs no forge selection");
    persistence();
    step15b_test_main();
    shop_forges();
    terrain_mutators();
    authored_trap_rejection();
    provenance_tests();
    puts("PASS Step 15 native runtime fixtures");return 0;
}
