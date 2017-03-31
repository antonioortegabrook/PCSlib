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
        Deallocate a t_pcs
 */
void pcs_free(t_pcs *pcs)
{
        if (!pcs)
                return;
        
        free(pcs->pitch_content);
        
        free(pcs);
        
        return;
}
