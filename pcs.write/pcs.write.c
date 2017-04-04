/*
        pcs.write by Antonio Ortega Brook        - April 2017
 
        Requires Max6/7 & OS X 10.7 or later
 
 
        @ input: list of two to four args:
                1 - Cardinal number (int)
                2 - Ordinal number (int)
                3 - Transposition factor (int) (optional)
                3/4 - Inversion status (symbol)
 
        @ output pointer to a t_pcs structure (t_ptr_mess)

 */

/** THIS IS EXPERIMENTAL **
 
 Usamos t_ptr_mess para enviar/recibir punteros, en vez de t_atom
 */

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.


#include "../pcslib-includes/pcslib_api.h"

#include "../pcslib-includes/pcslib_max_types.h"
#include "../pcslib-includes/pcslib_max_funcs.c"



typedef struct _pcs_write {	// defines our object's internal variables for each instance in a patch
        t_object x_obj;			// object header - ALL objects MUST begin with this...
        
        t_pcs *pcs;   /*pointer to PCS struct*/
        
        void *pcs_out;
} t_pcs_write;



// these are prototypes for the methods that are defined below
void pcs_write_list(t_pcs_write *x, t_symbol *s, long argc, t_atom *argv);

void pcs_write_assist(t_pcs_write *x, void *b, long m, long a, char *s);

void pcs_write_free(t_pcs_write *x);
void *pcs_write_new();

static t_class *pcs_write_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
        t_class *c;
        
        c = class_new("pcs.write", (method)pcs_write_new, (method)pcs_write_free, sizeof(t_pcs_write), 0L, 0);
        
        class_addmethod(c, (method)pcs_write_list,          "list",             A_GIMME,        0);
        
        class_addmethod(c, (method)pcs_write_assist,        "assist",           A_CANT,         0); // assistance method
        
        class_register(CLASS_BOX, c);
        pcs_write_class = c;
}

//--------------------------------------------------------------------------

void *pcs_write_new()
{
        t_pcs_write *x;             // local variable (pointer to a t_pcs_write data structure)
        
        x = (t_pcs_write *)object_alloc(pcs_write_class);           // create a new instance of this object
        x->pcs_out = outlet_new(x, MPID);                                    // create a list outlet

        x->pcs = pcs_new_empty();

        return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

/*
        Assistance method
 */

void pcs_write_assist(t_pcs_write *x, void *b, long m, long a, char *s) // 4 final arguments are always the same for the assistance method
{
        if (m == ASSIST_INLET)
                sprintf(s,"(list) Ordinal (int), Cardinal (int), Transposition factor (int), I (symbol, optional)");
        
        if (m == ASSIST_OUTLET)
                sprintf(s,"PCS");
}

/*
        Free routine
 */
void pcs_write_free(t_pcs_write *x)
{
        pcs_free(x->pcs);
        return;
}

void pcs_write_list(t_pcs_write *x, t_symbol *s, long argc, t_atom *argv)
{
        t_patom ptr_out;
        short err_code;

        int ncar;
        int nord;
        int transp = 0;
        int inv = false;
        
        /* Arguments must be 2, 3 or 4
         */
        if(argc < 2 || argc > 4) {
                object_error((t_object*)x, "too many or too few arguments...");
                return;
        }
        
        /** Check type for the first two args; must be long or float
         */
        for (int i = 0; i < 2; i++) {
                if (atom_gettype(argv + i) != A_LONG && atom_gettype(argv + i) != A_FLOAT) {
                        
                        object_error((t_object*)x, "wrong message");
                        
                        return;
                }
        }
        
        ncar = (int)atom_getlong(argv);
        nord = (int)atom_getlong(argv + 1);
        
        /* Check if given name is valid
         */
        if (not_exist(ncar, nord)) {
                object_error((t_object*)x, "sc %d-%d does not exist", ncar, nord);
                return;
        }
        
        /** Check if there are more arguments in the list
         */
        if (argc > 2) {
                for (int i = 2; i < argc && i < 4; i++) {
                        
                        /* If thrid arg is symbol "i", we assume no fourth argument, so we
                         break the loop
                         */
                        
                        // esto debería ser "i" o zero/non-zero. Es una cuestión de consistencia con pcs.read
                        if (atom_gettype(argv + i) == A_SYM) {
                                if (atom_getsym(argv + i) == gensym("I") || atom_getsym(argv + i) == gensym("i")) {
                                        inv = true;
                                        break;
                                } else {
                                        object_error((t_object*)x, "bad arguments...");
                                        return;
                                }
                        }
                        
                        /* If third arg is not a symbol, check for long or float
                         */
                        if (atom_gettype(argv + i) == A_LONG || atom_gettype(argv + i) == A_FLOAT) {
                                
                                /* If i = 3, arg MUST be symbol "i"
                                 */
                                if (i < 3) {
                                        transp = (int)atom_getlong(argv + i);
                                } else {
                                        object_error((t_object*)x, "bad arguments...");
                                        return;
                                }
                        } else {
                                /* Not a symbol or a number
                                 */
                                object_error((t_object*)x, "bad arguments...");
                                return;
                        }
                }
        }


        transp = transp % 12;

        /* Fill t_pcs from its name and t/i status
         */
        int err_f = pcs_fill_from_name(x->pcs, ncar, nord, transp, inv);
        
        if (err_f) {
                object_error((t_object*)x, "pcs_fill failed whith error code %d", err_f);
                return;
        }
        
        
        /**
                Send pointer out
         */
        err_code = patom_setpcs(&ptr_out, x->pcs);
        
        if (err_code) {
                object_error((t_object*)x, "error code %d", err_code);
                return;
        }
        
        patom_setpcs(&ptr_out, x->pcs);
        outlet_anything (x->pcs_out, gensym(MPID), 1, (t_atom*)&ptr_out);


        return;
}
