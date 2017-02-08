/*  pcs_perm object  by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
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

#include "../pcslib-includes/pcslib_max.c"       //    PCSlib

typedef struct _pcs_perm {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    t_int np;	      /*number of different permutations*/
    t_int n;		  /*size of the PCS (without repetitions and/or spaces)*/
    PCS *pcs;		  /*pointer to PCS struct*/
    int *pl;        /*array for storing the permutations*/
    
    long p_in;
    void *pcs_in;
    void *perm_out, *np_out;
} t_pcs_perm;

// these are prototypes for the methods that are defined below
void pcs_perm_pcs_ptr(t_pcs_perm *x, t_symbol *s, int argc, t_atom *argv);
void pcs_perm_compute_perm(t_pcs_perm *x);
void pcs_perm_bang(t_pcs_perm *x);
void pcs_perm_int(t_pcs_perm *x, int n);

void pcs_perm_assist(t_pcs_perm *x, void *b, long m, long a, char *s);
void stdinletinfo();
void pcs_perm_free(t_pcs_perm *x);

void *pcs_perm_new();

static t_class *pcs_perm_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("pcs.perm", (method)pcs_perm_new, (method)pcs_perm_free, sizeof(t_pcs_perm), 0L, 0);
    
    class_addmethod(c, (method)pcs_perm_pcs_ptr,		MPID,		A_GIMME, 0);    // pointer to PCS
    class_addmethod(c, (method)pcs_perm_bang,           "bang",              0);    // bang (all perms)
    class_addmethod(c, (method)pcs_perm_int,            "int",      A_LONG,  0);    // get $1   (get specific perm)
    
	class_addmethod(c, (method)pcs_perm_assist,         "assist",	A_CANT, 0);     // assistance method
    class_addmethod(c, (method)stdinletinfo,            "inletinfo", A_CANT, 0);    // method for hot and cold inlets

	class_register(CLASS_BOX, c);
	pcs_perm_class = c;
}

//--------------------------------------------------------------------------

void *pcs_perm_new() {
	t_pcs_perm *x;                                             // local variable (pointer to a t_pcs_perm data structure)

	x = (t_pcs_perm *)object_alloc(pcs_perm_class);           // create a new instance of this object

    x->pcs_in = proxy_new(x, 1, &x->p_in);              // create proxy for the ptr
    x->np_out=intout(x);                                 // create outlets
    x->perm_out=listout(x);
    
    x->pcs=NULL;
    
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs->find[0]=EOC;
    
    x->pl=(int*)malloc(sizeof(int)*PCSL);
    x->n=0;
    x->np=0;
    
	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void pcs_perm_assist(t_pcs_perm *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET) {
        switch(a) {
            case(0):
                sprintf(s,"bang outputs all the permutations, int outputs a specific one");
                break;
            case(1):
                sprintf(s,"PCS");
                break;
        }
    }
    if (m == ASSIST_OUTLET) {
        switch(a) {
            case(0):
                sprintf(s,"Permutations (series of lists)");
                break;
            case(1):
                sprintf(s,"Number of permutations (int)");
                break;
        }
    }
}

void pcs_perm_free(t_pcs_perm *x){
    
    if(x->pcs != NULL){
        free(x->pcs);
    }
    
    if(x->pl != NULL){
        free(x->pl);
    }
    return;
}

void pcs_perm_pcs_ptr(t_pcs_perm *x, t_symbol *s, int argc, t_atom *argv) {     //- get PCS pointer
    long inum = proxy_getinlet((t_object *)x);
    
    if(inum !=1) {
        error_sym(x, gensym("only accepts pointers in its second inlet"));
        return;
    }

    {  //------------ get ptr --------------
        t_symbol *temp;                 //- se podría omitir la variable temp...?
        PCS *tempcs;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
        CopiaSet(tempcs,x->pcs);                    //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------
    
    
    if(x->pcs->ncar < 3) {
        post_sym(x, gensym("PCS must be of cardinal > 3"));
        x->pcs->find[0]=EOC;
        return;
    }
    
    pcs_perm_compute_perm(x);       //- compute all perms
    
    return;
}

void pcs_perm_bang(t_pcs_perm *x) {
    t_atom perm_list[12];
    int i,j, index=0;
    int aux;
//    t_symbol *temp;                                                           //- unused variable 'temp'
    
    if(x->pcs->find[0]==EOC) {         //- ver esto para pcs_chain
        error_sym(x, gensym("no pointer to PCS received"));
        return;
    }
    
    /*send all the permutations only if the set size is <=7 */
    if(x->n <=7) {
        for(i=0; i<x->np; ++i) {
            for(j=0; j < x->n; ++j) {
                aux = x->pl[index+j];
                atom_setlong(&(perm_list[j]),aux);
            }
            index+=x->n;
            outlet_list(x->perm_out, gensym("list"),x->n,perm_list);
        }
    }
    else {
        post_sym(x, gensym("bang is only accepted when the PCS size is < 8!"));
    }
    
    return;
}

void pcs_perm_int(t_pcs_perm *x, int n) {
    
    t_atom perm_list[12];
    int j, index=0;
    int aux;
    
    if(x->pcs->find[0]==EOC) {
        error_sym(x, gensym("no pointer to PCS received"));
        return;
    }
    
    index=n;
    
    if(index < 0 || (t_int)index > x->np-1 ) {
        post_sym(x, gensym("number of permutation out of range!"));
        return;
    }
    if(x->n <=7) {
        index*=x->n;
        for(j=0; j < x->n; ++j) {
            aux = x->pl[index+j];
            atom_setlong(&(perm_list[j]),aux);
        }
    }
    else {
        get_permutation(x->pl, x->np, x->n, index);
        for(j=0; j < x->n; ++j) {
            aux = x->pl[x->n+j];
            atom_setlong(&(perm_list[j]),aux);
        }
    }
    
    outlet_list(x->perm_out, gensym("list"), x->n, perm_list);
    return;
}

void pcs_perm_compute_perm(t_pcs_perm *x) {    
    x->n=0;
    x->np=0;
    x->n=no_rep(x->pcs,x->pl);
    x->np=factorial(x->n);
    
    /*if the size of the set is < 8 its permutations are previously computed*/
    if(x->n < 8) {
        x->pl=(int*)realloc(x->pl,x->np*x->n*sizeof(int));      //- revisar si estos (int) son necesarios
        permutations(x->pl,x->np,x->n);
    }
    /*if the size of the set is >= 8 its permutations are NOT previously computed and
     a specific number of permutation must be requested
     */
    else {
        /*allocate only space for the set and for just one permutation*/
        x->pl=(int*)realloc(x->pl,2*x->n*sizeof(int));
    }
    outlet_int(x->np_out, x->np);
    return;
}

