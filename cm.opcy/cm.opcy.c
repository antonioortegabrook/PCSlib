/*  cm_opcy object  by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
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

typedef struct _cm_opcy {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    PCS *pcs; 	  // pointer to PCS struct
    CM  *cm;      // pointer to CM struct
    
    long p_in;
    void *pcs_in;
    void *cm_out, *cm_nrc;
} t_cm_opcy;

// these are prototypes for the methods that are defined below
void cm_opcy_pcs_ptr(t_cm_opcy *x, t_symbol *s, long argc, t_atom *argv);
void cm_opcy_int(t_cm_opcy *x, long argv);

void cm_opcy_assist(t_cm_opcy *x, void *b, long m, long a, char *s);
void cm_opcy_free(t_cm_opcy *x);
void *cm_opcy_new();

static t_class *cm_opcy_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("cm.opcy", (method)cm_opcy_new, (method)cm_opcy_free, sizeof(t_cm_opcy), 0L, 0);
    
    class_addmethod(c, (method)cm_opcy_pcs_ptr,        MPID,          A_GIMME, 0);    // pointer to PCS
    class_addmethod(c, (method)cm_opcy_int,            "int",         A_LONG,  0);
    
    class_addmethod(c, (method)cm_opcy_assist,         "assist",      A_CANT,  0);	// (optional) assistance method

	class_register(CLASS_BOX, c);
	cm_opcy_class = c;
}

//--------------------------------------------------------------------------

void *cm_opcy_new() {
	t_cm_opcy *x;                                             // local variable (pointer to a t_cm_opcy data structure)

	x = (t_cm_opcy *)object_alloc(cm_opcy_class);           // create a new instance of this object

    x->pcs_in = proxy_new(x, 1, &x->p_in);                  // create second inlet
    x->cm_nrc=listout(x);                                   // create outlets
    x->cm_out=outlet_new(x, NULL);
    
    x->pcs=NULL;
    
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs->find[0]=EOC;
    x->cm=(CM*)malloc(sizeof(CM));
    Inicializar(x->cm);
    
	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void cm_opcy_assist(t_cm_opcy *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET) {
        switch(a) {
            case(0):
                sprintf(s,"Cycles operator to be used: 6, -6(inversion), 3, 9, 4, 8, 2 or 10");
                break;
            case(1):
                sprintf(s,"Pointer to a PCS structure");
                break;
        }
    }
    if (m == ASSIST_OUTLET) {
        switch(a) {
            case(0):
                sprintf(s,"Pointer to a CM structure");
                break;
            case(1):
                sprintf(s,"Number of rows and columns (list)");
                break;
        }
    }
}

void cm_opcy_pcs_ptr(t_cm_opcy *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=1) {
        object_error((t_object*)x, "left inlet doesn't accept pointers");
        return;
    }
    
    {  //------------ get ptr --------------
        t_symbol *temp;
        PCS *tempcs;
        temp = atom_getsym(argv);
            
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
        CopiaSet(tempcs,x->pcs);                    //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------
    
    return;
}

void cm_opcy_int(t_cm_opcy *x, long argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    if(inum !=0) {
        object_error((t_object*)x, "wrong message");
        return;
    }
    
    int TnI;
    
    if(x->pcs->find[0]==EOC) {
        object_warn((t_object*)x, "pointer to pcs not received");
        return;
    }
    TnI = argv;
    
    if(TnI == 5 || TnI == 7 || TnI > 10) {
        object_post((t_object*)x, "wrong cycles operator, valid choices are: -6, 6, 2, 3, 4, 8, 9 or 10");
        return;
    }
    if(TnI < 2) {
        if(TnI != -6) {
            object_post((t_object*)x, "wrong cycles operator, valid choices are: -6, 6, 2, 3, 4, 8, 9 or 10");
            return;
        }
        else {
            TnI=abs(TnI)+INVERT; /*invert*/
        }
    }
    if(MatFCycles(x->cm, x->pcs,TnI)==FALSE)  {
        object_warn((t_object*)x, "could not make the CM with this PCS");
        return;
    }
    
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

void cm_opcy_free(t_cm_opcy *x){
    
    if(x->pcs != NULL){
        free(x->pcs);
    }
    if(x->cm != NULL){
        free(x->cm);
    }
    return;
}