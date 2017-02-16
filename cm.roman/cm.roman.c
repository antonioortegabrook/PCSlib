/*  cm_roman object  by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
-------
 
*/

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS
 
        Borrar variables no utilizadas...?
*/

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

#include "../pcslib-includes/pcslib_max.c"       //    PCSlib header

typedef struct _cm_roman {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    PCS *pcs; 	  /*pointer to PCS struct*/
    CM  *cm;
    void  *cm_out, *cm_nrc;
} t_cm_roman;

// these are prototypes for the methods that are defined below
void cm_roman_pcs_ptr(t_cm_roman *x, t_symbol *s, long argc, t_atom *argv);

void cm_roman_assist(t_cm_roman *x, void *b, long m, long a, char *s);
void cm_roman_free(t_cm_roman *x);
void *cm_roman_new();

static t_class *cm_roman_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("cm.roman", (method)cm_roman_new, (method)cm_roman_free, sizeof(t_cm_roman), 0L, 0);
    
    class_addmethod(c, (method)cm_roman_pcs_ptr,        MPID,          A_GIMME, 0);    // pointer to PCS
    
    class_addmethod(c, (method)cm_roman_assist,         "assist",      A_CANT, 0);	// (optional) assistance method

	class_register(CLASS_BOX, c);
	cm_roman_class = c;
}

//--------------------------------------------------------------------------

void *cm_roman_new() {
	t_cm_roman *x;                                             // local variable (pointer to a t_cm_roman data structure)

	x = (t_cm_roman *)object_alloc(cm_roman_class);           // create a new instance of this object

                                     // create outlets
    x->cm_nrc=listout(x);
    x->cm_out=outlet_new(x, NULL);
    
    x->pcs=NULL;
    
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs->find[0]=EOC;
    x->cm=(CM*)malloc(sizeof(CM));
    Inicializar(x->cm);
    
	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void cm_roman_assist(t_cm_roman *x, void *b, long m, long a, char *s) {
	if (m == ASSIST_INLET)
		sprintf(s,"PCS");
    if (m == ASSIST_OUTLET) {
        switch(a) {
            case(0):
                sprintf(s,"CM");
                break;
            case(1):
                sprintf(s,"Number of rows and columns (list)");
                break;
        }
    }
}

void cm_roman_pcs_ptr(t_cm_roman *x, t_symbol *s, long argc, t_atom *argv) {
    
    {  //------------ get ptr --------------
        t_symbol *temp;
        PCS *tempcs;
        temp = atom_getsym(argv);
            
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
        CopiaSet(tempcs,x->pcs);                    //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------
    
    switch(check_size(x->pcs)){     // Check PCS size
        case(-1):
            object_error((t_object*)x, "Max number of positions per CM is %i", NEXTC);
            return;
        case(-2):
            object_error((t_object*)x, "Max number of elements per position in a CM is %i", NEXTC);
            return;
    }
    
    RomanSq(x->cm,x->pcs);  // Build the cm
    
    {   //------ output Number of rows and cols ------
        t_atom plist[2];
        atom_setlong(&(plist[0]), x->cm->NroFilas);
        atom_setlong(&(plist[1]), x->cm->NroCols);
        outlet_list(x->cm_nrc, gensym("list"),2,plist);
    }   //--------- end output rows and cols ---------
    
    {   //------------- out cm ptr ----------------
        char pstr[STRLP];
        t_atom ptr;
        sprintf(pstr, "%p", x->cm);    //convert pointer to CM struct into symbol //- (cuidado con el nombre de la struct)
        atom_setsym(&ptr, gensym(pstr));
        outlet_anything (x->cm_out, gensym(MMID), 1, &ptr);    //- (cuidado con el nombre del outlet)
    }   //------------- end out -------------------
    
    return;
}

void cm_roman_free(t_cm_roman *x){
    if(x->pcs != NULL) free(x->pcs);
    if(x->cm != NULL) free(x->cm);
    return;
}
