/*  pcs_subs object by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
------- 
Esta versión de pcs. subs recibe el puntero por el inlet derecho y un int por el izquierdo, al revés que la original pero igual que otros objetos como pcs.ttos, pcs.perm pcs.invar, etc.
*/

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS
 
*/

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

#include "../pcslib-includes/pcslib_max.c"       //    PCSlib header

typedef struct _pcs_subs {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    PCS *pcs;   /*pointer to PCS struct*/
    t_int n;  //-   <-- float convertido a int
    t_int a;	/*the "a" flag=true if the adjacent subsets are required and false if not*/
    
    long p_in;
    void *pcs_in;
    void  *ns_out;
    void  *pcs_out;
} t_pcs_subs;

// these are prototypes for the methods that are defined below
void pcs_subs_pcs_ptr(t_pcs_subs *x, t_symbol *s, long argc, t_atom *argv);  //-
void pcs_subs_int(t_pcs_subs *x, int n);

void pcs_subs_assist(t_pcs_subs *x, void *b, long m, long a, char *s);
void stdinletinfo();

void pcs_subs_free(t_pcs_subs *x);              //- _destroy traduce como _free CAMBIAR TAMBIÉN EN LA FUNCIÓN!!!
void *pcs_subs_new(long n);    //- <-- revisar ese parámetro long n!!!

static t_class *pcs_subs_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("pcs.subs", (method)pcs_subs_new, (method)pcs_subs_free, sizeof(t_pcs_subs), 0L, 0);
    
    class_addmethod(c, (method)pcs_subs_pcs_ptr,		MPID,		A_GIMME, 0);
    class_addmethod(c, (method)pcs_subs_int,            "int",		A_LONG, 0);    // int in the right inlet (inlet 1)
	
    class_addmethod(c, (method)pcs_subs_assist,         "assist",	A_CANT, 0);	// (optional) assistance method
    class_addmethod(c, (method)stdinletinfo,            "inletinfo", A_CANT, 0);    // method for hot and cold inlets

	class_register(CLASS_BOX, c);
	pcs_subs_class = c;
}

//--------------------------------------------------------------------------

void *pcs_subs_new() {
	t_pcs_subs *x;                                             // local variable (pointer to a t_pcs_subs data structure)

	x = (t_pcs_subs *)object_alloc(pcs_subs_class);           // create a new instance of this object
	
    x->pcs_in = proxy_new(x, 1, &x->p_in);              // create proxy for the ptr
    
    x->ns_out = intout(x);                                    // create an int outlet
    x->pcs_out = outlet_new(x, NULL);                                    // create a list outlet

    x->pcs=NULL;
    
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs->find[0]=EOC;
    x->n=3;/*default smallest value for n*/
    x->a=false; /*all subsets are required*/
    

	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void pcs_subs_assist(t_pcs_subs *x, void *b, long m, long a, char *s) // 4 final arguments are always the same for the assistance method
{
    if (m == ASSIST_INLET) {
        switch(a) {
            case 0:
                sprintf(s,"(int) Cardinal number of the subsets to be calculated");
                break;
            case 1:
                sprintf(s,"PCS");
                break;
        }
    }
    if (m == ASSIST_OUTLET) {
        switch(a) {
            case 0:
                sprintf(s,"Series of PCS");
                break;
            case 1:
                sprintf(s,"(int) Number of subsets");
                break;
        }
    }
        
}

void pcs_subs_free(t_pcs_subs *x){
    
    if(x->pcs != NULL)
        free(x->pcs);
    
    return;
}

void pcs_subs_pcs_ptr(t_pcs_subs *x, t_symbol *s, long argc, t_atom *argv) {
    long inum = proxy_getinlet((t_object *)x);
    
    if(inum !=1) {
        error_sym(x, gensym("only accepts pointers in its second inlet"));
        return;
    }
    
    if(strcmp(s->s_name, "ADJ")==0) {
        x->a=true;
        return;
    }

    {  //------------ get ptr --------------
        t_symbol *temp;
        PCS *tempcs;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
        CopiaSet(tempcs,x->pcs);                    //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------
    
    // check for input errors
    if(x->pcs->ncar >9 || x->pcs->ncar <4) {
        object_warn((t_object*)x, "cardinal of input set must be greater than 3 and less than 10");
        return;
    }
    return;		
}

void pcs_subs_int(t_pcs_subs *x, int f) {       //- Ver este tema del int
    
    t_int i,h,j,s_n,err;
    PCS *pcsbuf=NULL;
    PCS *tpcsbuf=NULL;
    PCS tempcs;
    int npcs[7]={12,29,38,50,38,29,12};
    
    x->n=f;
    
    if(x->pcs->find[0] ==EOC) {
        object_warn((t_object*)x, "no pointer to PCS received");
        return;
    }
    if(x->n >= x->pcs->ncar || x->n <3.) {
        object_warn((t_object*)x, "cardinal of subsets set must be equal or greater than 3 and less than cardinal of set");
        return;
    }
//----
    else {
        if(x->a==false) {
            s_n=get_n_subsets(x->pcs->ncar,(int)x->n);
            pcsbuf=(PCS*)malloc(sizeof(PCS)*s_n);
            tpcsbuf=(PCS*)malloc(sizeof(PCS)*s_n);
            err=Subsets(x->pcs,(int)x->n,tpcsbuf,tableptr);
            if(err==0) {
                j=0;
                /*sort by ordinal number*/
                for(i=1; i<=npcs[(int)(x->n-3.)]; i++){
                    for(h=0; h < s_n; h++){
                        if(tpcsbuf[h].nord==i) {
                            pcsbuf[j++]=tpcsbuf[h];
                        }
                    }
                }
            }
            else{
                free(pcsbuf);
                free(tpcsbuf);
                object_error((t_object*)x, "pcs_subs failed");
                return;
            }
        }
        else {
            x->a=false;
            s_n=(x->pcs->ncar-(int)x->n) + 1;
            pcsbuf=(PCS*)malloc(sizeof(PCS)*s_n);
            for(i=0; i<s_n; ++i) {
                for(h=0; h<x->n; ++h) {
                    tempcs.find[h]=x->pcs->find[h+i];
                }
                tempcs.find[h]=EOC;
                forma_prima(&tempcs, tableptr);
                CopiaSet(&tempcs,&pcsbuf[i]);
            }
        }
    }
 //----
    
    outlet_int(x->ns_out, (t_int)s_n);  //send the number of subsets

    for(int i=0; i < s_n; i++) {        // send a series of pointers
        
        {   //------------- out ptr -------------------
            char pstr[STRLP];
            t_atom ptr;
            sprintf(pstr, "%p", &pcsbuf[i]);    //convert pointer to PCS struct into symbol //- (cuidado con el nombre de la struct)
            atom_setsym(&ptr, gensym(pstr));
            outlet_anything (x->pcs_out, gensym(MPID), 1, &ptr);    //- (cuidado con el nombre del outlet)
        }   //------------- end out -------------------
    }
    return;
}

