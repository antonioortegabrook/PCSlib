/*  pcs_invar object  by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
-------
 The pcs_invar object expects a series of pointers to  PCS structs (must be at least 2).
 The pointers must be generated using the prime_form object or any other.
 A "reset" message reset the list of PCS to 0.
 Any other message(most likely "bang") generates the output:
 outlet1: invariant pcs between all the pcs delivered  -if any-(floats list)
 outlet2: number of invariant pcs between all the pcs delivered  -if any-(float)
*/

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS
 
        Borrar variables no utilizadas...?
*/

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

#include "../pcslib-includes/pcslib_max.c"       //    PCSlib header

typedef struct _pcs_invar {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    PCS *pcs1;	  /*pointers to PCS struct*/
    t_atom filist[PCSL];    /*invariant subset*/
    t_int npcs;
    
    long p_in;
    void *pcs_in;
    void *list_out;
    void *nin;
} t_pcs_invar;

// these are prototypes for the methods that are defined below
void pcs_invar_bang(t_pcs_invar *x);  //- _list traducido como _anything
void pcs_invar_pcs_ptr(t_pcs_invar *x, t_symbol *s, int argc, t_atom *argv);
void pcs_invar_reset(t_pcs_invar *x /*, t_symbol *s, long argc, t_atom *argv */);

void pcs_invar_assist(t_pcs_invar *x, void *b, long m, long a, char *s);
void stdinletinfo();

void pcs_invar_free(t_pcs_invar *x);
void *pcs_invar_new();

static t_class *pcs_invar_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("pcs.invar", (method)pcs_invar_new, (method)pcs_invar_free, sizeof(t_pcs_invar), 0L, 0);
    
    class_addmethod(c, (method)pcs_invar_pcs_ptr,		MPID,         A_GIMME, 0); //- pointer to PCS
    class_addmethod(c, (method)pcs_invar_reset,         "reset",                 0);
    class_addmethod(c, (method)pcs_invar_bang,          "bang",                  0);
    
	class_addmethod(c, (method)pcs_invar_assist,        "assist",       A_CANT, 0);	// (optional) assistance method
    class_addmethod(c, (method)stdinletinfo,            "inletinfo", A_CANT, 0);    // method for hot and cold inlets

	class_register(CLASS_BOX, c);
	pcs_invar_class = c;
}

//--------------------------------------------------------------------------

void *pcs_invar_new() {
	t_pcs_invar *x;                                             // local variable (pointer to a t_pcs_invar data structure)

	x = (t_pcs_invar *)object_alloc(pcs_invar_class);           // create a new instance of this object

    x->pcs_in = proxy_new(x, 1, &x->p_in);              // create proxy for the ptr
    x->nin=intout(x);                                 // create outlets
    x->list_out=listout(x);
    
    x->pcs1=NULL;
    
    x->npcs=0;
    x->pcs1=(PCS*)malloc(sizeof(PCS));
    x->pcs1[0].find[0]=EOC;

	return(x);					// return a reference to the object instance
}

//--------------------------------------------------------------------------

void pcs_invar_assist(t_pcs_invar *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET) {
        switch (a) {
            case (0):
                sprintf(s,"bang outputs the invariant set, 'reset' clears the PCS list");
                break;
            case(1):
                sprintf(s,"Two or more pointers to PCS");
                break;
        }
    }
    if (m == ASSIST_OUTLET) {
        switch(a) {
            case(0):
                sprintf(s,"Shared pitch-classes (invariant set) (list)");
                break;
            case(1):
                sprintf(s,"Number of shared pitch-classes (size of the invariant set) (int)");
                break;
        }
    }
}

void pcs_invar_free(t_pcs_invar *x){
    
    if(x->pcs1 != NULL){
        free(x->pcs1);
    }
    return;
}

void pcs_invar_pcs_ptr(t_pcs_invar *x, t_symbol *s, int argc, t_atom *argv) {
    long inum = proxy_getinlet((t_object *)x);
    
    if(inum !=1) {
        object_error((t_object*)x, "left inlet doesn't accept pointers");
        return;
    }

    {  //------------ get ptr --------------
        t_symbol *temp;
        PCS *tempcs;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
        x->npcs++;
        x->pcs1=(PCS*)realloc(x->pcs1,x->npcs*sizeof(PCS));
        CopiaSet(tempcs,&x->pcs1[x->npcs-1]);      //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------

    return;
}

void pcs_invar_bang(t_pcs_invar *x) {   //- agregar condicional para filtrar el inlet
    t_int h,i,j,ptr=0, cont=0;
    t_int tempf;    //- ex float, cambiado a int
    t_atom *filist;
    t_int aux[PCSL];
    
    if(x->npcs==0){
        object_error((t_object*)x, "no pointer/s to PCS received");
        return;
    }
    if(x->npcs==1){
        object_warn((t_object*)x, "need at leat two PCS");
        return;
    }
    
    for(i=0; i < PCSL; ++i) {
        aux[i]=EOP;
    }
    
    for(j=0; j<12; ++j) {
        cont=0;
        for(i=0; i < x->npcs; ++i) {
            h=0;
            do{
                if(x->pcs1[i].find[h]==j && x->pcs1[i].find[h] != EOP ) {
                    ++cont;
                    break;
                }
            }while(x->pcs1[i].find[h++] != EOC);
        }
        if(cont == x->npcs) { /*the pc was found on all pcs*/
            aux[ptr++]=j;
        }
    }
    
    
    //--------------
    
    /*output the size of the invariant set*/
    outlet_int(x->nin,(t_int)ptr);
    
    filist=x->filist;
    if(ptr != 0){
        for(i=0; i<=ptr; ++i) {
            tempf=(int)aux[i];
            atom_setlong(&(filist[i]),tempf);
        }
        /*output the invariant set*/
        outlet_list (x->list_out, gensym("list"),ptr,x->filist);
    }
    
    return;
}

void pcs_invar_reset(t_pcs_invar *x) {
    /*reset the PCS buffer*/
    x->npcs=0;
    x->pcs1=(PCS*)realloc(x->pcs1,sizeof(PCS));
    x->pcs1[0].find[0]=EOC;
    return;
}
