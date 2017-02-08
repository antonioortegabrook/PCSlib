/*                                              Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
-------

 */

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS
 
        Borrar variables no utilizadas...?
*/

#include "ext.h"			//      Max headers
#include "ext_obex.h"		//

#include "../pcslib-includes/pcslib_max.c"       //    PCSlib header

typedef struct _cm_ana {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    CM *cm;   // pointer to a CM struct
    PCS *pcs, *pcs2;
    t_int c;
    t_int r;
    t_atom hist[12];
    t_atom aset[12];
    void *frag_out, *spar_out, *hist_out;
} t_cm_ana;

// these are prototypes for the methods that are defined below
void cm_ana_cm_ptr(t_cm_ana *x, t_symbol *s, long argc, t_atom *argv);

void cm_ana_assist(t_cm_ana *x, void *b, long m, long a, char *s);

void cm_ana_free(t_cm_ana *x);              //-
void *cm_ana_new();

static t_class *cm_ana_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("cm.ana", (method)cm_ana_new, (method)cm_ana_free, sizeof(t_cm_ana), 0L, 0);
    
    class_addmethod(c, (method)cm_ana_cm_ptr,        MMID,		A_GIMME, 0);
    
	class_addmethod(c, (method)cm_ana_assist,        "assist",	A_CANT,  0);	// (optional) assistance method

	class_register(CLASS_BOX, c);
	cm_ana_class = c;
}

//--------------------------------------------------------------------------

void *cm_ana_new() {
	t_cm_ana *x;                                             // local variable (pointer to a t_cm_ana data structure)
    
	x = (t_cm_ana *)object_alloc(cm_ana_class);           // create a new instance of this object
    
    x->hist_out=listout(x);
    x->spar_out=floatout(x);
    x->frag_out=floatout(x);
     
    x->cm=NULL;
    
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs->find[0]=EOC;
    
    x->pcs2=(PCS*)malloc(sizeof(PCS));
    x->pcs2->find[0]=EOC;
    
    x->cm=(CM*)malloc(sizeof(CM));
    Inicializar(x->cm);
    
    return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void cm_ana_assist(t_cm_ana *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET)
        sprintf(s,"Pointer to a CM structure");
        
    if (m == ASSIST_OUTLET) {
        switch(a) {
            case(0):
                sprintf(s,"(float) Fragmentation Coefficient");
                break;
            case(1):
                sprintf(s,"(float) Sparseness Coefficient");
                break;
            case(2):
                sprintf(s,"(list) Pitch-Frequency Array");
                break;
        }
    }
}

void cm_ana_free(t_cm_ana *x){
    
    if(x->cm != NULL){
        free(x->cm);
    }
    if(x->pcs != NULL){
        free(x->pcs);
    }
    if(x->pcs != NULL){
        free(x->pcs2);
    }
    return;
}

void cm_ana_cm_ptr(t_cm_ana *x, t_symbol *s, long argc, t_atom *argv) {
    t_int i;
    float fr, sp, tempf;
    float pfa[12];
    t_atom *pfalist;
    
    {  //------------ get ptr --------------
        t_symbol *temp;
        CM *tempcm;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcm);    // get the pointer to a CM struct
        CopiaMatriz(tempcm,x->cm);                    //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------
    
    hist(x->cm,pfa);    // histogram
    sp=spar(x->cm);     // spareseness
    fr=frag(x->cm);     // fragmentation
    
    pfalist=x->hist;
    for(i=0; i<12; ++i) {
        tempf=pfa[i];
        atom_setlong(&(pfalist[i]),tempf);
    }
    outlet_list(x->hist_out, gensym("list"),12,x->hist);    // histogram out
    outlet_float(x->spar_out,sp);                           // spareseness out
    outlet_float(x->frag_out,fr);                           // fragmentation out
    
    return;
}

