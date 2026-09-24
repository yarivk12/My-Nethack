/* Current native object-chain codec, including upper mask bits and knowledge. */
static void step15d_persistence_tests(void)
{
    struct obj *chain=NULL,*o;
    const int types[]={BOW,LEATHER_ARMOR,RIN_CONFLICT,AMULET_OF_LIFE_SAVING};
    int kind,tier,ids[EP_COUNT],n,i,j,v,k,a,b,total=0;
    for(kind=0;kind<SIZE(types);++kind)for(tier=1;tier<=4;++tier) {
        o=item(types[kind]);n=socket_candidates(o,tier,-1,ids);obfree(o,NULL);
        for(i=0;i<n;++i) {
            const struct enhancement_entry *e=equipment_property(ids[i]);
            for(v=e->stat?e->dice:0;v<=(e->stat?e->dice*e->sides:0);++v)
                for(k=0;k<2;++k) {
                    o=item(types[kind]);
                    o->o_sockets[0].property=(uint8)ids[i];
                    o->o_sockets[0].value=(uint8)v;o->o_sockets[0].known=(uint8)k;
                    if(o->o_socket_capacity==2) {
                        int other[EP_COUNT],count=socket_candidates(o,1,-1,other);
                        assert(count);o->o_sockets[1].property=(uint8)other[0];
                        e=equipment_property(other[0]);
                        o->o_sockets[1].value=(uint8)(e->stat?e->dice:0);
                        o->o_sockets[1].known=(uint8)!k;
                        e=equipment_property(ids[i]);
                    }
                    o->nobj=chain;chain=o;++total;
                }
        }
    }
    for(i=0;i<4;++i)for(j=0;j<4;++j)for(a=1;a<=6;++a)for(b=1;b<=6;++b) {
        const struct enhancement_entry *ea=&enhancement_catalog[24+i],*eb=&enhancement_catalog[28+j];
        if(a<ea->dice||a>ea->dice*ea->sides||b<eb->dice||b>eb->dice*eb->sides)continue;
        o=item(BOW);o->o_enh_props=ea->bit|eb->bit;
        o->o_enh_values[i]=(uint8)a;o->o_enh_values[4+j]=(uint8)b;
        o->o_enh_known=eb->bit;o->o_sockets[0].property=EP_FIRE_III;
        o->o_sockets[0].known=1;o->o_sockets[1].property=EP_COLD;
        o->nobj=chain;chain=o;++total;
    }
    step14_roundtrip_batch(&chain);
    o=item(BOW);o->o_enh_props=OEP_STR_IV;o->o_enh_values[3]=255;
    o->o_socket_capacity=255;o->o_sockets[0].property=255;
    o->o_sockets[1].property=EP_STR_I;o->o_sockets[1].value=255;
    socket_normalize(o);
    assert(o->o_socket_capacity==2&&!socket_count(o)&&!o->o_enh_props&&!o->o_enh_values[3]);
    o->o_sockets[0].property=EP_FIRE;o->o_sockets[0].known=1;
    unknow_object(o);assert(!o->o_sockets[0].known&&o->o_sockets[0].property==EP_FIRE);
    obfree(o,NULL);
    printf("PASS Step 15D native codec %d mixed socket/value/partial-knowledge and ordinary upper-bit states; malformed-state and acquisition knowledge controls\n",total);
}
