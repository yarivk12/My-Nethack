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
extern int step15_forge_result;
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
static void scripted_room(void) {
    int x, y, candidates=0;
    nhl_sandbox_info sbi={NHL_SB_SAFE,1024*1024,0,1024*1024};
    lua_State *L=nhl_init(&sbi);assert(L);create_des_coder();
    assert(luaL_loadstring(L,"des.level_init({style='solidfill',fg=' '}); des.room({type='ordinary',filled=1,w=8,h=5,contents=function() end})")==LUA_OK);
    assert(lua_pcall(L,0,0,0)==LUA_OK);nhl_done(L);
    level_finalize_topology();assert(svn.nroom==1 && svr.rooms[0].rtype==OROOM);
    for(x=1;x<COLNO;++x)for(y=0;y<ROWNO;++y)candidates+=forge_candidate(x,y);
    assert(!candidates);
    puts("PASS scripted Lua ordinary room excluded by transient provenance");
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
        assert(doapply()==((i==1||i==2||i==3)?ECMD_OK:ECMD_TIME));
        assert(!memcmp(&saved,h,sizeof saved));
        assert(!memcmp(&terrain,&levl[20][10],sizeof terrain));
        assert(svl.level.objects[20][10]==o);
    }
    assert(messages==before+2);
    HConfusion=HStun=0;ABASE(A_STR)=AMAX(A_STR)=18;
    assert(enhancement_set(h,OEP_FIRE,OQ_EXCEPTIONAL,FALSE));
    h->spe=-5;h->cursed=1;h->oeroded=2;saved=*h;
    cmdq_add_key(CQ_CANNED,h->invlet);assert(doapply()==ECMD_TIME);assert(!memcmp(&saved,h,sizeof saved));
    h->oartifact=ART_MJOLLNIR;saved=*h;
    cmdq_add_key(CQ_CANNED,h->invlet);assert(doapply()==ECMD_TIME);assert(!memcmp(&saved,h,sizeof saved));
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
            ++count;assert(levl[x][y].lit);levl[x][y].typ=ROOM;
            if(!forge_candidate(x,y))++invalid;
            levl[x][y].typ=FORGE;
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
    eligible_tests();candidates();scripted_room();
    free_level();u.uz.dnum=medusa_level.dnum;u.uz.dlevel=40;gi.in_mklev=TRUE;step15_generate();
    terrain_activation();
    step15_forge_result=-99;level_finalize_topology();assert(step15_forge_result==-99);
    puts("PASS standalone Lua topology finalization performs no forge selection");
    persistence();
    puts("PASS Step 15 native runtime fixtures");return 0;
}
