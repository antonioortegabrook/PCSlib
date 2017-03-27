/*  pcs_read object  by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
-------
 The pcs_read object expects a pointer to a PCS struct (x->pcs).
 The pointer must be generated using the prime_form object or any
 other.
 The output:
 outlet0: original pc of the pcs(ints list)
 outlet1: cardinal number(int)
 outlet2: ordinal number(int)
 outlet3: status (T/I)(ints list)
 outlet4: prime form(ints list)
 outlet5: interval class vector(ints list)
 outlet6: literal complement(ints list)
*/

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS
 
        Borrar variables no utilizadas...?
*/
/** EXPERIMENTAL **
    Usamos t_ptr_mess para enviar/recibir punteros, en vez de t_atom
 */

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

#include "../pcslib-includes/pcslib_api.h"

#include "../pcslib-includes/pcslib_max_types.h"
#include "../pcslib-includes/pcslib_max_funcs.c"

typedef struct _pcs_read {	// defines our object's internal variables for each instance in a patch
        t_object x_obj;			// object header - ALL objects MUST begin with this...
        
        t_pcs *pcs;		  /*pointer to PCS struct*/
        
        void *nc_out, *no_out, *pf_out, *iv_out, *ti_out, *fi_out, *co_out;
} t_pcs_read;

// these are prototypes for the methods that are defined below
void pcs_read_pcs_ptr_mes(t_pcs_read *x, t_symbol *s, long argc, t_ptr_mess *argv);

void pcs_read_assist(t_pcs_read *x, void *b, long m, long a, char *s);
void pcs_read_free(t_pcs_read *x);
void *pcs_read_new();

static t_class *pcs_read_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("pcs.read", (method)pcs_read_new, (method)pcs_read_free, sizeof(t_pcs_read), 0L, 0);
    
    class_addmethod(c, (method)pcs_read_pcs_ptr_mes,	MPID,       A_GIMME,  0);
	class_addmethod(c, (method)pcs_read_assist,         "assist",	A_CANT, 0);	// (optional) assistance method

	class_register(CLASS_BOX, c);
	pcs_read_class = c;
}

//--------------------------------------------------------------------------

void *pcs_read_new() {
        t_pcs_read *x;                                 // local variable (pointer to a t_pcs_read data structure)
        
        x = (t_pcs_read *)object_alloc(pcs_read_class);           // create a new instance of this object
        
        x->co_out = listout(x);                                 // create outlets
        x->iv_out = listout(x);
        x->pf_out = listout(x);
        x->ti_out = listout(x);
        x->no_out = intout(x);
        x->nc_out = intout(x);
        x->fi_out = listout(x);
        
        x->pcs = NULL;
        
        return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void pcs_read_assist(t_pcs_read *x, void *b, long m, long a, char *s) {
	if (m == ASSIST_INLET)
		sprintf(s,"PCS");
    if (m == ASSIST_OUTLET) {
        switch(a) {
            case(0):
                sprintf(s,"Original PCS (list)");
                break;
            case(1):
                sprintf(s,"Cardinal number (int)");
                break;
            case(2):
                sprintf(s,"Ordinal number (int)");
                break;
            case(3):
                sprintf(s,"T/I status (list)");
                break;
            case(4):
                sprintf(s,"Prime form (list)");
                break;
            case(5):
                sprintf(s,"ICV (list)");
                break;
            case(6):
                sprintf(s,"Literal complement (list)");
                break;
        }
    }
}

void pcs_read_free(t_pcs_read *x){
    if(x->pcs != NULL) free(x->pcs);
    return;
}

void pcs_read_pcs_ptr_mes(t_pcs_read *x, t_symbol *s, long argc, t_ptr_mess *argv)
{
        t_int nord_pf;	          /*ordinal number*/
        t_int nc_pf;	          /*cardinal number*/
        t_atom pflist[PCSL];    /*prime form*/
        t_atom ivlist[7];       /*interval class vector*/
        t_atom filist[PCSL];    /*individual form(may have pc repeated)*/
        t_atom clist[PCSL];     /*complement list*/
        t_atom tlist[2];     /*IT [0]=T, [1]=I list*/
        
        
    
    int i,j,cp=0,index=0,n;
    long tempf;            //- <-- ver: quizás sea bueno que esto sea long.
    t_atom *pflist, *ivlist, *filist, *clist, *tilist;
    t_int cvec[PCSL];

    {  //------------ get ptr --------------
            t_pcs *tempcs;
            
            tempcs = ptr_mess_getpcs(argv);    // get the pointer to a PCS struct
            
            if (!tempcs) {
                    object_error((t_object*)x, "bad pointer");
                    return;
            }
            
            x->pcs = tempcs;

    }  //------------ end get --------------
    
        if (!x->pcs->consistent) {
                object_error((t_object*)x, "pcs data inconsistent");
                return;
        }
    
    pflist=x->pflist;
    ivlist=x->ivlist;
    filist=x->filist;
    clist=x->clist;
    tilist=x->tlist;
    
    /*pickup data from pcs struct*/
    x->nc_pf=x->pcs->ncar;
    x->no_pf=x->pcs->nord;
    
    for(i=0; i<x->pcs->ncar; ++i) {
        if(i == PCSL-1) {
            break;
        }
        tempf=(long)x->pcs->fprima[i];
        atom_setlong(&(pflist[i]),tempf);
    }
    
    for(i=0; i<PCSL; ++i) {
        if(x->pcs->find[i]==EOC) {
            n=i;
            break;
        }
        tempf=(long)x->pcs->find[i];
        atom_setlong(&(filist[i]),tempf);
    }
    
    for(i=0; i<ICVL; ++i) {
        tempf=(long)x->pcs->icvmat[i];
        atom_setlong(&(ivlist[i]),tempf);
    }
    
    
    //temps=gensym(x->pcs->Status);
    /*Status field is not anymore a symbol*/
    /*instead, it is a list of two floats*/     // <-- longs en la versión Max
    atom_setlong(&(tilist[0]),(long)x->pcs->T);
    atom_setlong(&(tilist[1]),(long)x->pcs->I);
    
    /*find literal complement*/
    index=0;
    for(i=0; i<12; ++i) {
        cp=0; j=0;
        while(x->pcs->find[j] != EOC) {
            if(x->pcs->find[j]==i){
                ++cp;
                break;
            }
            j++;
        }
        if(cp==0){
            cvec[index++]=i;
        }
    }
    for(i=0; i<12-x->pcs->ncar; ++i) {
        tempf=(long)cvec[i];
        atom_setlong(&(clist[i]),tempf);
    }

    //-------------- output data via outlets
//    outlet_list (x->co_out, gensym("list"), 12-x->pcs->ncar, x->clist);       //(complement)
    outlet_list (x->iv_out, gensym("list"), ICVL, ivlist);
    outlet_list (x->pf_out, gensym("list"), x->pcs->ncar, pflist);
    outlet_list (x->ti_out, gensym("list"), 2, tlist);
    outlet_int  (x->no_out, nord_pf);
    outlet_int  (x->nc_out, nc_pf);
    outlet_list (x->fi_out, gensym("list"), n, filist);
    
    return;
}
