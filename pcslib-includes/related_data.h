
/*      Get individual data from related PCSs
 */

/*      Get literal complement
                @ params: pointer to t_pcs, pointer to target array
                @ return: error code if NULL pointers
                @ warning: requires target of index 12 or complement's cardinal
 */
int pcs_literal_complement(t_pcs *pcs, int *target);

/*      Get complement's prime form
                @ params: pointer to t_pcs, pointer to target array
                @ return: error code if NULL pointers
 */
int pcs_complement_pf(t_pcs *pcs, int *target);
