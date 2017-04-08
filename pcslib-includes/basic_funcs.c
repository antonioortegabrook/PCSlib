
/*
 
 **
        These functions don't check arguments. Is caller's responsiblity to
        pass good data.
 **
 
 */


//#include "basic_funcs.h"


/**     Transpose a PCS. Puts results in input
                @ parameters: pointer to an array (int), # of elements
 */
void transpose(int *vector, int nelem, int tf)
{
        for (int i = 0; i < nelem; i++) {
                
                if (vector[i] != EOP)
                        vector[i] += tf;
                
                if (vector[i] < 0)
                        vector[i] += 12;
                
                else if (vector[i] >= 12)
                        vector[i] = vector[i] % 12;
                
        }
        
        return;
}


/**     Invert a PCS. Puts results in input
                @ parameters: pointer to an array (int), # of elements
 */
void invert(int *vector, int nelem)
{
        for (int i=0; i<nelem; i++) {
                
                if (vector[i] != EOP)
                        vector[i] = 12 - vector[i];
                
                if (vector[i] == 12)
                        vector[i] = 0;
                
        }
        
        return;
}

/**     Find the literal complement of a PCS
                @ parameters: pointer to source array (int), # of elements,
                        pointer to target array (int)
                @ warning: doesn't check for EOPs. Also it's better if pcs
                        is filtered
 */
void literal_complement(int *vector, int nelem, int *target)
{
        int is_present;
        int k = 0;

        for (int i = 0; i < 12; i++) {

                is_present = false;
                for (int j = 0; j < nelem; j++) {

                        if (vector[j] == i) {
                                is_present = true;
                                break;
                        }

                }
                if (!is_present) {
                        target[k] = i;
                        k++;
                }
        }

        return;
}
