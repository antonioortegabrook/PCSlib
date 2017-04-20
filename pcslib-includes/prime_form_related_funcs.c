//#include "prime_form_related_funcs.h"



/**     Computes pime-form's binary value and t/i status
                
                @ parameters: pointer to an array (int) whith the pitch content as
                        received, # of elements, pointers (int) to write binary value,
                        cardinal number, transposition factor and inversion status

                @ warning: doesn't check pointers; it's caller responsiblity to pass good data
 */
void prime_form_data(int *pitches, int n, int *bin_value, int *ncar, int *tr, int *inv)
{
        int card, pcs[12];
        int bin_val, transp;
        int i_bin_val, i_transp;
        int descendent, inverted;


        descendent = is_descendent(pitches, n);                 // 0- see if we prefer inverted

        filter_and_sort(pitches, n, pcs, &card);                // 1- filter & sort

        minimum_bin_value(pcs, card, &bin_val, &transp);        // 2- find min bin value

        invert(pcs, card);                                      // 3- invert
        
        minimum_bin_value(pcs, card, &i_bin_val, &i_transp);    // 4- find inversion min bin value


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
                        i_transp = (i_transp + 5) % 12;
                        break;
                case 717:   // 6-Z29
                        i_bin_val = 843;
                        i_transp = (i_transp + 6) % 12;
                        break;
                case 691:   // 6-31
                        i_bin_val = 811;
                        i_transp = (i_transp + 4) % 12;
                        break;
                case 743:   // 7-20
                        i_bin_val = 919;
                        i_transp = (i_transp + 5) % 12;
                        break;
                case 1467:  // 8-26
                        i_bin_val = 1719;
                        i_transp = (i_transp + 3) % 12;
                        break;
                default:
                        break;
        }
        /** End special cases
         */


        /*      Compare binary values of inverted and non inverted forms; if equal,
                we prefer inverted if delivered is descendent and vice versa
         */
        inverted = (i_bin_val < bin_val || (i_bin_val == bin_val && descendent)) ? 1 : 0;


        /*      Write to target
         */
        if (inverted) {
                
                *bin_value = i_bin_val;
                *ncar      = card;
                *tr        = i_transp;
                *inv       = true;
                
        } else {
                
                *bin_value = bin_val;
                *ncar      = card;
                *tr        = transp;
                *inv       = false;
        }
        
        return;
}


/**     Sort a PCS, take mod 12, removes duplicates and EOPs/EOCs (revisar este comentario)
 
                @ parameters: pointer to an array (int), array length (int) (must be [12]!)
                
                @ returns: pointer to a 13 element array (int); index 12 is equal to n
                        (# of pitch classes); from index 0 to index, n-1 the pitch classes,
                        sorted, mod 12, without duplicates
 */
void filter_and_sort(int *pitches, long n, int *sorted, int *nelem)
{
        int bit_vector[12];
        int j;
        
        for (int i = 0; i < 12; i++)
                bit_vector[i] = -5;
        
        for (long i = 0; i < n; i++) {
                if (pitches[i] != EOP) {
                        j = pitches[i] % 12;  // mod 12 is very important here!
                        bit_vector[j] = j;
                }
        }
        
        j = 0;
        for (int i = 0; i < 12; i++) {
                if (bit_vector[i] != -5) {
                        sorted[j] = bit_vector[i];
                        j++;
                }
        }
        
        *nelem = j;                 // store # of elems
        return;
}


/**     Find the minimum binary value and its transposition of the first n elements of
        a given array
 
                @ parameters: pointer to array, n (# of elements), pointers (int) for
                        storing minimum binary value and transposition factor
 
                @ warning: doesn't check pointers; it's caller responsiblity to pass good
                        data

                @ note: doesn't write to the original array, so it can be reused
 */
void minimum_bin_value(int *vector, int n, int *mbi, int *t)
{
        int rots[12][12], binvals[12], transp[12];
        int min_bv, tf;
        int tmp;
        
        /*
                Binary values array must be initialized to 0
         */
        for (int i = 0; i < 12; i++)
                binvals[i] = 0;
        
        /*
                Put input vector in the first row of rotations
         */
        for (int i = 0; i < n; i++)
                rots[0][i] = vector[i];
        
        /*
                Compute all the other rotations
         */
        for (int i = 1; i < n; i++) {
                tmp = rots[i - 1][0];
                
                for (int j = 0; j < n - 1; j++)
                        rots[i][j] = rots[i - 1][j + 1];
                
                rots[i][n-1] = tmp;
        }
        
        /*
                Transpose rotations to 0
         */
        for (int i = 0; i < n; i++) {
                transp[i] = rots[i][0];
                
                for (int j = 0; j < n; j++) {
                        rots[i][j] -= transp[i];
                        
                        if (rots[i][j] < 0)
                                rots[i][j] += 12;
                }
        }
        
        /*
                Find binary value for each rotation
         */
        for (int i = 0; i < n; i++) {
                
                for (int j = 0; j < n; j++)
                        binvals[i] += pow(2, rots[i][j]);

        }
        
        /*
                Get minimum binary value
         */
        min_bv = binvals[0];
        tf = transp[0];
        
        for(int i = 1; i < n; i++) {
                if(binvals[i] < min_bv) {
                        min_bv = binvals[i];
                        tf = transp[i];
                }
        }
        
        /*
                Write minimum binary value and transposition factor
         */
        *mbi = min_bv;
        *t = tf;
        
        return;
}


/**
        We use this function to tell if we prefer taking a pcs as inverted
        or not when inversion is equivalent to transposition
 */
int is_descendent(int *vector, int n)
{
        int descendent;
        int up = 0, dn = 0;


        for (int i = 1; i < n; i++) {
                up += vector[i] > vector[i - 1];
                dn += vector[i] < vector[i - 1];
        }


        if (up >= dn)
                descendent = false;

        if (up < dn)
                descendent = true;


        return descendent;
}
