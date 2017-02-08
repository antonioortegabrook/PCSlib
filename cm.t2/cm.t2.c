/*  cm_t2 object  by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
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

typedef struct _cm_t2 {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    PCS *pcs1; // pointer to PCS struct1
    PCS *pcs2; // pointer to PCS struct2
    CM  *cm;   // pointer to CM struct
    long p_in;
    void *pcs_in;   // second inlet
    void  *cm_out, *cm_nrc;
} t_cm_t2;

// these are prototypes for the methods that are defined below
void cm_t2_pcs_ptr(t_cm_t2 *x, t_symbol *s, long argc, t_atom *argv);

void cm_t2_assist(t_cm_t2 *x, void *b, long m, long a, char *s);
void cm_t2_free(t_cm_t2 *x);
void *cm_t2_new();

static t_class *cm_t2_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("cm.t2", (method)cm_t2_new, (method)cm_t2_free, sizeof(t_cm_t2), 0L, 0);
    
    class_addmethod(c, (method)cm_t2_pcs_ptr,        MPID,          A_GIMME, 0);    // pointer to PCS
    
    class_addmethod(c, (method)cm_t2_assist,         "assist",      A_CANT,  0);	// (optional) assistance method
    class_addmethod(c, (method)stdinletinfo,         "inletinfo",   A_CANT,  0);

	class_register(CLASS_BOX, c);
	cm_t2_class = c;
}

//--------------------------------------------------------------------------

void *cm_t2_new() {
	t_cm_t2 *x;                                             // local variable (pointer to a t_cm_t2 data structure)

	x = (t_cm_t2 *)object_alloc(cm_t2_class);           // create a new instance of this object

    x->pcs_in = proxy_new(x, 1, &x->p_in);
    x->cm_nrc=listout(x);                               // create outlets
    x->cm_out=outlet_new(x, NULL);
    
    x->pcs1=NULL;
    x->pcs2=NULL;
    
    x->pcs1=(PCS*)malloc(sizeof(PCS));
    x->pcs1->find[0]=EOC;
    x->pcs2=(PCS*)malloc(sizeof(PCS));
    x->pcs2->find[0]=EOC;
    x->cm=(CM*)malloc(sizeof(CM));
    Inicializar(x->cm);
    
	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void cm_t2_assist(t_cm_t2 *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET) {
        switch(a) {
            case(0):
                sprintf(s,"(PCS) Horizontal norm");
                break;
            case(1):
                sprintf(s,"(PCS) Vertical norm");
                break;
        }
    }
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

void cm_t2_pcs_ptr(t_cm_t2 *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    {  //------------ get ptr --------------
        t_symbol *temp;
        PCS *tempcs;
        temp = atom_getsym(argv);
            
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
        //------------
        if(inum==1) {
            CopiaSet(tempcs,x->pcs2);
            return;
        }
        if(inum==0) {
            CopiaSet(tempcs,x->pcs1);
        }
    }  //------------ end get --------------
    if(x->pcs1->find[0]==EOC || x->pcs2->find[0]==EOC) {
        object_warn((t_object*)x, "one or both pointers to pcs not received");
        return;
    }
    
    MatTipo2(x->cm, x->pcs1,x->pcs2);   // Build the CM
    
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

void cm_t2_free(t_cm_t2 *x){
    
    if(x->pcs1 != NULL){
        free(x->pcs1);
    }
    if(x->pcs2 != NULL){
        free(x->pcs2);
    }
    if(x->cm != NULL){
        free(x->cm);
    }
    return;
}
