/**     new_free.h by Antonio Ortega Brook - March 2017
 
 These are the methods for allocating and deallocatinc pcs and cm structs
 */

#include<types.h>

/*
 Allocates a new empty t_pcs; if succesful, returns a pointer (null if not)
 */
t_pcs * pcs_new_empty();

/*
 Creates a new t_pcs from its name and t/i status
 */
t_pcs * pcs_new_from_name(int car, int ord, int tr, int inv);

/*
 Deallocate a t_pcs
 */
void pcs_free(t_pcs *pcs);
