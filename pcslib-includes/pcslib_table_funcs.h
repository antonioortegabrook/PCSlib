
/*
        Methods for getting data from the pcs_table (see tables.h)

        They're classified in three orders, based on input and return:
                1- get index from user input
                2- get index from index
                3- get data from index
 */

/***    1st Order (get index from user input)                            ***/

/** Returns the index in table of a PCS from its name
        @ params: cardinal number (int), ordinal number (int)
        @ returns: index (int)
 */
int name_table_index(int ncar, int nord);

/** Return the index in table of a PCS from its binary value and cardinal
        number
        @ params: binary value (int), cardinal number (int)
        @ returns: index (int) (-1 means not found)
 */
int binval_table_index(int binval, int ncar);


/***    2nd Order (get index from index)                                 ***/

/** Returns the index in table of a PCS's Z mate from its index (0 if it
        doesn't exists)
        @ params: index of PCS n (int)
        @ returns: index of n's Z mate or 0 (int)
 */
int z_table_index(int index);

/** Returns the index in table of the complement of a PCS from its index
        @ params: index of PCS n (int)
        @ returns: index of n's complement (int)
 */
int complement_table_index(int index);


/***    3rd Order (get data from index)                                 ***/

/** Returns the cardinal number of a PCS from its index
        @ params: index of PCS n (int)
        @ returns: n's cardinal number (int)
 */
int ncar_table(int index);

/** Returns the ordinal number of a PCS from its index
        @ params: index of PCS n (int)
        @ returns: n's ordinal number (int)
 */
int nord_table(int index);

/** Write to an array the prime form of a PCS from its index
        @ params: index in table of PCS (int), pointer to target array
        @ warning: doesn't check pointer; caller is responsible for that
 */
void pf_table(int index, int *pf_target);

//---
int n_table(int index);

/** Write to an array the interval vector of a PCS from its index
        @ params: index in table of PCS (int), pointer to target array
        @ warning: doesn't check pointer; caller is responsible for that
 */
void icv_table(int index, int *icv_target);
