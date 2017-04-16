/**     
        new_free.c by Antonio Ortega Brook - March 2017
 
        These are the methods for allocating, deallocating and copying pcs and cm structs
 */



/*
        Allocate a new empty t_pcs; returns a pointer (null if unsuccesful)
 */
t_pcs * pcs_new_empty()
{
        t_pcs *pcs;
        
        pcs = malloc(sizeof(t_pcs));
        
        if (!pcs)
                return NULL;
        
        pcs->nelem = 0;
        pcs->delivered = NULL;
        pcs->npitches = 0;
        pcs->pitch_content = NULL;
        pcs->consistent = false;    // mark as non consistent
        
        return pcs;
}


/*
        Copy the contents of a t_pcs into a new one; return null if allocation
        fails
 */
t_pcs * pcs_copy(t_pcs *source)
{
        t_pcs *new_pcs;
        int tmp_nelem = 0;


        if (!source)
                return NULL;

        new_pcs = pcs_new_empty();

        if (!new_pcs)
                return NULL;

        tmp_nelem = source->nelem;

        if (source->pitch_content) {
                new_pcs->pitch_content = malloc(tmp_nelem * sizeof(int));

                if (!new_pcs->pitch_content)
                        return NULL;
        }


        /** 
                Now that everything went well, we copy the contents of source
                into new_pcs
         */
        new_pcs->consistent = source->consistent;
        new_pcs->ncar = source->ncar;
        new_pcs->nord = source->nord;

        for (int i = 0; i < tmp_nelem; i++)
                new_pcs->pitch_content[i] = source->pitch_content[i];

        new_pcs->nelem = source->nelem;

        for (int i = 0; i < AGL; i++)
                new_pcs->prime_form[i] = source->prime_form[i];

        new_pcs->t = source->t;
        new_pcs->inverted = source->inverted;

        for (int i = 0; i < ICVL; i++)
                new_pcs->icv[i] = source->icv[i];

        new_pcs->selected = source->selected;
        new_pcs->table_index = source->table_index;

        return new_pcs;
}


/*
        Deallocate a t_pcs
 */
void pcs_free(t_pcs *pcs)
{
        if (!pcs)
                return;

        free(pcs->delivered);
        pcs->delivered = NULL;

        free(pcs->pitch_content);
        pcs->pitch_content = NULL;

        free(pcs);
        pcs = NULL;

        return;
}


/*
        Copy the contents of a t_pcs into another. Target must be initialized.
                @ warning: doesn't check for consistency. Doesn't check pointers
                        (caller is responsible for that)
        ---DEPRECATED---
 */
void pcs_copy_old(t_pcs *target, t_pcs *source)
{
        int nelem, ncar;

        nelem = source->nelem;
        ncar = source->ncar;
        
        

        if (source->nelem) {                    // revisar esto
                free(target->pitch_content);
                target->pitch_content = malloc(nelem * sizeof(int));
                
                if (!target->pitch_content)
                        return;
        }

        for (int i = 0; i < nelem; i++)
                target->pitch_content[i] = source->pitch_content[i];

        target->consistent = source->consistent;
        target->ncar = source->ncar;
        target->nord = source->nord;
        target->nelem = source->nelem;

        for (int i = 0; i < ncar; i++)
                target->prime_form[i] = source->prime_form[i];

        target->t = source->t;
        target->inverted = source->inverted;

        for (int i = 0; i < 6; i++)
                target->icv[i] = source->icv[i];

        target->selected = source->selected;
        target->table_index = source->table_index;

        return;
}
