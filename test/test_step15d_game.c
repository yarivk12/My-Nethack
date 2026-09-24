/* Diagnostic creation and read-only checks around production executable runs. */
static void step15d_game_fixture(boolean resuming)
{
    struct obj *o;
    FILE *log;
    int retained=0, result=0;
    if (!resuming) {
        o=item(WAR_HAMMER);o->bknown=0;addinv(oname(o,"15d-hammer",ONAME_NO_FLAGS));
        o=item(DAGGER);o->quan=3;o->owt=weight(o);
        o->o_sockets[0].property=EP_STR_III;o->o_sockets[0].value=4;o->o_sockets[0].known=1;
        addinv(oname(o,"15d-stack",ONAME_NO_FLAGS));
        o=item(WORTHLESS_RED_GLASS);o->quan=2;o->owt=weight(o);o->dknown=1;
        objects[o->otyp].oc_name_known=0;addinv(oname(o,"15d-material",ONAME_NO_FLAGS));
        o=item(BLACK_OPAL);o->quan=30;o->owt=weight(o);o->dknown=1;
        makeknown(BLACK_OPAL);addinv(oname(o,"15d-gems",ONAME_NO_FLAGS));
        ABASE(A_STR)=AMAX(A_STR)=16;
        return;
    }
    for(o=gi.invent;o;o=o->nobj) if(has_oname(o)) {
        if(!strcmp(ONAME(o),"15d-hammer")) {
            assert(!o->blessed&&!o->cursed&&!o->bknown&&!socket_count(o));
        } else if(!strcmp(ONAME(o),"15d-stack")) {
            assert(o->o_socket_capacity==1);
            if(o->quan==2) {
                assert(o->o_sockets[0].property==EP_STR_III
                       &&o->o_sockets[0].value==4&&o->o_sockets[0].known);++retained;
            } else {
                const struct enhancement_entry *e=equipment_property(o->o_sockets[0].property);
                assert(o->quan==1&&e&&e->tier==2&&o->o_sockets[0].known);
                if(e->stat)assert(o->o_sockets[0].value>=e->dice
                                  &&o->o_sockets[0].value<=e->dice*e->sides);
                ++result;
            }
        } else if(!strcmp(ONAME(o),"15d-material")) {
            assert(o->quan==1&&!objects[o->otyp].oc_name_known);
        }
    }
    assert(retained==1&&result==1);
    log=fopen("step15d-game-results.txt","a");assert(log);
    fprintf(log,"PASS production affixing: one result, unchanged stack remainder, acquired value and knowledge, unknown glass and hammer BUC\n");
    fclose(log);
}
