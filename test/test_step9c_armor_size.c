#include "hack.h"
#include <assert.h>
#include <stdio.h>
#include "step9c_armor_size.h"
int main(void) {
    struct obj o={0};struct permonst body={0};int typ,size,body_size,material,cases=0;
    monst_globals_init();objects_globals_init();
    for(typ=1;typ<NUM_OBJECTS;++typ) {
        o.otyp=typ;o.oclass=objects[typ].oc_class;
        for(size=0;size<=MZ_GIGANTIC+1;++size)
          for(body_size=MZ_TINY;body_size<=MZ_GIGANTIC;++body_size)
            for(material=0;material<=MINERAL;++material) {
                o.obranch_size=size;o.obranch_material=material;body.msize=body_size;
                assert(mith_armor_size_fits(&o,&body)==donor_fits(&o,&body));++cases;
            }
    }
    printf("PASS %d pinned clothing size cases, native unmarked gear, donor flexible hats/cloaks/elven suits/scales/shields\n",cases);
    return 0;
}
