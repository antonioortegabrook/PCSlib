/*  pcs_parts object  by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
-------
 The pcs_parts object expects a pointer to a PCS struct (x->pcs).
 The pointer must be generated using the prime_form object or any
 other. The cardinal of the PCS must be 5 or 6.
 It find then all the the binary partitions of the PCS.
 The output:
 outlet1: left pcs part of the partition (floats list)
 outlet2: right pcs part of the partition (floats list)
 outlet3: a float indicating the kind of partition(0,1,2,3,4,or 12).

*/

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS
 
        Borrar variables no utilizadas...?
*/

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

#include "../pcslib-includes/pcslib_max.c"       //    PCSlib header

typedef struct _pcs_parts {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    PCS *pcs; 	  /*pointer to PCS struct*/
    t_atom llist[PCSL];	 /*left part of the partition*/
    t_atom rlist[7];		  /*right part of the partition*/
    PART* part;
    t_int from;
    t_int to;
    void *l_out, *r_out, *ty_out;
} t_pcs_parts;

// these are prototypes for the methods that are defined below
void pcs_parts_pcs_ptr(t_pcs_parts *x, t_symbol *s, long argc, t_atom *argv);  //- _list traducido como _anything
void pcs_parts_send_out(t_pcs_parts *x);
//void pcs_parts_int(t_pcs_parts *x, int n);

void pcs_parts_assist(t_pcs_parts *x, void *b, long m, long a, char *s);
void pcs_parts_free(t_pcs_parts *x);              //- _destroy traduce como _free CAMBIAR TAMBIÉN EN LA FUNCIÓN!!!
void *pcs_parts_new();

static t_class *pcs_parts_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("pcs.parts", (method)pcs_parts_new, (method)pcs_parts_free, sizeof(t_pcs_parts), 0L, 0);
    
    class_addmethod(c, (method)pcs_parts_pcs_ptr,        MPID,          A_GIMME, 0);    // pointer to PCS
//    class_addmethod(c, (method)pcs_parts_int,            "int",         A_LONG, 0);
    
    class_addmethod(c, (method)pcs_parts_assist,         "assist",      A_CANT, 0);	// (optional) assistance method

	class_register(CLASS_BOX, c);
	pcs_parts_class = c;
}

//--------------------------------------------------------------------------

void *pcs_parts_new() {
	t_pcs_parts *x;                                             // local variable (pointer to a t_pcs_parts data structure)

	x = (t_pcs_parts *)object_alloc(pcs_parts_class);           // create a new instance of this object

    x->ty_out=intout(x);                                 // create outlets
    x->r_out=listout(x);
    x->l_out=listout(x);
    
    x->pcs=NULL;
    
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs->find[0]=EOC;
    
    x->part  =(PART*)malloc(sizeof(PART));
    x->from=0;
    x->to=0;
    
	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void pcs_parts_assist(t_pcs_parts *x, void *b, long m, long a, char *s) {
	if (m == ASSIST_INLET)
		sprintf(s,"PCS");
    if (m == ASSIST_OUTLET) {
        switch(a) {
            case(0):
                sprintf(s,"Left part (list)");
                break;
            case(1):
                sprintf(s,"Right part (list)");
                break;
            case(2):
                sprintf(s,"Type of partition (int)");
                break;
        }
    }
}

void pcs_parts_pcs_ptr(t_pcs_parts *x, t_symbol *s, long argc, t_atom *argv) {
    
    {  //------------ get ptr --------------
        t_symbol *temp;
        PCS *tempcs;
        temp = atom_getsym(argv);
            
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
        CopiaSet(tempcs,x->pcs);                    //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------

    if(x->pcs->ncar <5 || x->pcs->ncar > 6 ){
        object_warn((t_object*)x, "pcs_part: cardinal of input pcs must be 5 or 6 only");
            return;
    }
    
    Particiones(x->pcs,x->part,tableptr);
    x->from=0;
    x->to=x->part->cant;
    pcs_parts_send_out(x);
    
    return;
}
/*                          // comentado hasta que sepamos para qué sirve
void pcs_parts_int(t_pcs_parts *x, int n) {
    
    switch (n) {
        case 14:
            x->from=0; x->to=5;
            break;
        case 23:
            x->from=5; x->to=15;
            break;
        case 15:
            x->from=0; x->to=6;
            break;
        case 24:
            x->from=6; x->to=21;
            break;
        case 33:
            x->from=21; x->to=31;
            break;
        case 0:
            x->from=0; x->to=0;
            break;
    }
    
    return;
}
*/
void pcs_parts_send_out(t_pcs_parts *x) {
    int i,j;
    int tempf;
    t_atom *llist, *rlist;
    
    llist=x->llist;
    rlist=x->rlist;
    
    for(i = x->from; i < x->to; i++){
        /*output partition type*/
        tempf = x->part->clasif[i];
        outlet_int(x->ty_out,tempf);
        /*output right part of the partition*/
        for(j=0; j<PCSL; ++j){
            if(x->part->temp2[i].find[j]==EOP) break;
            tempf = x->part->temp2[i].find[j];
            atom_setlong(&(rlist[j]),tempf);
        }
        outlet_list(x->r_out, gensym("list"),j,x->rlist);
        /*output left part of the partition*/
        for(j=0; j<PCSL; ++j){
            if(x->part->temp1[i].find[j]==EOP) break;
            tempf = x->part->temp1[i].find[j];
            atom_setlong(&(llist[j]),tempf);
        }
        outlet_list (x->l_out, gensym("list"),j,x->llist);
    }
    
    return;
}

void pcs_parts_free(t_pcs_parts *x){
    
    if(x->pcs != NULL) {
        free(x->pcs);
    }
    free(x->part);
    return;
}
