static void step16a_persistence_tests(void)
{
    struct obj *chain=NULL,*o;
    int i,remaining,k;
    for(i=0;i<SIZE(offensive_bits);++i)for(k=0;k<2;++k)
        for(remaining=0;remaining<=(i>=11?(i==11?75:i==12?50:i==13?25:10):0);++remaining) {
            o=item(DAGGER);
            assert(enhancement_set(o,offensive_bits[i],OQ_FINE,k));
            o->o_stoning_remaining=remaining;o->o_stoning_turn=remaining?svm.moves:0;
            o->nobj=chain;chain=o;
        }
    step14_roundtrip_batch(&chain);
    o=item(DAGGER);assert(enhancement_set(o,OEP_STONING_II,0,TRUE));
    o->o_stoning_remaining=37;o->o_stoning_turn=svm.moves-1;
    place_object(o,24,10);o=poly_obj(o,DAGGER);
    assert(o->o_stoning_remaining==37&&o->o_stoning_turn==svm.moves-1);
    o=poly_obj(o,LONG_SWORD);assert(!o->o_enh_props&&!o->o_stoning_remaining);
    obj_extract_self(o);obfree(o,NULL);
    puts("PASS Step 16A native codec all offensive tiers/knowledge/cooldown values, same-object polymorph preservation and identity reset");
}
