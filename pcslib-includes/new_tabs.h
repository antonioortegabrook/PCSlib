/*  Tables  */

#include <stdint.h>

/** These are used to search *efficiently* trhough the big table;

    @ car_pos   stores the index in table of the first pcs of each cardinality, beeing the index of this
                    array equals to the cardinal number we have; to get the PCS we are looking for, substract
                    1 from ordinal number, multiply by 20 an add to it; i.e. to get index of PCS 5-10:
                    car_pos[5] = 820; then (16-1) * 20 = 300; then 820 + 300 = 1120 and that's the index of
                    5-16 in the big table.-

    @ max_ord   stores the maximum number of PCS of each cardinality, beeing the index of this array equal to
                    the cardinal number we have.-

    @ bv_idx    stores the index at which is located the binary value of the first PCS of each cardinality in
                    the bv_index array.-
 
    @ bin_vals  stores the binary value of each PCS in its prime form; multiply the index of this array by 20
                    to get the position in the big table; use bv_idx and max_ord to search *efficiently*
                    through bin_vals (i.e. search beetwen bv_idx and max_ord);
                    NOTE THAT the "minimum binary value" method gives us the Rahn's prime form and we are
                    interested in Forte's prime form; the known special cases will be either mapped to the
                    corresponding prime form here or in the function that computes bin values; we havent
                    decided yet.-
*/

/*                         card    3   4    5    6     7     8     9                                          */
int16_t car_pos[10] = {-1,-1,-1,   0,  240, 820, 1580, 2580, 3340, 3920};  /* index of first PCS of each card */
int16_t max_ord[10] = {-1,-1,-1,   12, 29,  38,  50,   38,   29,   12};    /* # of PCS of each card           */
int16_t bv_idx[10]  = {-1,-1,-1,   0,  12,  41,  79,   129,  167,  196};   /* index of first bv of each card  */

int16_t bin_vals[208]; = /* minimum binary value of each PCS (except for the known special cases); index of
                        this array corresponds to the number of row in the table; multiply it by 20 to
                        get index of table array                                                          */
/* card 3 (12) */
{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* card 4 (29) */
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* card 5 (38) */
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* card 6 (50) */
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1-1,-1,-1,-1,-1,-1,-1,-1,-1,-1-1,-1,-1,-1,-1,-1,-1,-1,-1,-1-1,-1,-1,-1,-1,-1,-1,-1,-1,-1-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* card 7 (38) */
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* card 8 (29) */
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* card 9 (12) */
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

/** PCS table (aka the big table)
 */
int16_t pcs_table[4160];
