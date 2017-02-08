/*  pcs_ttos object  by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
-------
 

*/

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS
 
        Borrar variables no utilizadas...?
 
    Evaluar si es realmente necesario que el factor de transposición se ingrese como argumento y tenga un inlet específico; quizás debería recibir símbolo y lista en le inlet izquierdo, como los otros objetos;
    En pcs_ttos_pcs_ptr comentamos la línea x->pcs2->find[0]=EOC; para evitar lo siguiente: si el objeto tiene múltiples objetos conectados a su salida y recibe un nuevo puntero mientras está enviando la salida a los otros objetos, algunos de ellos recibirán un pcs vacío. Además, estamos rompiendo un pcs por uno que todavía no checkeamos. Revisar dónde está esa línea en el original
*/

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

#include "../pcslib-includes/pcslib_max.c"       //    PCSlib header

typedef struct _pcs_ttos {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    PCS *pcs1;		  /*pointer to PCS struct*/
    PCS *pcs2;
    t_int n;
    
    long p_in;
    void *pcs_in;
    void *pcs_out;
} t_pcs_ttos;

// these are prototypes for the methods that are defined below
void pcs_ttos_pcs_ptr(t_pcs_ttos *x, t_symbol *s, long argc, t_atom *argv);  //- PCS pointer
void pcs_ttos_in1(t_pcs_ttos *x, long l);
void pcs_ttos_t(t_pcs_ttos *x, t_symbol *s, long argc, t_atom *argv);
void pcs_ttos_i(t_pcs_ttos *x, t_symbol *s, long argc, t_atom *argv);
void pcs_ttos_m(t_pcs_ttos *x, t_symbol *s, long argc, t_atom *argv);
void pcs_ttos_mi(t_pcs_ttos *x, t_symbol *s, long argc, t_atom *argv);

void pcs_ttos_out(t_pcs_ttos *x);

void pcs_ttos_assist(t_pcs_ttos *x, void *b, long m, long a, char *s);
void stdinletinfo();

void pcs_ttos_free(t_pcs_ttos *x);
void *pcs_ttos_new();

static t_class *pcs_ttos_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("pcs.ttos", (method)pcs_ttos_new, (method)pcs_ttos_free, sizeof(t_pcs_ttos), 0L, A_DEFLONG, 0);
    
    class_addmethod(c, (method)pcs_ttos_pcs_ptr,        MPID,		 A_GIMME, 0);    // pointer to PCS
    class_addmethod(c, (method)pcs_ttos_in1,            "in1",       A_LONG,  0);
    class_addmethod(c, (method)pcs_ttos_t,              "t",         A_GIMME, 0);
    class_addmethod(c, (method)pcs_ttos_i,              "i",         A_GIMME, 0);
    class_addmethod(c, (method)pcs_ttos_m,              "m",         A_GIMME, 0);
    class_addmethod(c, (method)pcs_ttos_mi,             "mi",        A_GIMME, 0);
    
    class_addmethod(c, (method)pcs_ttos_assist,         "assist",    A_CANT, 0);	// (optional) assistance method
    class_addmethod(c, (method)stdinletinfo,            "inletinfo", A_CANT, 0);    // method for hot and cold inlets

	class_register(CLASS_BOX, c);
	pcs_ttos_class = c;
}

//--------------------------------------------------------------------------

void *pcs_ttos_new(long l) {
	t_pcs_ttos *x;                                             // local variable (pointer to a t_pcs_ttos data structure)

	x = (t_pcs_ttos *)object_alloc(pcs_ttos_class);           // create a new instance of this object

    x->pcs_in = proxy_new(x, 2, &x->p_in);              // create proxy for the ptr
    intin(x,1);                                          // create second inlet
    x->pcs_out = outlet_new(x, NULL);                                 // create outlets
    
    x->pcs1=NULL;
    x->pcs2=NULL;
    
    x->pcs1=(PCS*)malloc(sizeof(PCS));
    x->pcs1->find[0]=EOC;
    x->pcs2=(PCS*)malloc(sizeof(PCS));
    x->pcs2->find[0]=EOC;
    
    long tf=l;
    x->n=tf%12; // transposition factor initialized to typed argument (modulo 12), default 0 if argument not present
    
	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void pcs_ttos_assist(t_pcs_ttos *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET) {
        switch(a) {
            case(0):
                sprintf(s,"Pointer to a PCS structure, (int) operator, (list) operator and transposition factor");
                break;
            case(1):
                sprintf(s,"(int) Transposition factor");
                break;
        }
    }
    if (m == ASSIST_OUTLET) {
        sprintf(s,"Pointer to a PCS structure");
    }
}

void pcs_ttos_pcs_ptr(t_pcs_ttos *x, t_symbol *s, long argc, t_atom *argv) {
    long inum = proxy_getinlet((t_object *)x);

    if(inum !=2) {
        error_sym(x, gensym("only accepts pointers in its third inlet"));
        return;
    }
    
    {  //------------ get ptr --------------
        t_symbol *temp;
        PCS *tempcs;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
        CopiaSet(tempcs,x->pcs1);                    //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------
    
//    x->pcs2->find[0]=EOC;
    return;
}

void pcs_ttos_in1(t_pcs_ttos *x, long l) {
    long tf=l;
    x->n = tf%12;
    return;
}

void pcs_ttos_t(t_pcs_ttos *x, t_symbol *sy, long argc, t_atom *argv) {    //- Transp
    long tf;
    if(argc !=0) {  //- acá deberíamos checkear tipo (update: no es necesario... ;) )
        tf=atom_getlong(argv);
        tf=tf%12;
    }
    else tf=x->n;
    
    if(x->pcs1->find[0] !=EOC) {
        CopiaSet(x->pcs1,x->pcs2);
    }
    else {
        object_warn((t_object*)x, "no pointer to PCS received");
        return;
    }
    
    TrInvPCS(x->pcs2, FALSE, tf);       // compute t
    pcs_ttos_out(x);
    
    return;
}

void pcs_ttos_i(t_pcs_ttos *x, t_symbol *s, long argc, t_atom *argv) {    //- Inv
    long tf;
    if(argc !=0) {
        tf=atom_getlong(argv);
        tf=tf%12;
    }
    else tf=x->n;
    
    if(x->pcs1->find[0] !=EOC) {
        CopiaSet(x->pcs1,x->pcs2);
    }
    else {
        object_warn((t_object*)x, "pcs.ttos: no pointer to PCS received");
        return;
    }
    
    TrInvPCS(x->pcs2, TRUE, tf);        // compute i
    pcs_ttos_out(x);
    
    return;
}

void pcs_ttos_m(t_pcs_ttos *x, t_symbol *s, long argc, t_atom *argv) {    //- Mult
    long tf;
    if(argc !=0) {
        tf=atom_getlong(argv);
        tf=tf%12;
    }
    else tf=x->n;
    
    if(x->pcs1->find[0] !=EOC) {
            CopiaSet(x->pcs1,x->pcs2);
        }
    else {
        object_warn((t_object*)x, "pcs.ttos: no pointer to PCS received");
        return;
    }
    
    for(int i=0; i<PCSL; i++){                  // compute m
        if(x->pcs2->find[i] == EOC || x->pcs2->find[i] == EOP) break;
        x->pcs2->find[i] =x->pcs2->find[i] * 5 + tf;
        if(x->pcs2->find[i] > 11) {
            x->pcs2->find[i]=x->pcs2->find[i]%12;
        }
    }
    forma_prima(x->pcs2, tableptr);
    pcs_ttos_out(x);
    
    return;
}

void pcs_ttos_mi(t_pcs_ttos *x, t_symbol *s, long argc, t_atom *argv) {   //- Inv mult
    long tf;
    if(argc !=0) {
        tf=atom_getlong(argv);
        tf=tf%12;
    }
    else tf=x->n;
    
    if(x->pcs1->find[0] !=EOC) {
            CopiaSet(x->pcs1,x->pcs2);
        }
    else {
        object_warn((t_object*)x, "pcs.ttos: no pointer to PCS received");
        return;
    }
    
    for(int i=0; i<PCSL; i++){              // compute mi
        if(x->pcs2->find[i] == EOC || x->pcs2->find[i] == EOP) break;
        x->pcs2->find[i] =x->pcs2->find[i] * 7 + tf;
        if(x->pcs2->find[i] > 11) {
            x->pcs2->find[i]=x->pcs2->find[i]%12;
        }
    }
    forma_prima(x->pcs2, tableptr);
    pcs_ttos_out(x);
    
    return;
}

void pcs_ttos_out(t_pcs_ttos *x) {

    {   //------------- out ptr -------------------
        char pstr[STRLP];
        t_atom ptr;
        sprintf(pstr, "%p", x->pcs2);    //convert pointer to PCS struct into symbol //- (cuidado con el nombre de la struct)
        atom_setsym(&ptr, gensym(pstr));
        outlet_anything (x->pcs_out, gensym(MPID), 1, &ptr);    //- (cuidado con el nombre del outlet)
    }   //------------- end out -------------------

    return;
}

void pcs_ttos_free(t_pcs_ttos *x){
    
    if(x->pcs1 != NULL){
        free(x->pcs1);
    }
    if(x->pcs2 != NULL){
        free(x->pcs2);
    }
    return;
}
