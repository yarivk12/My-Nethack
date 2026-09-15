/* Real Lua with the production allocator: finalizers and strict limits. */
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#define staticfn static
static void *re_alloc(void *p, unsigned n) { return realloc(p,n); }
#include "step8a_allocator.h"
static int finalized, warnings;
static void warning(void *ud,const char *s,int more) {(void)ud;(void)s;(void)more;warnings++;}
static int finalize(lua_State *L) {
    /* Exercise an allocation while Lua disables the GC count API. */
    assert(lua_gc(L,LUA_GCCOUNT)==-1);
    lua_newuserdatauv(L,4096,0);
    finalized++;
    return 0;
}
static int create(lua_State *L) {
    lua_newuserdatauv(L,8,0);
    luaL_getmetatable(L,"fixture");lua_setmetatable(L,-2);return 1;
}
int main(void) {
    nhl_user_data nud={0};
    lua_State *L;
    void *p;
    nud.memlimit=256*1024;
    L=lua_newstate(nhl_alloc,&nud);assert(L);nud.L=L;
    lua_setwarnf(L,warning,NULL);luaL_openlibs(L);
    luaL_newmetatable(L,"fixture");lua_pushcfunction(L,finalize);lua_setfield(L,-2,"__gc");lua_pop(L,1);
    lua_pushcfunction(L,create);lua_setglobal(L,"create");
    assert(luaL_dostring(L,"for i=1,1000 do local x=create(); if i%10==0 then collectgarbage('collect') end end collectgarbage('collect')")==LUA_OK);
    assert(finalized==1000 && !warnings);
    assert(luaL_dostring(L,"local x={};for i=1,1000000 do x[i]=i end")!=LUA_OK);
    lua_pop(L,1);lua_close(L);assert(!nud.meminuse && !warnings);
    p=nhl_alloc(&nud,NULL,LUA_TTABLE,100);assert(p && nud.meminuse==100);
    assert(!nhl_alloc(&nud,p,100,nud.memlimit+1) && nud.meminuse==100);
    p=nhl_alloc(&nud,p,100,20);assert(p && nud.meminuse==20);
    nhl_alloc(&nud,p,20,0);assert(!nud.meminuse);
    puts("PASS real Lua: 1000 allocating finalizers, strict memory-limit rejection, resize/free accounting, zero warnings");
    return 0;
}
