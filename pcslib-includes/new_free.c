/**     new_free.c by Antonio Ortega Brook - March 2017
 
        These are the methods for allocating and deallocatinc pcs and cm structs
 */


//#include "new_free.h"



/*
        Allocates a new empty t_pcs; returns a pointer (null if unsuccesful)
 */
t_pcs * pcs_new_empty()
{
        t_pcs *pcs = NULL;
        
        pcs = malloc(sizeof(t_pcs));
        if (!pcs)
                return NULL;
        
        pcs->pitch_content = NULL;
        pcs->consistent = false;    // mark as non consistent
        
        return pcs;
}

/*
        Creates a new t_pcs from its name and t/i status; returns a pointer
        (null if unsuccesful)
 */
t_pcs * pcs_new_from_name(int car, int ord, int tr, int inv)
{
        t_pcs *pcs = pcs_new_empty();
        
        if (!pcs)
                return NULL;
        
        pcs->pitch_content = malloc(car * sizeof(int) + sizeof(int));
        
        if (!pcs->pitch_content) {
                free(pcs);
                pcs = NULL;
                return NULL;
        }
        
        int index = name_table_index(car, ord);
        int tmp_pitch_content[13];
        int tmp_prime_form[12];
        int tmp_icv[6];
        int err = 0;
        /*
         Set temp data
         */
        err += pf_table(index, tmp_pitch_content);
        
        err += pf_table(index, tmp_prime_form);
        
        err += icv_table(index, tmp_icv);
        
        if (tr)                         // transpose...?
                err += transpose(tmp_pitch_content, car, tr);
        
        if (inv)                        // invert...?
                err += invert(tmp_pitch_content, car);
        
        /*
         Copy temp data to struct
         */
        pcs->ncar = car;
        pcs->nord = ord;
        
        for (int i = 0; i < car; i++)
                pcs->pitch_content[i] = tmp_pitch_content[i];
        pcs->pitch_content[car] = EOC;
        
        pcs->nelem = car + 1;
        
        for (int i = 0; i < car; i++)
                pcs->prime_form[i] = tmp_prime_form[i];
        
        pcs->t = tr;
        pcs->inverted = inv;
        
        for (int i = 0; i < 6; i++)
                pcs->icv[i] = tmp_icv[i];
        
        pcs->selected = false;
        pcs->table_index = index;
        
        /*
         Check for errors. If so, free everything and return NULL
         */
        if (err) {
                pcs_free(pcs);
                
                return NULL;
        }
        
        pcs->consistent = true;
        
        return pcs;
}

/*
 Deallocate a t_pcs
 */
void pcs_free(t_pcs *pcs)
{
        if (!pcs)
                return;
        
        if (pcs->pitch_content) {
                free(pcs->pitch_content);
                pcs->pitch_content = NULL;
        }
        
        free(pcs);
        pcs = NULL;
        
        return;
}
