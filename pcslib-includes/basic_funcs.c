
//#include "basic_funcs.h"


/**     Transpose a PCS. Puts results in input
                @ parameters: pointer to an array (int), # of elements
                @ returns: error code -1 if array is null
 */
int transpose(int *vector, int nelem, int tf)
{
        if (!vector)
                return -1;
        
        for (int i = 0; i < nelem; i++) {
                
                if (vector[i] != EOP)
                        vector[i] += tf;
                
                if (vector[i] < 0)
                        vector[i] += 12;
                
                else if (vector[i] >= 12)
                        vector[i] = vector[i] % 12;
                
        }
        
        return 0;
}


/**     Invert a PCS. Puts results in input
                @ parameters: pointer to an array (int), # of elements
                @ returns: error code -1 if array is null
 */
int invert(int *vector, int nelem)
{
        if (!vector)
                return -1;
        
        for (int i=0; i<nelem; i++) {
                
                if (vector[i] != EOP)
                        vector[i] = 12 - vector[i];
                
                if (vector[i] == 12)
                        vector[i] = 0;
                
        }
        
        return 0;
}


int literal_complement(int *vector, int nelem, int *target)
{
        int is_present;
        int k = 0;
        
        if (!vector)
                return -1;
        if (!target)
                return -2;


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
        
        return 0;
}
