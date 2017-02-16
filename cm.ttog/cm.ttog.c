/*  cm_ttog object  by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
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

typedef struct _cm_ttog {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    PCS *pcs; 	  /*pointer to PCS struct*/
    CM  *cm;
    TTOG *ttog;
    long p_in;
    void *ttog_in;
    void  *cm_out, *cm_nrc;
} t_cm_ttog;

// these are prototypes for the methods that are defined below
void cm_ttog_pcs_ptr(t_cm_ttog *x, t_symbol *s, long argc, t_atom *argv);
void cm_ttog_ttog(t_cm_ttog *x, t_symbol *s, long argc, t_atom *argv);

void cm_ttog_assist(t_cm_ttog *x, void *b, long m, long a, char *s);
void stdinletinfo();
void cm_ttog_free(t_cm_ttog *x);
void *cm_ttog_new();

static t_class *cm_ttog_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("cm.ttog", (method)cm_ttog_new, (method)cm_ttog_free, sizeof(t_cm_ttog), 0L, 0);
    
    class_addmethod(c, (method)cm_ttog_pcs_ptr,        MPID,          A_GIMME, 0);    // pointer to PCS
    class_addmethod(c, (method)cm_ttog_ttog,           "list",        A_GIMME, 0);    // ttog
    
    class_addmethod(c, (method)cm_ttog_assist,         "assist",      A_CANT,  0);	// (optional) assistance method
    class_addmethod(c, (method)stdinletinfo,           "inletinfo",   A_CANT,  0);

	class_register(CLASS_BOX, c);
	cm_ttog_class = c;
}

//--------------------------------------------------------------------------

void *cm_ttog_new() {
	t_cm_ttog *x;                                             // local variable (pointer to a t_cm_ttog data structure)

	x = (t_cm_ttog *)object_alloc(cm_ttog_class);           // create a new instance of this object
    
    x->ttog_in = proxy_new(x, 1, &x->p_in);                                 // create outlets
    x->cm_nrc=listout(x);
    x->cm_out=outlet_new(x, NULL);
    
    x->pcs=NULL;
    x->cm=NULL;
    x->ttog=NULL;
    
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs->find[0]=EOC;
    
    x->cm=(CM*)malloc(sizeof(CM));
    Inicializar(x->cm);
    
    x->ttog=(TTOG*)malloc(sizeof(TTOG));
    ttog_init(x->ttog);
    
	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void cm_ttog_assist(t_cm_ttog *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET) {
        switch(a) {
            case(0):
                sprintf(s,"(PCS) Horizontal norm");
                break;
            case(1):
                sprintf(s,"(list) Group Type, Class and Member");
                break;
        }
    }
    if (m == ASSIST_OUTLET) {
        switch(a) {
            case(0):
                sprintf(s,"CM");
                break;
            case(1):
                sprintf(s,"(list) Number of rows and columns");
                break;
        }
    }
}

void cm_ttog_pcs_ptr(t_cm_ttog *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    if(inum!=0) {
        object_error((t_object*)x, "wrong message");
        return;
    }
    
    {  //------------ get ptr --------------
        t_symbol *temp;
        PCS *tempcs;
        temp = atom_getsym(argv);
            
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
        CopiaSet(tempcs,x->pcs);                    //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------
    
    if(x->pcs->ncar !=3 && x->pcs->ncar !=4 && x->pcs->ncar !=6) {
        object_warn((t_object*)x, "allowed cardinality for PCS are only 3, 4 or 6");
        x->pcs->find[0]=EOC;
        return;
    }
    if(x->ttog->op[0]==EOC) {
        object_warn((t_object*)x, "TTO GROUP not received; no action taken");
        return;
    }
    else {
        if(x->pcs->ncar != x->ttog->or) {
            object_warn((t_object*)x, "error, the cardinal of the PCS and the Order of the TTO Group must match; no action taken");
            return;
        }
    }
    if(cm_ttog(x->cm, x->ttog, x->pcs)== true) {
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
    }
    else {
        object_warn((t_object*)x, "sorry, could not make the CM, not yet implemented");
    }
    return;
}

void cm_ttog_ttog(t_cm_ttog *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    if(inum!=1) {
        object_error((t_object*)x, "bad pointer");
        return;
    }
    if(argc<2) {
        object_warn((t_object*)x, "missing arguments for ttog message");
        return;
    }
    for(t_int i=0; i<argc; i++) {
        if (atom_gettype(argv+i)!=A_LONG && atom_gettype(argv+i)!=A_FLOAT) {
            object_error((t_object*)x, "wrong arguments for message 'ttog'");
            break;
            return;
        }
    }
    x->ttog->ty = (int)atom_getlong(argv);
    x->ttog->cl = (int)atom_getlong(argv+1);
    if(argc >=3)
        x->ttog->mb = (int)atom_getlong(argv+2);
    else
        x->ttog->mb=1;
    
    if(ttog_read_group (x->ttog)==false) {
        object_warn((t_object*)x, "error, invalid group type, class or member");
        ttog_init(x->ttog);
        return;
    }
}

void cm_ttog_free(t_cm_ttog *x){
    if(x->pcs != NULL) free(x->pcs);
    if(x->cm != NULL) free(x->cm);
    if(x->ttog != NULL) free(x->ttog);
    return;
}
