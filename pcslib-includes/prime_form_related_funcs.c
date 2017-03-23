
/*
    Prime form related functions
 */




int pcs_sort(int *pitches, long n, int *sorted, int *nelem);
int pcs_min_bin_value(int *vector, int n, int *mbi, int *t);
int pcs_invert(int *vector, int n);

int prime_form_data(int *pitches, int n, int *bin_value, int *tr, int *inv);
/** Computes pime-form's binary value and t/i status
 
 */
int prime_form_data(int *pitches, int n, int *bin_value, int *tr, int *inv)
{
    if (!pitches)
        return -1;
    
    int card, pcs[12];
    int sort_err = pcs_sort(pitches, n, pcs, &card);                // 1- filter & sort
    if (sort_err)
        return -2;
    
    int bin_val, transp;
    int bin_err = pcs_min_bin_value(pcs, card, &bin_val, &transp);  // 2- find min bin value
    if (bin_err)
        return -3;
    
    int inv_err = pcs_invert(pcs, card);                            // 3- invert
    if (inv_err)
        return -4;
    
    int i_bin_val, i_transp;
    int i_bin_err = pcs_min_bin_value(pcs, card, &i_bin_val, &i_transp);    // 4- find inversion min bin value
    if (i_bin_err)
        return -5;
    
    /** Special cases
     These are the cases in which the minimum binary value doesn't match Forte's
     prime form, so we correct them
     */
    switch (bin_val) {
        case 355:   // 5-20
                bin_val = 395;
                transp = (transp + 5) % 12;
                break;
        case 717:   // 6-Z29
                bin_val = 843;
                transp = (transp + 6) % 12;
                break;
        case 691:   // 6-31
                bin_val = 811;
                transp = (transp + 4) % 12;
                break;
        case 743:   // 7-20
                bin_val = 919;
                transp = (transp + 5) % 12;
                break;
        case 1467:  // 8-26
                bin_val = 1719;
                transp = (transp + 3) % 12;
                break;
        default:
                break;
    }
    // inversions
    switch (i_bin_val) {
        case 355:   // 5-20
                i_bin_val = 395;
                i_transp = (i_transp + 7) % 12;
                break;
        case 717:   // 6-Z29
                i_bin_val = 843;
                i_transp = (i_transp + 6) % 12;
                break;
        case 691:   // 6-31
                i_bin_val = 811;
                i_transp = (i_transp + 8) % 12;
                break;
        case 743:   // 7-20
                i_bin_val = 919;
                i_transp = (i_transp + 7) % 12;
                break;
        case 1467:  // 8-26
                i_bin_val = 1719;
                i_transp = (i_transp + 9) % 12;
                break;
        default:
                break;
    }
    /** End special cases
     */
    
    if (bin_val <= i_bin_val) {                                      // 5- compare results
        *bin_value = bin_val;                                       // 6- write to target
        *tr = transp;
        *inv = false;
        
    } else if (i_bin_val < bin_val) {
        *bin_value = i_bin_val;
        *tr = i_transp;
        *inv = true;
    }
    
    return 0;
}


/** Sort a PCS, take mod 12, removes duplicates and EOPs/EOCs
 
 @ parameters: pointer to an array (int), array length (int)
 @ returns: pointer to a 13 element array (int); index 12 is equal to n
 (# of pitch classes); from index 0 to index, n-1 the pitch classes,
 sorted, mod 12, without duplicates
 */
int pcs_sort(int *pitches, long n, int *sorted, int *nelem)
{
    if(!pitches)
        return -2;
    
    if(sizeof(sorted) < 12)
        return -2;
    
    int bit_vector[12];
    for(int i=0; i<12; i++)
        bit_vector[i] = -5;
    
    int j;
    for(long i=0; i<n; i++) {
        if(pitches[i] != EOP && pitches[i] != EOC) {
            j = pitches[i] % 12;  // mod 12 is very important here!
            bit_vector[j] = j;
        }
    }
    
    j=0;
    for(int i=0; i<12; i++) {
        if(bit_vector[i] != -5) {
            sorted[j] = bit_vector[i];
            j++;
        }
    }
    
    *nelem = j;                 // store # of elems
    return 0;
}


// int pcs_min_bin_value(int *vector, int n, int *mbi, int *t);
/** Find the minimum binary value and its transposition of the first n elements of
 a given array
 
 @ parameters: pointer to array, n (# of elements), pointer to int for storing
 minimum bin value, pointer to int for storing transposition
 @ returns: error code (int) -1 for null pointer to array, -2 for null pointer
 to mbi, -3 for null pointer to t
 @ note: doesn't write to the original array, so it can be reused
 */
int pcs_min_bin_value(int *vector, int n, int *mbi, int *t)
{
    if(!vector)
        return -1;
    if(!mbi)
        return -2;
    if(!t)
        return -3;
    
    int rots[12], binvals[12], transp[12];
    
    for(int i=0; i<12; i++)
        binvals[i] = 0;
    
    for(int i=0; i<n; i++)
        rots[i] = vector[i];
    
    int tmp;
    for(int i=0; i<n; i++) {
        
        // Transpose
        transp[i] = rots[0];
        for(int j=0; j<n; j++) {
            rots[j] -= transp[i];
            if(rots[j] < 0)
                rots[j] += 12;
        }
        
        // Get binary value
        binvals[i] += pow(2, vector[i]);
        
        // Rotate
        tmp = rots[0];
        for(int i=0; i < n-1; i++) {
            rots[i] = rots[i+1];
        }
        rots[n-1] = tmp;
    }
    
    // Get minimum binary value
    int min_bv = binvals[0];
    int tf = transp[0];
    
    for(int i=1; i<n; i++) {
        if(binvals[i] < min_bv) {
            min_bv = binvals[i];
            tf = transp[i];
        }
    }
    
    // Write minimum binary value
    *mbi = min_bv;
    *t = tf;
    
    return 0;
}


/** Invert a PCS. Puts results in input
 @ parameters: pointer to an array (int), # of elements
 @ returns: error code -1 if array is null
 */
int pcs_invert(int *vector, int n)
{
    if(!vector)
        return -1;
    
    int tmp[12];
    
    for (int i=0; i<n; i++)
        tmp[i] = vector[i];
    
    for (int i=0; i<n; i++)
        tmp[i] = 12 - tmp[i];
    
    for (int i=0; i<n; i++)
        vector[i] = tmp[i];
    
    return 0;
}

