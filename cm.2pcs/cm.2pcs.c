/*  cm_2pcs object  by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
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

typedef struct _cm_2pcs {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    CM *cm;		  /*pointers to CM structs*/
    PCS *pcs;
    t_int c;
    t_int r;
    
    long p_in;
    void *cm_in;
    void *pcs_out;
} t_cm_2pcs;

// these are prototypes for the methods that are defined below
void cm_2pcs_cm_ptr(t_cm_2pcs *x, t_symbol *s, long argc, t_atom *argv);

void cm_2pcs_pos(t_cm_2pcs *x, t_symbol *s, long argc, t_atom *argv);
void cm_2pcs_row(t_cm_2pcs *x, long n);
void cm_2pcs_col(t_cm_2pcs *x, long n);
void cm_2pcs_all(t_cm_2pcs *x);

void output (t_cm_2pcs *x);

void cm_2pcs_assist(t_cm_2pcs *x, void *b, long m, long a, char *s);
void stdinletinfo();
void cm_2pcs_free(t_cm_2pcs *x);
void *cm_2pcs_new();

static t_class *cm_2pcs_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("cm.2pcs", (method)cm_2pcs_new, (method)cm_2pcs_free, sizeof(t_cm_2pcs), 0L, 0);
    
    class_addmethod(c, (method)cm_2pcs_cm_ptr,         MMID,          A_GIMME, 0);    // pointer to CM
    
    class_addmethod(c, (method)cm_2pcs_pos,            "pos",         A_GIMME,  0);
    class_addmethod(c, (method)cm_2pcs_row,            "row",         A_LONG,  0);
    class_addmethod(c, (method)cm_2pcs_col,            "col",         A_LONG,  0);
    class_addmethod(c, (method)cm_2pcs_all,            "all",                  0);
    
    class_addmethod(c, (method)cm_2pcs_assist,         "assist",      A_CANT,  0);	// (optional) assistance method
    class_addmethod(c, (method)stdinletinfo,            "inletinfo",   A_CANT,  0);

	class_register(CLASS_BOX, c);
	cm_2pcs_class = c;
}

//--------------------------------------------------------------------------

void *cm_2pcs_new() {
	t_cm_2pcs *x;                                             // local variable (pointer to a t_cm_2pcs data structure)

	x = (t_cm_2pcs *)object_alloc(cm_2pcs_class);           // create a new instance of this object
    
    x->cm_in = proxy_new(x, 1, &x->p_in);
                                     // create outlets
    x->pcs_out=outlet_new(x, NULL);
    
    x->cm=NULL;
    x->pcs=NULL;
    
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs->find[0]=EOC;
    
    x->cm=(CM*)malloc(sizeof(CM));
    Inicializar(x->cm);
    
	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void cm_2pcs_assist(t_cm_2pcs *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET) {
        switch(a) {
            case(0):
                sprintf(s,"pos (row, column), row (n), col (n), all");
                break;
            case(1):
                sprintf(s,"Pointer to a CM structure");
                break;
        }
    }
    if (m == ASSIST_OUTLET) {
            sprintf(s,"Pointer to a PCS structure");
    }
}

void cm_2pcs_cm_ptr(t_cm_2pcs *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=1) {
        object_error((t_object*)x, "wrong message");
        return;
    }
    
    {  //----------- get cm ptr ------------
        t_symbol *temp;
        CM *tempcm;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcm);    // get the pointer to a PCS struct
        CopiaMatriz(tempcm, x->cm);                    //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------
    
    return;
}

void cm_2pcs_pos(t_cm_2pcs *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "wrong message");
        return;
    }
    if(argc != 2){
        object_error((t_object*)x, "missing or extra arguments for message pos");
        return;
    }
    for(int n=0; n<argc; n++) {
        if(atom_gettype(argv+n) != A_LONG && atom_gettype(argv+n) != A_FLOAT) {
            object_error((t_object *)x, "bad arguments for message pos");
            return;
        }
    }
    
    x->r=atom_getlong(argv);
    x->c=atom_getlong(argv+1);
    if(check_rc(x->cm, x->r,x->c)==FALSE){
        object_warn((t_object*)x, "warning: out of range row/column");
        return;
    }
    x->pcs->find[0]=EOC;
    pos_to_pcs(x->cm, x->pcs, x->r,x->c);
    output(x);
    return;
}
void cm_2pcs_row(t_cm_2pcs *x, long n) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "wrong message");
        return;
    }
    x->r=n;
    if(x->r < 0 || x->r > x->cm->NroFilas-1){
        object_warn((t_object*)x, "warning: out of range row");
        return;
    }
    row_to_pcs(x->cm, x->pcs, x->r);
    output(x);
    return;
}
void cm_2pcs_col(t_cm_2pcs *x, long n) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "wrong message");
        return;
    }
    x->c=n;
    if(x->c < 0 || x->c > x->cm->NroCols-1){
        object_warn((t_object*)x, "warning: out of range column");
        return;
    }
    col_to_pcs(x->cm, x->pcs, x->c);
    output(x);
    return;
}
void cm_2pcs_all(t_cm_2pcs *x) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "wrong message");
        return;
    }
    cm_to_pcs(x->cm, x->pcs);
    output(x);
}

void output (t_cm_2pcs *x) {
    {   //------------- out ptr -------------------
        char pstr[STRLP];
        t_atom ptr;
        sprintf(pstr, "%p", x->pcs);    //convert pointer to PCS struct into symbol //- (cuidado con el nombre de la struct)
        atom_setsym(&ptr, gensym(pstr));
        outlet_anything (x->pcs_out, gensym(MPID), 1, &ptr);    //- (cuidado con el nombre del outlet)
    }   //------------- end out -------------------
}

void cm_2pcs_free(t_cm_2pcs *x){
    
    if(x->cm != NULL){
        free(x->cm);
    }
    if(x->pcs != NULL){
        free(x->pcs);
    }
    return;
}