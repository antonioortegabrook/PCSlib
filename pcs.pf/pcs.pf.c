/*  pcs_pf object by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
------- 

*/

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS

*/

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

#include "../pcslib-includes/pcslib_max.c"       //    PCSlib header

typedef struct _pcs_pf {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    PCS *pcs;   /*pointer to PCS struct*/
	void *pcs_out;
} t_pcs_pf;

// these are prototypes for the methods that are defined below
void pcs_pf_list(t_pcs_pf *x, t_symbol *s, long argc, t_atom *argv);  //-
void pcs_pf_assist(t_pcs_pf *x, void *b, long m, long a, char *s);
void pcs_pf_free(t_pcs_pf *x);
void *pcs_pf_new();

static t_class *pcs_pf_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("pcs.pf", (method)pcs_pf_new, (method)pcs_pf_free, sizeof(t_pcs_pf), 0L, 0);
    
    	class_addmethod(c, (method)pcs_pf_list,          "list",	A_LONG, 0);
	class_addmethod(c, (method)pcs_pf_assist,        "assist",	A_CANT, 0);	// (optional) assistance method

	class_register(CLASS_BOX, c);
	pcs_pf_class = c;
}

//--------------------------------------------------------------------------

void *pcs_pf_new() {
	t_pcs_pf *x;                                             // local variable (pointer to a t_pcs_pf data structure)

	x = (t_pcs_pf *)object_alloc(pcs_pf_class);           // create a new instance of this object
	x->pcs_out = outlet_new(x, NULL);                                    // create a list outlet

    x->pcs=NULL;
    
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs->find[0]=EOC;

	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void pcs_pf_assist(t_pcs_pf *x, void *b, long m, long a, char *s) // 4 final arguments are always the same for the assistance method
{
	if (m == ASSIST_INLET)
		sprintf(s,"(list) List of Pitch Classes");
    if (m == ASSIST_OUTLET)
        sprintf(s,"PCS");
}

void pcs_pf_free(t_pcs_pf *x){
    if(x->pcs != NULL) free(x->pcs);
    return;
}

void pcs_pf_list(t_pcs_pf *x, t_symbol *s, long argc, t_atom *argv) {
    t_int i;
    t_int j;
    
    if(argc>PCSL-2) { //- debemos garantizarnos dos posiciones libres al final del array para escribir EOP y EOC (revisar)
        object_error((t_object*)x, "too many elements received; no action taken");
        return;
    }
    
    // pickup a list with pcs
    /*
     Usamos una variable de incremento para leer el array de atoms y otra para escribir en el array find; esto es parte del typechecking y es necesario para que, si aparece un símbolo en el array de atoms, además de no copiarlo, la variable que usamos para escribir el array find no se incremente; de esta manera evitamos que luego EOC quede en un lugar incorrecto, lo cual derivaría en otros errores.
     */
    i=0;
    for(j = 0; j < argc; j++){
        if(atom_gettype(argv+j) == A_LONG || atom_gettype(argv+j) == A_FLOAT) {
            x->pcs->find[i] =(int)atom_getfloat(&argv[j]);
            
            if(x->pcs->find[i] < 0 && x->pcs->find[i]!=EOP)
                x->pcs->find[i]=abs(x->pcs->find[i]);
            
            if(x->pcs->find[i] >= 12)
                x->pcs->find[i]=x->pcs->find[i]%12; // take modulo-12
            i++;
        }
    }
    if(x->pcs->find[i-1] ==EOP) {
        x->pcs->find[i-1] =EOC;
    }
    else {
        x->pcs->find[i] =EOC;
    }
    
    if(x->pcs->find[0]==EOC){
        //post("pcs_pf: NULL");
        return;
    }
    
    forma_prima(x->pcs, tableptr);      // find prime form
    
    {   //------------- out ptr -------------------
        char pstr[STRLP];
        t_atom ptr;
        sprintf(pstr, "%p", x->pcs);    //convert pointer to PCS struct into symbol //- (cuidado con el nombre de la struct)
        atom_setsym(&ptr, gensym(pstr));
        outlet_anything (x->pcs_out, gensym(MPID), 1, &ptr);    //- (cuidado con el nombre del outlet)
    }   //------------- end out -------------------
    return;
}
