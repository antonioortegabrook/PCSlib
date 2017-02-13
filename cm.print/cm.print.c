/*                                              Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
-------
*/

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS
 
        Borrar variables no utilizadas...?  /
*/

#include "ext.h"			//      Max headers
#include "ext_obex.h"		//

#include "../pcslib-includes/pcslib_max.c"       //    PCSlib header

typedef struct _cm_print {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    CM *cm;   // pointer to a CM struct
} t_cm_print;

// these are prototypes for the methods that are defined below
void cm_print_cm_ptr(t_cm_print *x, t_symbol *s, long argc, t_atom *argv);

void cm_print_assist(t_cm_print *x, void *b, long m, long a, char *s);

void cm_print_free(t_cm_print *x);              //-
void *cm_print_new();

static t_class *cm_print_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("cm.print", (method)cm_print_new, (method)cm_print_free, sizeof(t_cm_print), 0L, 0);
    
    class_addmethod(c, (method)cm_print_cm_ptr,        MMID,		A_GIMME, 0);
	class_addmethod(c, (method)cm_print_assist,        "assist",	A_CANT,  0);	// (optional) assistance method

	class_register(CLASS_BOX, c);
	cm_print_class = c;
}

//--------------------------------------------------------------------------

void *cm_print_new() {
	t_cm_print *x;                                             // local variable (pointer to a t_cm_print data structure)
    
	x = (t_cm_print *)object_alloc(cm_print_class);           // create a new instance of this object
    
    x->cm=NULL;
    
    x->cm=(CM*)malloc(sizeof(CM));
    Inicializar(x->cm);;

	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void cm_print_assist(t_cm_print *x, void *b, long m, long a, char *s) {
	if (m == ASSIST_INLET)
		sprintf(s,"CM");
}

void cm_print_free(t_cm_print *x){
    
    if(x->cm != NULL){
        free(x->cm);
    }
    return;
}

void cm_print_cm_ptr(t_cm_print *x, t_symbol *s, long argc, t_atom *argv) {
    
    char *row;
    char *elem;
    char *pos;
    char *spa;
    char se[] = " ·  ";
    
    {  //------------ get ptr --------------
        t_symbol *temp;
        CM *tempcm;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcm);    // get the pointer to a PCS struct
        CopiaMatriz(tempcm,x->cm);                    //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------

    // 1- buscar números de dos cifras en las posiciones
    int chcount;			//	cantidad total de caracteres por posición
    int ddcount=0, tcount;		//	cantidad de números de dos dígitos
    int npos;
    int ddpos;
    int pelemcount;
    
    int elemIndex;			//	índice del elemento en la posición
    
    for(int fila=0; fila < x->cm->NroFilas; fila++) {
        for(int columna=0; columna < x->cm->NroCols; columna++) {
            elemIndex=0;
            tcount=0;
            while(x->cm->mat[columna*NEXTC+elemIndex][fila] != SPAC) {
                if(x->cm->mat[columna*NEXTC+elemIndex][fila] > 9) {
                    tcount++;
                    if(tcount > ddcount) {
                        ddcount++;
                    }
                }
                elemIndex++;
            }
        }
    }
    chcount = x->cm->MaxElemxPos*3+ddcount*2;
    
    // 2- print the CM on the PD prompt
    row=(char*)malloc(sizeof(char)*(MAXELEM*2+1));
    *row=0;
    elem=(char*)malloc(sizeof(char)*4);
    *elem=0;
    pos=(char*)malloc(sizeof(char)*(NEXTC*4));
    *pos=0;
    spa=(char*)malloc(sizeof(char)*4);
    *spa=0;
    
    object_post((t_object*)x, "CM · %i x %i · Max elems per position: %i", x->cm->NroFilas, x->cm->NroCols, x->cm->MaxElemxPos);
    for(int fila=0; fila < x->cm->NroFilas; fila++) {
        *row=0;
        for(int columna=0; columna < x->cm->NroCols; columna++) {
            *pos=0;
            elemIndex=0;
            npos=0;
            ddpos=0;
            while(x->cm->mat[columna*NEXTC+elemIndex][fila] != SPAC && elemIndex < x->cm->MaxElemxPos) {
                if(x->cm->mat[columna*NEXTC+elemIndex][fila] > 9) {
                    ddpos++;
                }
                npos++;
                *elem=0;
                sprintf(elem, "%i ", x->cm->mat[columna*NEXTC+elemIndex][fila]);
                strcat(pos, elem);
                elemIndex++;
            }
            pelemcount=npos*3+ddpos*2;
            for(long i=pelemcount; i < chcount; i++) {
                *spa=0;
                sprintf(spa, " ");
                strcat(pos, spa);
            }
            
            strcat(row, pos);
            if(columna < x->cm->NroCols-1) {
                strcat(row, se);
            }
        }
        post(row);
    }
    free(row);
    free(elem);
    free(pos);
    free(spa);
    return;
}
