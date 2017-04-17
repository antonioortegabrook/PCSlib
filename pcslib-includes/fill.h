
/**
        Methods to fill a t_pcs from user input (pcs name or pitch content)
 */

/*
        Fill a t_pcs from its name
 */
int pcs_fill_from_name(t_pcs *pcs, int car, int ord, int tr, int inv);

/*
        Fill a t_pcs from pitch content
 */
int pcs_fill_from_pitch_content(t_pcs *pcs, int *vector, int nelem);

/**     Remove EOPs. Return a pointer to the new array and write number of
        pitches to an int passed as parameter
 
 */
int * remove_eops(int *delivered, int n, int *npitches);
