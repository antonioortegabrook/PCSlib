/*  pcs_write object by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
------- 
 The pcs_write object expects a list with three or four args.
 cardinal number(float), ordinal number(float) and transposition factor(float).
 If a fourth arg is delivered, it must be a symbol "I", which stands for
 "inversion". The PCS is scanned at the PCS table and transposed and/or inverted
 if required.
 The output is a pointer to a PCS struct. To access the data, a pcs_read
 object must be used.
*/

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS

 //- bug! Crash cuando ncar==3 && nord==0
                       ncar==4 && nord==-12 Puede estar relacionado con pcs_funcs.c
                       ncar==5 && nord==-41
                       ncar==6 && nord==-79
                       ncar==7 && nord==-129
                       ncar==8 && nord==-167
                       ncar==9 && nord==-196
 
*/

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

#include "../pcslib-includes/pcslib_max.c"       //    PCSlib header

typedef struct _pcs_write {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    PCS *pcs;   /*pointer to PCS struct*/
    t_int ncar;
	t_int nord;
    t_int t;
    t_int i;
	void *pcs_out;
} t_pcs_write;

// these are prototypes for the methods that are defined below
void pcs_write_list(t_pcs_write *x, t_symbol *s, long argc, t_atom *argv);

void pcs_write_assist(t_pcs_write *x, void *b, long m, long a, char *s);

void pcs_write_free(t_pcs_write *x);
void *pcs_write_new();

static t_class *pcs_write_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("pcs.write", (method)pcs_write_new, (method)pcs_write_free, sizeof(t_pcs_write), 0L, 0);
    
    class_addmethod(c, (method)pcs_write_list,          "list",		A_GIMME, 0);
	
    class_addmethod(c, (method)pcs_write_assist,        "assist",	A_CANT, 0);	// (optional) assistance method

	class_register(CLASS_BOX, c);
	pcs_write_class = c;
}

//--------------------------------------------------------------------------

void *pcs_write_new() {
	t_pcs_write *x;                                             // local variable (pointer to a t_pcs_write data structure)

	x = (t_pcs_write *)object_alloc(pcs_write_class);           // create a new instance of this object
	x->pcs_out = outlet_new(x, NULL);                                    // create a list outlet

    x->pcs=NULL;
    
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs->find[0]=EOC;

	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void pcs_write_assist(t_pcs_write *x, void *b, long m, long a, char *s) // 4 final arguments are always the same for the assistance method
{
	if (m == ASSIST_INLET)
		sprintf(s,"(list) Ordinal (int), Cardinal (int), Transposition factor (int), I (symbol, optional)");
    if (m == ASSIST_OUTLET)
        sprintf(s,"PCS");
}

void pcs_write_free(t_pcs_write *x){
    
    if(x->pcs != NULL){
        free(x->pcs);
    }
    return;
}

void pcs_write_list(t_pcs_write *x, t_symbol *s, long argc, t_atom *argv) {
    t_int i;
    t_int tpos[7]={0,12,41,79,129,167,196};
    t_int nc[7]={12,29,38,50,38,29,12};
    t_int ptr=0;
    
    if(argc < 2) {
        object_error((t_object*)x, "missing arguments...");
        return;
    }
    
    //pickup a list with args
    if(atom_gettype(argv) == A_LONG || atom_gettype(argv) == A_FLOAT) {
        x->ncar = atom_getlong(argv);
    } else {
        object_error((t_object*)x, "wrong message");      // mostramos un mensaje de error si recibimos no longs o floats
        return;
    }
    if(x->ncar < 3 || x->ncar > 9 ) {
        object_warn((t_object*)x, "cardinal number must be between 3 and 9");
        return;
    }
    
    if(atom_gettype(argv+1) == A_LONG || atom_gettype(argv+1) == A_FLOAT) {
        x->nord = atom_getlong(argv+1);
    } else {
        object_error((t_object*)x, "wrong message");      // mostramos un mensaje de error si recibimos no longs o floats
        return;
    }
    if((x->nord > nc[x->ncar-3]) || (x->nord < 1)) {
        object_warn((t_object*)x, "the SC %d-%d does not exist!", x->ncar, x->nord);
        return;
    }
    if (argc == 2) {
        x->t = 0;         // transposition default is 0
        x->i = FALSE;     // inversion default is false
    } else {                // if only two args are given, there's no need to check the others ;)
        if(argc > 2) {
            if(atom_gettype(argv+2) == A_SYM) { // if third arg is sym i, it's taken as inv flag...
                if(atom_getsym(argv+2) == gensym("I") || atom_getsym(argv+2) == gensym("i")) {
                    x->i =TRUE;
                } else {
                    x->i =FALSE;            //- esta línea no es redundante?
                }
                x->t = 0;                   // ...and transposition is set to default
            } else {
                if(atom_gettype(argv+2) == A_LONG || atom_gettype(argv+2) == A_FLOAT)
                    x->t = atom_getlong(argv+2);    // if it's a long instead, we set t factor to it and check for a fourth arg
                if(argc > 3) {
                    if(atom_getsym(argv+3) == gensym("I") || atom_getsym(argv+3) == gensym("i"))
                        x->i =TRUE;
                    else
                        x->i =FALSE;
                } else
                    x->i =FALSE;
            }
        }
    }
    
    ptr=((tpos[x->ncar-3] + x->nord-1)*20);
    x->pcs->ncar=tableptr[ptr];
    x->pcs->nord=tableptr[ptr+1];
    for(i=0; i<x->pcs->ncar; i++) {
        x->pcs->find[i]=tableptr[i+ptr+2];
    }
    x->pcs->find[i]=EOC;
    
    forma_prima(x->pcs, tableptr);  //find prime form
    
    if(x->i == TRUE )         //need trasposition and/or inversion?
        TrInvPCS(x->pcs,TRUE,x->t);
    else
        TrInvPCS(x->pcs,FALSE,x->t);
    
    forma_prima(x->pcs, tableptr);  //- por qué buscamos dos veces la forma prima...? y más aún cuando ya lo hace TrInvPCS?
    
    {   //------------- out ptr -------------------
        char pstr[STRLP];
        t_atom ptrOut;
        sprintf(pstr, "%p", x->pcs);    //convert pointer to PCS struct into symbol //- (cuidado con el nombre de la struct)
        atom_setsym(&ptrOut, gensym(pstr));
        outlet_anything (x->pcs_out, gensym(MPID), 1, &ptrOut);    //- (cuidado con el nombre del outlet)
    }   //------------- end out -------------------
    return;		
}
