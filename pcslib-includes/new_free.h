/**     new_free.h by Antonio Ortega Brook - March 2017
 
 These are the methods for allocating and deallocatinc pcs and cm structs
 */

//#include "types.h"

/*
 Allocates a new empty t_pcs; if succesful, returns a pointer (null if not)
 */
t_pcs * pcs_new_empty();

/*
 Deallocate a t_pcs
 */
void pcs_free(t_pcs *pcs);

/*
 Copy the contents of a t_pcs into another. Target must be initialized
 */
void pcs_copy(t_pcs *source, t_pcs *target);
