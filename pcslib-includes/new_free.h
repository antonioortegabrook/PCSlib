/**     new_free.h by Antonio Ortega Brook - March 2017
 
 These are the methods for allocating and deallocatinc pcs and cm structs
 */

//#include "types.h"


/*
        Allocate a new empty t_pcs; returns null if unsuccesful
 */
t_pcs * pcs_new_empty();

/*
        Copy the contents of a t_pcs into a new one; returns null if unsuccesful
 */
t_pcs * pcs_copy(t_pcs *source);

/*
        Deallocate a t_pcs
 */
void pcs_free(t_pcs *pcs);

/*
        Copy the contents of a t_pcs into another. Target must be initialized
        ---DEPRECATED---
 */
void pcs_copy_old(t_pcs *source, t_pcs *target);
