/*                                              Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
-------
pcs.store object by Antonio Ortega Brook
 input: right inlet: a CM structure pointer
        left inlet: bang, CM structure pointer
 Stores a PCS received in its right inlet. Bang or PCS in left inlet causes output.
 */

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS
 
        Borrar variables no utilizadas...?
*/

#include "ext.h"			//      Max headers
#include "ext_obex.h"		//

#include "../pcslib-includes/pcslib_max.c"       //    PCSlib header

typedef struct _pcs_store {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    PCS *pcs;   // pointer to a CM struct
    
    long p_in;
    void *pcs_in;
    void *pcs_out;
} t_pcs_store;

// these are prototypes for the methods that are defined below
void pcs_store_pcs_ptr(t_pcs_store *x, t_symbol *s, long argc, t_atom *argv);
void pcs_store_bang(t_pcs_store *x);

void pcs_store_assist(t_pcs_store *x, void *b, long m, long a, char *s);
void stdinletinfo();

void pcs_store_free(t_pcs_store *x);              //-
void *pcs_store_new();

static t_class *pcs_store_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("pcs.store", (method)pcs_store_new, (method)pcs_store_free, sizeof(t_pcs_store), 0L, 0);
    
    class_addmethod(c, (method)pcs_store_pcs_ptr,        MPID,		A_GIMME, 0);
    class_addmethod(c, (method)pcs_store_bang,          "bang",               0);
	class_addmethod(c, (method)pcs_store_assist,        "assist",	A_CANT,  0);	// (optional) assistance method
    class_addmethod(c, (method)stdinletinfo,           "inletinfo", A_CANT,  0);

	class_register(CLASS_BOX, c);
	pcs_store_class = c;
}

//--------------------------------------------------------------------------

void *pcs_store_new() {
	t_pcs_store *x;                                             // local variable (pointer to a t_pcs_store data structure)
    
	x = (t_pcs_store *)object_alloc(pcs_store_class);           // create a new instance of this object
    
    x->pcs_in = proxy_new(x, 1, &x->p_in);
    x->pcs_out=outlet_new(x, NULL);
     
    x->pcs=NULL;
    
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs->find[0]=EOC;

	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void pcs_store_assist(t_pcs_store *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET) {
        switch(a) {
            case(0):
                sprintf(s,"bang, PCS");
                break;
            case(1):
                sprintf(s,"PCS");
                break;
        }
    }
	if (m == ASSIST_OUTLET)
        sprintf(s,"PCS");
}

void pcs_store_free(t_pcs_store *x){
    if(x->pcs != NULL) free(x->pcs);
    return;
}

void pcs_store_pcs_ptr(t_pcs_store *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    {  //------------ get ptr --------------
        t_symbol *temp;
        PCS *tempcs;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
        CopiaSet(tempcs,x->pcs);                    //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------
    
    if(inum==0)
        pcs_store_bang(x);
    return;
}

void pcs_store_bang(t_pcs_store *x) {
    {   //------------- out ptr -------------------
        char pstr[STRLP];
        t_atom ptr;
        sprintf(pstr, "%p", x->pcs);    //convert pointer to PCS struct into symbol //- (cuidado con el nombre de la struct)
        atom_setsym(&ptr, gensym(pstr));
        outlet_anything (x->pcs_out, gensym(MPID), 1, &ptr);    //- (cuidado con el nombre del outlet)
    }   //------------- end out -------------------
    return;
}
