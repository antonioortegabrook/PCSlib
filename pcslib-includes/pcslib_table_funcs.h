/*
 Methods for getting data from the tables (see tables.h)
 */

//--- 1st Order (get index from user input)
int name_table_index(int ncar, int nord);
int binval_table_index(int binval, int ncar);

//--- 2nd Order (get index from index)
int z_table_index(int index);
int complement_table_index(int index);

//--- 3rd Order (get data from index)
int ncar_table(int index);
int nord_table(int index);
void pf_table(int index, int *pf_target);
int n_table(int index);
void icv_table(int index, int *icv_target);
