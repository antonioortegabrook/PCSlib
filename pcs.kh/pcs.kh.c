/*  pcs_kh object  by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
-------
 The pcs_kh object evaluates the Kh inclusion of SCs
 
 Input:
 hot inlet: "Kh" symbol to see if the two PCS belongs to the same Kh Complex.
 "Khall" symbol to have all the members of the Kh Complex to which the two PCS belongs.
 cold inlets: the two pointers of the PCS to be compared. Must be
 generated using the prime_form object or any other which outputs a pcs
 pointer.
 The output:
 outlet:
 For the case of "Kh" , if the two PCS belongs to the same Kh complex,
 a bang is fired, otherwise no action is taken.
 For the case of "Khall" , if the two PCS belongs to the same Kh complex,
 a succesion of lists with two floats each, being the first one of each couple the cardinal
 number of the Member SC and the second one the Ordinal number. If the
 two PCS does not belongs to the same Kh complex, no action is taken.
 
*/

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS
 
        Borrar variables no utilizadas...?
*/

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

#include "../pcslib-includes/pcslib_max.c"       //    PCSlib header

typedef struct _pcs_kh {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    float index;      //- ex t_float
    PCS* pcs1;
    PCS* pcs2;
    t_atom kl[2];
    
    long ptr_num;
    void *ptr2, *ptr1;
    
    void *kh_out, *khall_out;
} t_pcs_kh;

// these are prototypes for the methods that are defined below
void pcs_kh_pcs_ptr(t_pcs_kh *x, t_symbol *s, long argc, t_atom *argv);  //- PCS pointer

void pcs_kh_bang(t_pcs_kh *x);
void pcs_kh_kh(t_pcs_kh *x);
void pcs_kh_khall(t_pcs_kh *x);

int pcs_kh_check_input_errs(t_pcs_kh *x);
void pcs_kh_compute(t_pcs_kh *x, int fl);

void pcs_kh_out(t_pcs_kh *x);

void pcs_kh_assist(t_pcs_kh *x, void *b, long m, long a, char *s);
void stdinletinfo();
void pcs_kh_free(t_pcs_kh *x);

void *pcs_kh_new();

static t_class *pcs_kh_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("pcs.kh", (method)pcs_kh_new, (method)pcs_kh_free, sizeof(t_pcs_kh), 0L, A_DEFLONG, 0);
    
    class_addmethod(c, (method)pcs_kh_pcs_ptr,        MPID,          A_GIMME, 0);    // pointer to PCS
    
    class_addmethod(c, (method)pcs_kh_bang,           "bang",                 0);
    class_addmethod(c, (method)pcs_kh_kh,             "kh",                   0);
    class_addmethod(c, (method)pcs_kh_khall,          "khall",                0);
    
    class_addmethod(c, (method)pcs_kh_assist,         "assist",       A_CANT, 0);	// (optional) assistance method
    class_addmethod(c, (method)stdinletinfo,          "inletinfo",    A_CANT, 0);    // method for hot and cold inlets

	class_register(CLASS_BOX, c);
	pcs_kh_class = c;
}

//--------------------------------------------------------------------------

void *pcs_kh_new(long l) {
	t_pcs_kh *x;                                             // local variable (pointer to a t_pcs_kh data structure)

	x = (t_pcs_kh *)object_alloc(pcs_kh_class);           // create a new instance of this object

    x->ptr2 = proxy_new(x, 2, &x->ptr_num);                   // create second inlet
    x->ptr1 = proxy_new(x, 1, &x->ptr_num);
    
    x->khall_out=outlet_new(x, NULL);           // create outlets
    x->kh_out=bangout(x);
    
    x->pcs1=(PCS*)malloc(sizeof(PCS));
    x->pcs2=(PCS*)malloc(sizeof(PCS));
    x->pcs1->find[0]=EOC;
    x->pcs2->find[0]=EOC;
    
	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void pcs_kh_assist(t_pcs_kh *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET) {
        switch(a) {
            case(0):
                sprintf(s,"bang, kh, khall");
                break;
            case(1):
                sprintf(s,"PCS");
                break;
            case(2):
                sprintf(s,"PCS");
                break;
        }
    }
    if (m == ASSIST_OUTLET) {
        switch(a) {
            case(0):
                sprintf(s,"bang");
                break;
            case(1):
                sprintf(s,"Succesion of lists");
                break;
        }
    }
}

void pcs_kh_free(t_pcs_kh *x){
    if(x->pcs1 != NULL) free(x->pcs1);
    if(x->pcs2 != NULL) free(x->pcs2);
    return;
}

//----------------------------------------------------------------------

void pcs_kh_pcs_ptr(t_pcs_kh *x, t_symbol *s, long argc, t_atom *argv) {    //- ptr to PCS
    long inlet = proxy_getinlet((t_object *)x);     //- leftmost (default) inlet is #0 (so our proxies counts from 1)
    
    if(inlet==0) {
        object_error((t_object*)x, "left inlet doesn't accept PCS");
        return;
    }
    
    {  //------------ get ptr --------------
        t_symbol *temp;
        PCS *tempcs;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
        if(inlet==1)                                //- if it came from inlet #1, we copy it to x->pcs1
            CopiaSet(tempcs,x->pcs1);
        if(inlet==2)                                //- if it came from inlet #2, we copy it to x->pcs2
            CopiaSet(tempcs,x->pcs2);
    }  //------------ end get --------------

    return;
}

void pcs_kh_bang(t_pcs_kh *x) {    //- bang (compute everything)
    long inlet = proxy_getinlet((t_object *)x);
    
    if(inlet!=0) {
        object_error((t_object*)x, "no PCS received");
        return;
    }
    if(pcs_kh_check_input_errs(x)!=0)
        return;
    
    pcs_kh_compute(x, 0);
    pcs_kh_compute(x, 1);
    
    return;
}

void pcs_kh_kh(t_pcs_kh *x) {    //- kh
    long inlet = proxy_getinlet((t_object *)x);
    
    if(inlet!=0) {
        object_error((t_object*)x, "no PCS pointer received");
        return;
    }
    if(pcs_kh_check_input_errs(x)!=0)
        return;
    
    pcs_kh_compute(x, 0);
    
    return;
}

void pcs_kh_khall(t_pcs_kh *x) {    //- khall
    long inlet = proxy_getinlet((t_object *)x);
    
    if(inlet!=0) {
        object_error((t_object*)x, "no PCS pointer received");
        return;
    }
    if(pcs_kh_check_input_errs(x)!=0)
        return;
    pcs_kh_compute(x, 1);
    
    return;
}
//----------------------------------------------------------------------

void pcs_kh_compute(t_pcs_kh *x, int fl) {
    t_int i;
    t_int cnt;
    t_int ppcs[3]={0,29,67};
    t_int kindx, kflag;
    t_atom *klist;
    int pos, zpos;
    cnt=0;
    
    
    
    
    /*compute index first*/
    /*the Z issue makes things complicated in cardinal 6 PCS...*/
    if(x->pcs1->ncar == 6 && x->pcs1->nord > 35 ) {
        pos=pcs_table_loc(x->pcs1->ncar,x->pcs1->nord);
        if(tableptr[ZLOC+pos] !=0) {
            zpos=tableptr[ZLOC+pos]-1;
            kindx=ppcs[2]*2 + (tableptr[zpos+1]-1)*2;
        }
        else{
            kindx=(ppcs[x->pcs1->ncar - 4]*2)+(x->pcs1->nord-1)*2;
        }
    }
    else{
        kindx=(ppcs[x->pcs1->ncar - 4]*2)+(x->pcs1->nord-1)*2;
    }
    
    /*Kh mess*/
    if(fl==0){          //- 0 = kh message
        for(i=1; i<MKS+1; ++i) {
            if(kdataptr[kindx][i]==0) break;
            if(kdataptr[kindx+1][i]==x->pcs2->nord && kdataptr[kindx][i]==x->pcs2->ncar){
                outlet_bang(x->kh_out);
                break;
            }
        }
    }
    
    
    if(fl==1){      //- 1 = khall message
        kflag=0;
        for(i=1; i<MKS+1; ++i) {
            if(kdataptr[kindx][i]==0) break;
            if(kdataptr[kindx+1][i]==x->pcs2->nord && kdataptr[kindx][i]==x->pcs2->ncar){
                kflag=1;
                break;
            }
        }
        if(kflag==1){
            klist=x->kl;
            for(i=1; i<MKS+1; ++i) {
                if(kdataptr[kindx][i]==0) break;
                atom_setlong(&(klist[0]), kdataptr[kindx][i]);
                atom_setlong(&(klist[1]), kdataptr[kindx+1][i]);
                outlet_list(x->khall_out, gensym("list"), 2, x->kl);
                /*if a member of the complex is cardinal 6 and Z, its Z pair is included as well*/
                if(kdataptr[kindx][i]==6) {
                    pos=pcs_table_loc(kdataptr[kindx][i],kdataptr[kindx+1][i]);
                    if(tableptr[ZLOC+pos] !=0) {
                        zpos=tableptr[ZLOC+pos]-1;
                        atom_setlong(&(klist[0]), tableptr[zpos]);
                        atom_setlong(&(klist[1]), tableptr[zpos+1]);
                        outlet_list(x->khall_out, gensym("list"),2,x->kl);
                    }
                }
            }
        }
    }
    return;
}

int pcs_kh_check_input_errs(t_pcs_kh *x) {
    // check for input errors
    if(x->pcs1->find[0]==EOC || x->pcs2->find[0]==EOC ){
        object_error((t_object*)x, "one or both PCS pointers not received");
        return 1;
    }
    if(NombresIguales(x->pcs1,x->pcs2)==true ) {
        object_warn((t_object*)x, "Input error, both PCS belongs to the same SC");
        return 1;
    }
    if(x->pcs1->ncar == x->pcs2->ncar) {
        object_warn((t_object*)x, "Input error, both PCS have the same cardinal number");
        return 1;
    }
    if(x->pcs1->ncar > 6 || x->pcs2->ncar > 6 || x->pcs1->ncar <= 3 || x->pcs2->ncar < 3) {
        object_warn((t_object*)x, "Input error, out of range cardinal number/s");
        return 1;
    }
    return 0;
}
