/*  pcs_zmate object                            - Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
------- 
 The pcs_zmate object expects a pointer to a PCS structure. It outputs the z mate of the received pcs, if exist, trough the letf outlet or a bang trough the right one if it doesn't
*/

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS
 
*/

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

#include "../pcslib-includes/pcslib_max.c"

typedef struct _pcs_zmate {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    PCS *pcs1;   /*pointer to PCS struct*/
    PCS *pcs2;
    
    void *bang_out;
	void *pcs_out;
} t_pcs_zmate;

// these are prototypes for the methods that are defined below
void pcs_zmate_pcs_ptr(t_pcs_zmate *x, t_symbol *s, int argc, t_atom *argv);

void pcs_zmate_assist(t_pcs_zmate *x, void *b, long m, long a, char *s);

void pcs_zmate_free(t_pcs_zmate *x);
void *pcs_zmate_new();

static t_class *pcs_zmate_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("pcs.zmate", (method)pcs_zmate_new, (method)pcs_zmate_free, sizeof(t_pcs_zmate), 0L, 0);
    
    class_addmethod(c, (method)pcs_zmate_pcs_ptr,       MPID,		A_GIMME, 0);
	
    class_addmethod(c, (method)pcs_zmate_assist,        "assist",	A_CANT, 0);	// (optional) assistance method

	class_register(CLASS_BOX, c);
	pcs_zmate_class = c;
}

//--------------------------------------------------------------------------

void *pcs_zmate_new() {
	t_pcs_zmate *x;                                             // local variable (pointer to a t_pcs_zmate data structure)

	x = (t_pcs_zmate *)object_alloc(pcs_zmate_class);           // create a new instance of this object
	x->bang_out = bangout(x);
    x->pcs_out = outlet_new(x, NULL);                                    // create a list outlet

    x->pcs1=NULL;
    x->pcs1=NULL;
    
    x->pcs1=(PCS*)malloc(sizeof(PCS));
    x->pcs1->find[0]=EOC;
    
    x->pcs2=(PCS*)malloc(sizeof(PCS));
    x->pcs2->find[0]=EOC;

	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void pcs_zmate_assist(t_pcs_zmate *x, void *b, long m, long a, char *s) // 4 final arguments are always the same for the assistance method
{
	if (m == ASSIST_INLET)
		sprintf(s,"PCS");
    if (m == ASSIST_OUTLET) {
        switch(a) {
            case(0):
                sprintf(s,"(PCS) Z mate if exists");
                break;
            case(1):
                sprintf(s, "Bang if there's no Z mate");
                break;
        }
    }
}

void pcs_zmate_free(t_pcs_zmate *x){
    if(x->pcs1 != NULL) free(x->pcs1);
    if(x->pcs2 != NULL) free(x->pcs2);
    return;
}

void pcs_zmate_pcs_ptr(t_pcs_zmate *x, t_symbol *s, int argc, t_atom *argv) {
    int i, ptr;
    
    {  //------------ get ptr --------------
        t_symbol *temp;                 //- se podría omitir la variable temp...?
        PCS *tempcs;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
        CopiaSet(tempcs,x->pcs1);                    //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------
    
    if(isz(x->pcs1,tableptr)==false) {      // bang if not z
        outlet_bang(x->bang_out);
        return;
    }
    
    ptr=isz(x->pcs1,tableptr)-1;
    
    x->pcs2->ncar=tableptr[ptr];
    x->pcs2->nord=tableptr[ptr+1];
    for(i=0; i<x->pcs2->ncar; i++) {
        x->pcs2->find[i]=tableptr[i+ptr+2];
    }
    x->pcs2->find[i]=EOC;
    
    forma_prima(x->pcs2, tableptr);  //find prime form
    
    
    {   //------------- out ptr -------------------
        char pstr[STRLP];
        t_atom ptrOut;
        sprintf(pstr, "%p", x->pcs2);    //convert pointer to PCS struct into symbol //- (cuidado con el nombre de la struct)
        atom_setsym(&ptrOut, gensym(pstr));
        outlet_anything (x->pcs_out, gensym(MPID), 1, &ptrOut);    //- (cuidado con el nombre del outlet)
    }   //------------- end out -------------------
    return;		
}
