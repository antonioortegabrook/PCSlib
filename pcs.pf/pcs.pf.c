/*  pcs_pf object by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
------- 

*/

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS

*/

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

#include "../pcslib-includes/pcslib_api.h"

#include "../pcslib-includes/pcslib_max_types.h"
#include "../pcslib-includes/pcslib_max_funcs.c"

typedef struct _pcs_pf {	// defines our object's internal variables for each instance in a patch
        t_object x_obj;			// object header - ALL objects MUST begin with this...
        
        t_pcs *pcs;             /*pointer to PCS struct*/
        
        void *pcs_out;
} t_pcs_pf;

// these are prototypes for the methods that are defined below
void pcs_pf_list(t_pcs_pf *x, t_symbol *s, long argc, t_atom *argv);  //-

void pcs_pf_assist(t_pcs_pf *x, void *b, long m, long a, char *s);
void pcs_pf_free(t_pcs_pf *x);
void *pcs_pf_new();

static t_class *pcs_pf_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
        t_class *c;
        
        c = class_new("pcs.pf", (method)pcs_pf_new, (method)pcs_pf_free, sizeof(t_pcs_pf), 0L, 0);
        
        class_addmethod(c, (method)pcs_pf_list,          "list",	A_LONG, 0);
        class_addmethod(c, (method)pcs_pf_assist,        "assist",	A_CANT, 0);	// assistance method
        
        class_register(CLASS_BOX, c);
        pcs_pf_class = c;
}

//--------------------------------------------------------------------------

void *pcs_pf_new()
{
        t_pcs_pf *x;                                             // local variable (pointer to a t_pcs_pf data structure)

        x = (t_pcs_pf *)object_alloc(pcs_pf_class);           // create a new instance of this object
        x->pcs_out = outlet_new(x, NULL);                                    // create a list outlet

        x->pcs = pcs_new_empty();

        return(x);					// return a reference to the object instance
}

//--------------------------------------------------------------------------

void pcs_pf_assist(t_pcs_pf *x, void *b, long m, long a, char *s) // 4 final arguments are always the same for the assistance method
{
        if (m == ASSIST_INLET)
                sprintf(s,"(list) List of Pitch Classes");
        if (m == ASSIST_OUTLET)
                sprintf(s,"PCS");
}

void pcs_pf_free(t_pcs_pf *x)
{
        pcs_free(x->pcs);
        return;
}

//--------------------------------------------------------------------------

void pcs_pf_list(t_pcs_pf *x, t_symbol *s, long argc, t_atom *argv)
{
        int *tmp_pitch_content;
        int tmp_nelem;
        
        tmp_pitch_content = malloc(argc * sizeof(int));
        
        if (!tmp_pitch_content) {
                object_error((t_object*)x, "allocation failed (tmp_pitch_content)");
                return;
        }
        
        int i;
        int j;
        
        
        // pickup a list with pcs
        /*
         Usamos una variable de incremento para leer el array de atoms y otra para escribir en el array find; esto es parte del typechecking y es necesario para que, si aparece un símbolo en el array de atoms, además de no copiarlo, la variable que usamos para escribir el array find no se incremente; de esta manera evitamos que luego EOC quede en un lugar incorrecto, lo cual derivaría en otros errores.
         */
        i=0;
        for (j = 0; j < argc; j++) {
                if (atom_gettype(argv+j) == A_LONG || atom_gettype(argv+j) == A_FLOAT) {
                        
                        tmp_pitch_content[i] = (int)atom_getfloat(&argv[j]);
                        
                        if(tmp_pitch_content[i] < 0 && tmp_pitch_content[i]!=EOP)
                                tmp_pitch_content[i] = abs(tmp_pitch_content[i]);
                        
                        if(tmp_pitch_content[i] >= 12)
                                tmp_pitch_content[i] = tmp_pitch_content[i] % 12; // take modulo-12
                        i++;
                }
        }

        tmp_nelem = i;
        
        int err = pcs_fill_from_pitch_content(x->pcs, tmp_pitch_content, tmp_nelem);
        
        if (err) {
                object_error((t_object*)x, "fill from pitch content failed"); // (revisar, deberíamos liberar tmp_pitch content)
                return;
        }

        {   //------------- out ptr -------------------
                t_patom ptr_out;
                short err_code;
                
                err_code = patom_setpcs(&ptr_out, x->pcs);
                if (err_code) {
                        object_error((t_object*)x, "error code %d", err_code);
                        return;
                }
                patom_setpcs(&ptr_out, x->pcs);
                outlet_anything (x->pcs_out, gensym(MPID), 1, (t_atom*)&ptr_out);    //- (cuidado con el nombre del outlet)
        }   //------------- end out -------------------

        /*
                Free temp data
         */
        free(tmp_pitch_content);

    return;
}
