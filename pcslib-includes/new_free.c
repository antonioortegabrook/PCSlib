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
        pcs->pitch_content = NULL;
        pcs->consistent = false;    // mark as non consistent
        
        return pcs;
}


/*
        Deallocate a t_pcs
 */
void pcs_free(t_pcs *pcs)
{
        if (!pcs)
                return;

        free(pcs->pitch_content);
        pcs->pitch_content = NULL;

        free(pcs);
        pcs = NULL;

        return;
}

/*
        Copy the contents of a t_pcs into another. Target must be initialized.
        Warning: doesn't check for consistency
 */
int pcs_copy(t_pcs *target, t_pcs *source)
{
        int nelem, ncar;

        if (!source)
                return -1;
        if (!target)
                return -2;

        nelem = source->nelem;
        ncar = source->ncar;

        if (source->nelem) {
                free(target->pitch_content);
                target->pitch_content = malloc(nelem * sizeof(int));
                
                if (!target->pitch_content)
                        return -3;
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

        return 0;
}
