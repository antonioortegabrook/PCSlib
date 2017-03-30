/*
    Methods for getting data from the tables (see tables.h)
 */

//#include "pcslib_table_funcs.h"



/** Return the index in table of a PCS from its binary value and cardinal number
        @ params: binary value (int), cardinal number (int)
        @ returns: index (int)
 */
int binval_table_index(int binval, int ncar)
{
        if (ncar < 3 || ncar > 9)
                return -1;
        
        int index = -1;
        int lower_limit = bv_idx[ncar];
        int upper_limit = lower_limit + max_ord[ncar];
        
        for (int i = lower_limit; i < upper_limit; i++) {
                if (bin_vals[i] == binval) {
                        index = i * 20;
                        break;
                }
        }
        
        if (index < 0)
                return -2;
        
        return index;
}

/** Returns the index in table of a PCS from its name
        @ params: cardinal number (int), ordinal number (int)
        @ returns: index (int)
 */
int name_table_index(int ncar, int nord)
{
        int index;
        int tmp_a, tmp_b;

        if (ncar < 3 || ncar > 9)
                return -1;              // invalid ncar
        
        if (nord < 1 || nord > max_ord[ncar])
                return -2;              // invalid nord
    
        tmp_a = car_pos[ncar];          // index of pcs ncar-1
        tmp_b = 20 * (nord - 1);        // offset from ncar-1
        index = tmp_a + tmp_b;

        return index;
}

/** Returns the index in table of a PCS's Z mate from its index (0 if doesn't exists)
        @ params: index of PCS n (int)
        @ returns: index of n's Z mate or 0 (int)
 */
int z_table_index(int index)
{
        int z;

        z = pcs_table[index+11];     // 11 is the offset at which z index is located
        
        if (z)
                z = z - 1;          // z indexes have this 1 offset (why...?)

        return z;                   // it will return 0 (false) if no Z
}

/** Returns the index in table of the complement of a PCS from its index
        @ params: index of PCS n (int)
        @ returns: index of n's complement (int)
 */
int complement_table_index(int index)
{
        int complement;

        complement = pcs_table[index + 19];
        
        complement = complement - 1;    // complement indexes have this 1 offset (why...?)

        return complement;
}

/** Returns the cardinal number of a PCS from its index
        @ params: index of PCS n (int)
        @ returns: n's cardinal number (int)
 */
int ncar_table(int index)
{
        int ncar;

        ncar = pcs_table[index];

        return ncar;
}

/** Returns the ordinal number of a PCS from its index
        @ params: index of PCS n (int)
        @ returns: n's ordinal number (int)
 */
int nord_table(int index)
{
        int nord;
        
        nord = pcs_table[index + 1];
        
        return nord;
}

/** Returns the prime form of a PCS from its index
        @ params: index in table of PCS (int), pointer to target array
        @ returns: -1 if NULL pointer received, 0 if success
 */
int pf_table(int index, int *pf_target)
{
        if (!pf_target)
                return -1;
        
        int ncar = pcs_table[index];
        int j = index + 2;

        for(int i = 0; i < ncar; i++)
                pf_target[i] = pcs_table[j + i];

        return 0;
}

//---
int n_table(int index)
{
    int n;
    n = pcs_table[index+12];
    return n;
}

/** Returns the interval vector of a PCS from its index
        @ params: index in table of PCS (int), pointer to target array
        @ returns: -1 if NULL pointer received, 0 if success
 */
int icv_table(int index, int *icv_target)
{
        if (!icv_target)
                return -1;
        
        int j = index+13;
        
        for(int i = 0; i < 6; i++)
                icv_target[i] = pcs_table[j + i];
        
        return 0;
}

