/*  pcs_write object by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
------- 
 The pcs_write object expects a list with three or four args.
 cardinal number(float), ordinal number(float) and transposition factor(float).
 If a fourth arg is delivered, it must be a symbol "I", which stands for
 "inversion". The PCS is scanned at the PCS table and transposed and/or inverted
 if required.
 The output is a pointer to a PCS struct. To access the data, a pcs_read
 object must be used.
*/

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS

 //- bug! Crash cuando ncar==3 && nord==0
                       ncar==4 && nord==-12 Puede estar relacionado con pcs_funcs.c
                       ncar==5 && nord==-41
                       ncar==6 && nord==-79
                       ncar==7 && nord==-129
                       ncar==8 && nord==-167
                       ncar==9 && nord==-196
 
*/
/** EXPERIMENTAL **
 Usamos t_ptr_mess para enviar/recibir punteros, en vez de t_atom
 */

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

//#include "../pcslib-includes/pcslib_max.c"       //    PCSlib header

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
        
        x->pcs = NULL;

        return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void pcs_write_assist(t_pcs_write *x, void *b, long m, long a, char *s) // 4 final arguments are always the same for the assistance method
{
        if (m == ASSIST_INLET)
                sprintf(s,"(list) Ordinal (int), Cardinal (int), Transposition factor (int), I (symbol, optional)");
        
        if (m == ASSIST_OUTLET)
                sprintf(s,"PCS");
}

void pcs_write_free(t_pcs_write *x)
{
        /* pcs_free alredy checks for null pointer
         */
        pcs_free(x->pcs);
        
        return;
}

void pcs_write_list(t_pcs_write *x, t_symbol *s, long argc, t_atom *argv)
{
        
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
        if (!exist(ncar, nord)) {
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
        
        /* if there is a pcs allocated, free it
         */
        if (x->pcs)
                pcs_free(x->pcs);
        
        /* Create a new t_pcs from its name and t/i status
         */
        x->pcs = pcs_new_from_name(ncar, nord, transp, inv);
        
        
        
        
        {   //------------- out ptr -------------------
                t_ptr_mess ptr_out;
                short err_code;
                
                err_code = ptr_mess_setpcs(&ptr_out, x->pcs);
                if (err_code) {
                        object_error((t_object*)x, "error code %d", err_code);
                        return;
                }
                ptr_mess_setpcs(&ptr_out, x->pcs);
                outlet_anything (x->pcs_out, gensym(MPID), 1, (t_atom*)&ptr_out);    //- (cuidado con el nombre del outlet)
        }   //------------- end out -------------------
        return;		
}
