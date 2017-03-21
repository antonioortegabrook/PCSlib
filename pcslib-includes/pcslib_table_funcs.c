/*
    Methods for getting data from the tables (see tables.h)
 */

//--- 1st Order (get index from user input)
//  int name_table_index(ncar, nord);   <-- if returns negative, parameters were non valid
int name_table_index(int ncar, int nord);
//  int binval_table_index(binval);

//--- 2nd Order (get index from index)
//  int z_table_index(index)
int z_table_index(int index);
//  int complement_table_index(index)
int complement_table_index(int index);

//--- 3rd Order (get data from index)
//  int ncar_table(index)
int ncar_table(int index);
//  int nord_table(index)
int nord_table(int index);
//  int[] pf_table(index)   <-- may return NULL - variable size array: use ncar to loop throug it. Caller responsible for deallocating!!!
int * pf_table(int index);
//  int n_table(index)
int n_table(int index);
//  int[6] icv_table(index) <-- may return NULL - 6 elem array. Caller responsible for deallocating!!!
int * icv_table(int index);



/** Returns the index in table of a PCS from its name
        @ params: cardinal number (int), ordinal number (int)
        @ returns: index (int)
 */
int name_table_index(int ncar, int nord)
{
    int index;
    int tmp_a, tmp_b;
    
    if(ncar<3 || ncar>9)
        return -1;              // invalid ncar
    if(nord<1 || nord>max_ord[ncar])
        return -2;              // invalid nord
    
    tmp_a = car_pos[ncar];      // index of pcs ncar-1
    tmp_b = 20 * (nord - 1);    // offset from ncar-1
    index = tmp_a + tmp_b;
    
    return index;
}

/** Returns the index in table of a PCS's Z mate from its index (0 if doesn't exists)
        @ params: index of PCS n (int)
        @ returns: index of n's Z mate or 0 (int)
 */
int z_table_index(int index);
{
    int z;
    z = tableptr[index+11];     // 11 is the offset at which z index is located
    if(z)
        z = z-1;                // z indexes have this 1 offset (why...?)
    return z;                   // it will return 0 (false) if no Z
}

/** Returns the index in table of the complement of a PCS from its index
        @ params: index of PCS n (int)
        @ returns: index of n's complement (int)
 */
int complement_table_index(int index)
{
    int complement;
    complement = tableptr[index+19];
    return complement;
}

/** Returns the cardinal number of a PCS from its index
        @ params: index of PCS n (int)
        @ returns: n's cardinal number (int)
 */
int ncar_table(int index)
{
    int ncar;
    ncar = tableptr[index];
    return ncar;
}

/** Returns the ordinal number of a PCS from its index
        @ params: index of PCS n (int)
        @ returns: n's ordinal number (int)
 */
int nord_table(int index)
{
    int nord;
    nord = tableptr[index+1];
    return nord;
}

/** Returns the prime form of a PCS from its index
        @ params: index of PCS n (int)
        @ returns: pointer to an array of lenght = ncar containing n's prime form (NULL
            if allocation fails)
        @ warning: caller is responsible for deallocating (and checking for NULL also)
 */
int * pf_table(int index)
{
    int ncar = tableptr[index];
    int j = index+2;
    int *pf; = NULL;
    pf = malloc(ncar * sizeof(int));
    if(!pf)
        return NULL;
    for(int i=0; i<ncar; i++) {
        pf[i] = tableptr[j+i];
    }
    return pf;
}

//---
int n_table(int index)
{
    int n;
    n = tableptr[index+12];
    return n;
}

/** Returns the interval vector of a PCS from its index
        @ params: index of PCS n (int)
        @ returns: pointer to an array of lenght = 6 containing n's ICV (NULL if allocation
            fails)
        @ warning: caller is responsible for deallocating (and checking for NULL also)
 */
int * icv_table(int index)
{
    int j = index+13;
    int *icv = NULL;
    icv = malloc(6 * sizeof(int));
    if(!icv)
        return NULL;
    for(int i=0; i<6; i++) {
        icv[i] = tableptr[j+i];
    }
    return icv;
}

