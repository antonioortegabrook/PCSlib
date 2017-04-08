
/*
 
 **
        These functions don't check arguments. Is caller's responsiblity to
        pass good data.
 **
 
 */


/**     Transpose a PCS. Puts results in input
                @ parameters: pointer to an array (int), # of elements
 */
void transpose(int *vector, int nelem, int tf);


/**     Invert a PCS. Puts results in input
                @ parameters: pointer to an array (int), # of elements
 */
void invert(int *vector, int nelem);


/**     Find the literal complement of a PCS
                @ parameters: pointer to source array (int), # of elements,
                        pointer to target array (int)
 */
void literal_complement(int *vector, int nelem, int *target);
