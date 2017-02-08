/*                                              Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
-------
cm.store object by Antonio Ortega Brook
 input: right inlet: a CM structure pointer
        left inlet: bang, CM structure pointer
 Stores a CM received in its right inlet. Bang or CM in left inlet causes output.
 */

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS
 
        Borrar variables no utilizadas...?
*/

#include "ext.h"			//      Max headers
#include "ext_obex.h"		//

#include "../pcslib-includes/pcslib_max.c"       //    PCSlib header

typedef struct _cm_store {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    CM *cm;   // pointer to a CM struct
    
    long p_in;
    void *cm_in;
    void *cm_out;
} t_cm_store;

// these are prototypes for the methods that are defined below
void cm_store_cm_ptr(t_cm_store *x, t_symbol *s, long argc, t_atom *argv);
void cm_store_bang(t_cm_store *x);

void cm_store_assist(t_cm_store *x, void *b, long m, long a, char *s);
void stdinletinfo();

void cm_store_free(t_cm_store *x);              //-
void *cm_store_new();

static t_class *cm_store_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("cm.store", (method)cm_store_new, (method)cm_store_free, sizeof(t_cm_store), 0L, 0);
    
    class_addmethod(c, (method)cm_store_cm_ptr,        MMID,		A_GIMME, 0);
    class_addmethod(c, (method)cm_store_bang,          "bang",               0);
	class_addmethod(c, (method)cm_store_assist,        "assist",	A_CANT,  0);	// (optional) assistance method
    class_addmethod(c, (method)stdinletinfo,           "inletinfo", A_CANT,  0);

	class_register(CLASS_BOX, c);
	cm_store_class = c;
}

//--------------------------------------------------------------------------

void *cm_store_new() {
	t_cm_store *x;                                             // local variable (pointer to a t_cm_store data structure)
    
	x = (t_cm_store *)object_alloc(cm_store_class);           // create a new instance of this object
    
    x->cm_in = proxy_new(x, 1, &x->p_in);
    x->cm_out=outlet_new(x, NULL);
     
    x->cm=NULL;
    
    x->cm=(CM*)malloc(sizeof(CM));
    Inicializar(x->cm);;

	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void cm_store_assist(t_cm_store *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET) {
        switch(a) {
            case(0):
                sprintf(s,"bang, CM");
                break;
            case(1):
                sprintf(s,"CM");
                break;
        }
    }
	if (m == ASSIST_OUTLET)
        sprintf(s,"CM");
}

void cm_store_free(t_cm_store *x){
    
    if(x->cm != NULL){
        free(x->cm);
    }
    return;
}

void cm_store_cm_ptr(t_cm_store *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    {  //------------ get ptr --------------
        t_symbol *temp;
        CM *tempcm;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcm);    // get the pointer to a PCS struct
        CopiaMatriz(tempcm,x->cm);                    //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------
    
    if(inum==0)
        cm_store_bang(x);
    return;
}

void cm_store_bang(t_cm_store *x) {
    {   //------------- out cm ptr -------------------
        char pstr[STRLP];
        t_atom ptr;
        sprintf(pstr, "%p", x->cm);    //convert pointer to CM struct into symbol //- (cuidado con el nombre de la struct)
        atom_setsym(&ptr, gensym(pstr));
        outlet_anything (x->cm_out, gensym(MMID), 1, &ptr);    //- (cuidado con el nombre del outlet)
    }   //------------- end out -------------------
    return;
}
