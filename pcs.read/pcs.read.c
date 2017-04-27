/*
        pcs.read by Antonio Ortega Brook        - April 2017

                Requires Max6/7 & OS X 10.7 or later

 
        @ input: pointer to a t_pcs structure (t_ptr_mess)

        @ output (from left to right):
                - outlet 0: pcs as delivered (list)
                - outlet 1: cardinal number (int)
                - outlet 2: ordinal number (int)
                - outlet 3: T/I status (list)
                - outlet 4: prime form (list)
                - outlet 5: interval class vector (list)
                - outlet 6: literal complement (list)
*/

/** THIS IS EXPERIMENTAL **
 
    Usamos t_ptr_mess para enviar/recibir punteros, en vez de t_atom
 */

#include "ext.h"		// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

#include "../pcslib-includes/pcslib_api.h"

#include "../pcslib-includes/pcslib_max_types.h"
#include "../pcslib-includes/pcslib_max_funcs.c"


typedef struct _pcs_read {	// defines our object's internal variables for each instance in a patch
        t_object x_obj;			// object header - ALL objects MUST begin with this...
        
        t_pcs *pcs;		  /*pointer to PCS struct*/
        
        void *nc_out, *no_out, *pf_out, *iv_out, *ti_out, *fi_out, *co_out;
} t_pcs_read;

// these are prototypes for the methods that are defined below
void pcs_read_pcs_ptr_mes(t_pcs_read *x, t_symbol *s, long argc, t_patom *argv);

void pcs_read_assist(t_pcs_read *x, void *b, long m, long a, char *s);
void pcs_read_free(t_pcs_read *x);
void *pcs_read_new();

static t_class *pcs_read_class;		// global pointer to the object class - so max can reference the object

//--------------------------------------------------------------------------

void ext_main(void *r)
{
        t_class *c;
        
        c = class_new("pcs.read", (method)pcs_read_new, (method)pcs_read_free, sizeof(t_pcs_read), 0L, 0);
        
        class_addmethod(c, (method)pcs_read_pcs_ptr_mes,	MPID,           A_GIMME,  0);
        class_addmethod(c, (method)pcs_read_assist,             "assist",	A_CANT,   0);	//assistance method
        
        class_register(CLASS_BOX, c);
        pcs_read_class = c;
}

//--------------------------------------------------------------------------

void *pcs_read_new() {
        t_pcs_read *x;                                 // local variable (pointer to a t_pcs_read data structure)

        x = (t_pcs_read *)object_alloc(pcs_read_class);           // create a new instance of this object

        x->co_out = listout(x);                                 // create outlets
        x->iv_out = listout(x);
        x->pf_out = listout(x);
        x->ti_out = listout(x);
        x->no_out = intout(x);
        x->nc_out = intout(x);
        x->fi_out = listout(x);

        return(x);					// return a reference to the object instance
}
//--------------------------------------------------------------------------

/*
        Assistance method
 */
void pcs_read_assist(t_pcs_read *x, void *b, long m, long a, char *s)
{
        if (m == ASSIST_INLET)
                sprintf(s,"PCS");
        if (m == ASSIST_OUTLET) {
                switch(a) {
                        case(0):
                                sprintf(s,"Original PCS (list)");
                                break;
                        case(1):
                                sprintf(s,"Cardinal number (int)");
                                break;
                        case(2):
                                sprintf(s,"Ordinal number (int)");
                                break;
                        case(3):
                                sprintf(s,"T/I status (list)");
                                break;
                        case(4):
                                sprintf(s,"Prime form (list)");
                                break;
                        case(5):
                                sprintf(s,"ICV (list)");
                                break;
                        case(6):
                                sprintf(s,"Literal complement (list)");
                                break;
                }
        }
}

/*
        Free routine
 */
void pcs_read_free(t_pcs_read *x)
{
        pcs_free(x->pcs);
        return;
}
//--------------------------------------------------------------------------

void pcs_read_pcs_ptr_mes(t_pcs_read *x, t_symbol *s, long argc, t_patom *argv)
{
        /*
                Temp data
         */
        t_pcs *tempcs;
        int nelem;
        int comp_ncard;
        int literal_complement[12];

        /*
                Data to be sent through outlets
         */
        t_int  nord;                            // Ordinal number
        t_int  ncar;                            // Cardinal number
        t_atom *pitch_content_list;             // Individual form (may have pc repeated)
        t_atom prime_form_list[12];             // Prime form
        t_atom ti_list[2];                      // T/I status
        t_atom icv_list[6];                     // Interval class vector
        t_atom complement_list[12];             // Complement list


        /*
                Get pointer to struct
         */
        tempcs = patom_getpcs(argv);

        if (!tempcs) {
                object_error((t_object*)x, "NULL pointer received");
                return;
        }

        x->pcs = pcs_copy(tempcs);              // create a local copy of the structure

        if (!x->pcs) {
                object_error((t_object*)x, "allocation failed (pcs_copy)");
                return;
        }

        /*
                Check for consistency
         */
        if (!x->pcs->consistent) {
                object_warn((t_object*)x, "pcs data inconsistent");
        }


        /*
                Read data from t_pcs
         */
        nelem      = x->pcs->nelem;
        comp_ncard = 12 - x->pcs->ncar;
        
        nord = x->pcs->nord;
        ncar = x->pcs->ncar;
        
        pitch_content_list = malloc(nelem * sizeof(t_atom));
        
        if (!pitch_content_list) {
                object_error((t_object*)x, "allocation failed (pitch_content_list)");
                return;
        }
        
        for (int i = 0; i < nelem; i++)
                atom_setlong(&pitch_content_list[i], (long)x->pcs->pitch_content[i]);
        
        for (int i = 0; i < ncar; i++)
                atom_setlong(&prime_form_list[i], (long)x->pcs->prime_form[i]);
        
        atom_setlong(&ti_list[0], x->pcs->t);
        atom_setlong(&ti_list[1], x->pcs->inverted);
        
        for (int i = 0; i < 6; i++)
                atom_setlong(&icv_list[i], (long)x->pcs->icv[i]);
        
        /*
                Find complement
         */
         // replace by literal_complement in basic_funcs
        int err_c = pcs_literal_complement(x->pcs, literal_complement);
        
        if (err_c) {
                object_error((t_object*)x, "find complement failed");
                return;
        }
        
        for (int i = 0; i < comp_ncard; i++)
                atom_setlong(&complement_list[i], (long)literal_complement[i]);

        /*
                Output data
         */
        outlet_list (x->co_out, gensym("list"), comp_ncard, complement_list);       //(complement)
        outlet_list (x->iv_out, gensym("list"), 6, icv_list);
        outlet_list (x->pf_out, gensym("list"), ncar, prime_form_list);
        outlet_list (x->ti_out, gensym("list"), 2, ti_list);
        outlet_int  (x->no_out, nord);
        outlet_int  (x->nc_out, ncar);
        outlet_list (x->fi_out, gensym("list"), nelem, pitch_content_list);
        
        return;
}
