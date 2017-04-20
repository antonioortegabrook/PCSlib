/**
        Methods for getting data from the tables (see tables.h)
 
 **
        These functions don't check arguments. Is caller's responsiblity to
        pass good data.
 **
 */

//#include "pcslib_table_funcs.h"
#include "new_tabs.h"


/** Return the index in table of a PCS from its binary value and cardinal number
        @ params: binary value (int), cardinal number (int)
        @ returns: index (int) (-1 means not found)
 */
int binval_table_index(int binval, int ncar)
{
        int index = -1;
        int lower_limit = bv_idx[ncar];
        int upper_limit = lower_limit + max_ord[ncar];

        for (int i = lower_limit; i < upper_limit; i++) {
                if (bin_vals[i] == binval) {
                        index = PCS_TABLE_ROW_LEN * i;
                        break;
                }
        }

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

        tmp_a = car_pos[ncar];                          // index of pcs ncar-1
        tmp_b = PCS_TABLE_ROW_LEN * (nord - 1);         // offset from ncar-1
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

        z = pcs_table[index + Z_OFFSET];     // 11 is the offset at which z index is located

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

        complement = pcs_table[index + COMPLEMENT_OFFSET];

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

        ncar = pcs_table[index + NCAR_OFFSET];

        return ncar;
}


/** Returns the ordinal number of a PCS from its index
        @ params: index of PCS n (int)
        @ returns: n's ordinal number (int)
 */
int nord_table(int index)
{
        int nord;

        nord = pcs_table[index + NORD_OFFSET];

        return nord;
}


/** Write to an array the prime form of a PCS from its index
        @ params: index in table of PCS (int), pointer to target array
        @ warning: doesn't check pointer; caller is responsible for that
 */
void pf_table(int index, int *pf_target)
{
        int ncar = pcs_table[index + NCAR_OFFSET];
        int j = index + PF_OFFSET;

        for(int i = 0; i < ncar; i++)
                pf_target[i] = pcs_table[j + i];

        return;
}


//---
int n_table(int index)
{
        int n;

        n = pcs_table[index + N_OFFSET];

        return n;
}


/** Write to an array the interval vector of a PCS from its index
        @ params: index in table of PCS (int), pointer to target array
        @ warning: doesn't check pointer; caller is responsible for that
 */
void icv_table(int index, int *icv_target)
{
        int j = index + ICV_OFFSET;

        for(int i = 0; i < 6; i++)
                icv_target[i] = pcs_table[j + i];

        return;
}

