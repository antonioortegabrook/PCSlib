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

typedef struct _pcs_print {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    PCS *pcs;		  //    pointer to PCS struct
    t_int sym;
    
    t_symbol name;
} t_pcs_print;

// these are prototypes for the methods that are defined below
void pcs_print_pcs_ptr(t_pcs_print *x, t_symbol *s, long argc, t_atom *argv);

void pcs_print_makestr(PCS* pcs, char *str);
void pcs_print_set(PCS *pcs, char *array);
void pcs_print_pf(PCS* pcs, char* pf);
void pcs_print_stat(PCS* pcs, char* pf);

void pcs_print_assist(t_pcs_print *x, void *b, long m, long a, char *s);

void pcs_print_free(t_pcs_print *x);              //-

void *pcs_print_new();

static t_class *pcs_print_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("pcs.print", (method)pcs_print_new, (method)pcs_print_free, sizeof(t_pcs_print), 0L, 0);
    
    class_addmethod(c, (method)pcs_print_pcs_ptr,		MPID,		A_GIMME, 0);
	class_addmethod(c, (method)pcs_print_assist,        "assist",	A_CANT,  0);	// (optional) assistance method

	class_register(CLASS_BOX, c);
	pcs_print_class = c;
}

//--------------------------------------------------------------------------

void *pcs_print_new() {
	t_pcs_print *x;                                             // local variable (pointer to a t_pcs_print data structure)
    
	x = (t_pcs_print *)object_alloc(pcs_print_class);           // create a new instance of this object
    
    x->pcs=NULL;
    
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs->find[0]=EOC;

	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void pcs_print_assist(t_pcs_print *x, void *b, long m, long a, char *s) {
	if (m == ASSIST_INLET)
		sprintf(s,"PCS");
    if (m == ASSIST_OUTLET) {
        sprintf(s,"(series of lists) PCS data");
    }
}

void pcs_print_free(t_pcs_print *x){
    
    if(x->pcs != NULL){
        free(x->pcs);
    }
    return;
}

void pcs_print_pcs_ptr(t_pcs_print *x, t_symbol *s, long argc, t_atom *argv) {
    char *str, *spcs;
    
    str=(char*)malloc(sizeof(char)*PCSL*STRLP);
    spcs=(char*)malloc(sizeof(char)*PCSL*3);
    
    {  //------------ get ptr --------------
        t_symbol *temp;
        PCS *tempcs;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
        CopiaSet(tempcs,x->pcs);                    //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------

    pcs_print_set(x->pcs, spcs);
    pcs_print_makestr(x->pcs,str);
    
    object_post((t_object*)x, "PCS: %s", spcs);
    post("%s", str);
    
    free(str);
    free(spcs);
    
    return;
}

void pcs_print_makestr(PCS* pcs, char *str){
    
    char *aux;
    
    aux=(char*)malloc(sizeof(char)*PCSL);
    
    *str=0;
    *aux=0;
    if(pcs->ncar >=1) {
        /*
        strcat(str, "PCS: ");
        Pset(pcs, aux);
        strcat(str,aux);
        */
        strcat(str, "Name: ");  // queda igual
        Pname(pcs,aux);
        strcat(str,aux);
        
        strcat(str, " · Prime form: ");     // reescrita
        pcs_print_pf(pcs,aux);
        strcat(str,aux);
        
        strcat(str, "· Status: ");          // reescrita
        pcs_print_stat(pcs,aux);
        strcat(str,aux);
        
        strcat(str, "· ICV: ");    // queda igual
        Picv(pcs,aux);
        strcat(str,aux);
    }
    else {
        strcat(str, "NULL SET");
    }
    free(aux);
    return; 
}

void pcs_print_set(PCS* pcs, char* set) {
    char elem[4];
    int i;
    
    *set=0;
    i=0;
    while (pcs->find[i] != EOC ) {
        if(pcs->find[i] == EOP) strcat(set, "· ");
        else {
            sprintf(elem, "%i ", pcs->find[i]);
            strcat(set, elem);
        }
        i++;
    }
    return;
}

void pcs_print_pf(PCS* pcs, char* pf) {
    char elem[4];
    int i;
    
    *pf=0;
    i=0;
    while (pcs->fprima[i] != EOC ) {
        sprintf(elem, "%i ", pcs->fprima[i]);
        strcat(pf, elem);
        i++;
    }
    return;
}

void pcs_print_stat(PCS* pcs, char* stat) {
    *stat=0;
    
    sprintf(stat,"T%d ",pcs->T);
    if(pcs->I !=0) {
        strcat(stat,"I ");
    }
    return;
}
