/* Every legal active mask x every class-relevant knowledge input x quality
 * x flag. Step 16C clamps impossible knowledge to actual state on restore.
 * Batches bound memory without reducing the exhaustive cross product. */
static void
step14_roundtrip_batch(struct obj **chain)
{
    NHFILE *f;
    struct obj *restored,*a,*b,*nexta,*nextb;
    f=get_freeing_nhfile();file_mode(f,WRITING,open("step14-chain.tmp",O_CREAT|O_TRUNC|O_WRONLY|O_BINARY,_S_IREAD|_S_IWRITE));
    step13_save_chain(f,chain);close_nhfile(f);
    f=get_freeing_nhfile();file_mode(f,READING,open("step14-chain.tmp",O_RDONLY|O_BINARY));
    restored=step13_restore_chain(f);close_nhfile(f);
    for(a=*chain,b=restored;a&&b;a=nexta,b=nextb) {
        struct obj expected=*a;
        expected.o_enh_known &= expected.o_enh_props;
        expected.o_enh_known2 &= expected.o_enh_props2;
        SAME(&expected,b);assert(a->o_id==b->o_id);
        assert(a->spe==b->spe && a->age==b->age);
        nexta=a->nobj;nextb=b->nobj;a->nobj=b->nobj=NULL;
        obfree(a,NULL);obfree(b,NULL);
    }
    assert(!a&&!b);*chain=NULL;dobjsfree();
}

static void
step14_persistence_tests(void)
{
    /* Specification identities: do not derive the codec oracle from the
     * catalog/setter whose omissions this test must be able to detect. */
    static const uint32 contract_bits[2][13]={
        {OEP_FIRE,OEP_COLD,OEP_SHOCK,OEP_TRUEFLIGHT,
         OEP_FIRE_II,OEP_COLD_II,OEP_SHOCK_II,
         OEP_FIRE_III,OEP_COLD_III,OEP_SHOCK_III,OEP_PRIMORDIAL,0,0},
        {OEP_SEARCHING,OEP_WARNING,OEP_STEALTH,OEP_FIRE_RES,OEP_COLD_RES,
         OEP_SHOCK_RES,OEP_POISON_RES,OEP_SPEED,OEP_REGEN,OEP_DISPLACED,
         OEP_SLOW_DIGEST,OEP_MAGIC_RES,OEP_REFLECTION}
    };
    struct obj *chain=NULL,*o;
    uint32 bits[13],masks[92],known,props;
    int kind,i,j,n,nm,ki,q,flags,batch=0;
    long total=0;
    for(kind=0;kind<2;++kind) {
        n=kind?13:11;
        for(i=0;i<n;++i)bits[i]=contract_bits[kind][i];
        nm=0;masks[nm++]=0;
        for(i=0;i<n;++i) {
            masks[nm++]=bits[i];
            for(j=i+1;j<n;++j)masks[nm++]=bits[i]|bits[j];
        }
        assert(nm==(kind?92:67));
        for(i=0;i<nm;++i)for(ki=0;ki<(1<<n);++ki) {
            known=0;props=masks[i];
            for(j=0;j<n;++j)if(ki&(1<<j))known|=bits[j];
            for(q=0;q<3;++q)for(flags=0;flags<2;++flags) {
                o=item(kind?LEATHER_ARMOR:ARROW);
                o->o_enh_props=props;o->o_enh_quality=(uint8)q;
                o->o_enh_known=known;o->o_enh_flags=flags;
                o->nobj=chain;chain=o;++batch;++total;
                if(batch==8192){step14_roundtrip_batch(&chain);batch=0;}
            }
        }
    }
    if(chain)step14_roundtrip_batch(&chain);
    printf("PASS native saveobjchn/restobjchn %ld finalized legal active/class-knowledge/quality/flags states, including high-bit T4+T4\n",total);
}
