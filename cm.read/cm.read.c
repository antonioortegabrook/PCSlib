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

typedef struct _cm_read {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    t_atom row_list[MAXELEM];
    CM *cm;                             // pointer to a CM struct
    void *row_out, *nr_out, *nc_out, *nexp_out;
} t_cm_read;

// these are prototypes for the methods that are defined below
void cm_read_cm_ptr(t_cm_read *x, t_symbol *s, long argc, t_atom *argv);

void cm_read_assist(t_cm_read *x, void *b, long m, long a, char *s);

void cm_read_free(t_cm_read *x);              //-
void *cm_read_new();

static t_class *cm_read_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("cm.read", (method)cm_read_new, (method)cm_read_free, sizeof(t_cm_read), 0L, 0);
    
    class_addmethod(c, (method)cm_read_cm_ptr,        MMID,		A_GIMME, 0);
    
	class_addmethod(c, (method)cm_read_assist,        "assist",	A_CANT,  0);	// (optional) assistance method

	class_register(CLASS_BOX, c);
	cm_read_class = c;
}

//--------------------------------------------------------------------------

void *cm_read_new() {
	t_cm_read *x;                                             // local variable (pointer to a t_cm_read data structure)
    
	x = (t_cm_read *)object_alloc(cm_read_class);           // create a new instance of this object
    
    x->nexp_out=intout(x);
    x->nc_out=intout(x);
    x->nr_out=intout(x);
    x->row_out=listout(x);
     
    x->cm=NULL;
    
    x->cm=(CM*)malloc(sizeof(CM));
    Inicializar(x->cm);;

	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void cm_read_assist(t_cm_read *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET)
        sprintf(s,"bang, CM");
        
    if (m == ASSIST_OUTLET) {
        switch(a) {
            case(0):
                sprintf(s,"Raw data of each of the rows of the CM");
                break;
            case(1):
                sprintf(s,"Number of rows of the CM");
                break;
            case(2):
                sprintf(s,"Number of columns of the CM");
                break;
            case(3):
                sprintf(s,"Maximal number of PCs in the positions of the CM");
                break;
        }
    }
}

void cm_read_free(t_cm_read *x){
    
    if(x->cm != NULL){
        free(x->cm);
    }
    return;
}

void cm_read_cm_ptr(t_cm_read *x, t_symbol *s, long argc, t_atom *argv) {
    int i,j;
    t_int tempf;
    t_atom *row_list;
    
    {  //------------ get ptr --------------
        t_symbol *temp;
        CM *tempcm;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcm);    // get the pointer to a PCS struct
        CopiaMatriz(tempcm,x->cm);                    //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------
    
    // output nr, nc and max elements X position first
    outlet_int(x->nexp_out, (x->cm->MaxElemxPos));
    outlet_int(x->nr_out, (x->cm->NroFilas));
    outlet_int(x->nc_out, (x->cm->NroCols));
    
    // output nc rows
    row_list=x->row_list;
    
    for(j = 0; j < x->cm->NroFilas; j++){
        for(i = 0; i < NEXTC * x->cm->NroCols; i++){
            tempf = x->cm->mat[i][j];
            atom_setlong(&(row_list[i]),tempf);
        }
        outlet_list(x->row_out, gensym("list"),i,x->row_list);
    }
    
    return;
}

