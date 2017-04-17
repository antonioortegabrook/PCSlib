
/**
        Methods to fill a t_pcs from user input (pcs name or pitch content)
 */



/**     Fill a t_pcs from its name
                Assumes an initialized PCS. Caller is responsible for what it passes
                as parameters
 */
int pcs_fill_from_name(t_pcs *pcs, int car, int ord, int tr, int inv)
{
        int index;
        int tmp_pitch_content[13];
        int tmp_prime_form[12];
        int tmp_icv[6];
        int err = 0;


        /*
         checkear delivered. si es mayor o igual que nelem, lo dejamos como está.
         si es menor, liberamos y reasignamos
         */
        if (pcs->delivered) {
                if (pcs->nelem < car) {
                        free(pcs->delivered);
                        pcs->delivered = NULL;
                }
        }
        
        if (!pcs->delivered)
                pcs->delivered = malloc(car * sizeof(int));
        
        if (!pcs->delivered)
                return -1;
        
        
        /**
         checkear pitch_content. si es mayor o igual que ncar, lo dejamos como está.
         si es menor, liberamos y reasignamos
         
         warning: nelem might not be reliable if pcs is marked as non consistent
         */
        if (pcs->pitch_content) {
                if (pcs->nelem < car) {
                        free(pcs->pitch_content);
                        pcs->pitch_content = NULL;
                }
        }

        if (!pcs->pitch_content)
                pcs->pitch_content = malloc(car * sizeof(int));

        if (!pcs->pitch_content) {
                pcs->consistent = false;
                return -1;
        }


        index = name_table_index(car, ord);     // should we check existence here?

        /**
                Set temp data
         */
        pf_table(index, tmp_pitch_content);
        
        pf_table(index, tmp_prime_form);
        
        icv_table(index, tmp_icv);

        if (tr)                         // transpose...?
                transpose(tmp_pitch_content, car, tr);

        if (inv)                        // invert...?
                invert(tmp_pitch_content, car);

        /**
                Check for errors

                should we check temp data here, or should we
                ensure before that no bad data is passed to functions?
         */



        /**
                Copy temp data to struct
         */
        pcs->ncar = car;
        pcs->nord = ord;


        for (int i = 0; i < car; i++)
                pcs->delivered[i] = tmp_pitch_content[i];
        
        pcs->nelem = car;


        for (int i = 0; i < car; i++)
                pcs->pitch_content[i] = tmp_pitch_content[i];

        pcs->npitches = car;


        for (int i = 0; i < car; i++)
                pcs->prime_form[i] = tmp_prime_form[i];


        pcs->t = tr;
        pcs->inverted = inv;

        for (int i = 0; i < 6; i++)
                pcs->icv[i] = tmp_icv[i];


        pcs->selected = false;
        pcs->table_index = index;

        /*
                Check for errors (improve)
         */
        if (err) {

                return -3;
        }

        pcs->consistent = true;

        return 0;
}

/**     Fill a t_pcs from a delivered PCS. May have several positions
        t_pcs must be initialized
        Expects no EOC at the end
 */
int pcs_fill_from_pitch_content(t_pcs *pcs, int *vector, int nelem)
{
        int *tmp_pitch_content;
        int tmp_npitches;
        int pf_binvalue;
        int index;
        int tmp_ncar;
        int tmp_nord;
        int tmp_pf[12];
        int tmp_t;
        int tmp_i;
        int tmp_icv[6];
        
        
        /**     Mark as non consistent first
         */
        pcs->consistent = false;
        
        
        /*
         checkear delivered. si es mayor o igual que nelem, lo dejamos como está.
         si es menor, liberamos y reasignamos
         */
        if (pcs->delivered) {
                if (pcs->nelem < nelem) {
                        free(pcs->delivered);
                        pcs->delivered = NULL;
                }
        }
        
        if (!pcs->delivered)
                pcs->delivered = malloc(nelem * sizeof(int));
        
        if (!pcs->delivered)
                return -1;


        /**     Remove EOPs
         */
        tmp_pitch_content = remove_eops(vector, nelem, &tmp_npitches);
        
        if (!tmp_pitch_content)
                return -3;


        /*
         checkear pitch_content. si es mayor o igual que npitches, lo dejamos como está.
         si es menor, liberamos y reasignamos
         */
        if (pcs->pitch_content) {
                if (pcs->npitches < tmp_npitches) {
                        free(pcs->pitch_content);
                        pcs->pitch_content = NULL;
                }
        }
        
        if (!pcs->pitch_content)
                pcs->pitch_content = malloc(tmp_npitches * sizeof(int));
        
        if (!pcs->pitch_content)
                return -1;
        
        
        
        
        
        /**     Get binary value from pitch content
                (we now pass ONLY pitch data to prime_form_data())
         */
        prime_form_data(tmp_pitch_content, tmp_npitches, &pf_binvalue, &tmp_ncar, &tmp_t, &tmp_i);
        
        
        /**     Get index in table from binary value
         */
        index = binval_table_index(pf_binvalue, tmp_ncar);
        
        
        /**     Get table data from index
         */
        tmp_ncar = ncar_table(index);
        
        tmp_nord = nord_table(index);
        
        pf_table(index, tmp_pf);
        
        icv_table(index, tmp_icv);
        
        
        /**     Write data to struct
         */
        pcs->ncar = tmp_ncar;
        pcs->nord = tmp_nord;
        
        for (int i = 0; i < nelem; i++)
                pcs->delivered[i] = vector[i];
        
        pcs->nelem = nelem;
        
        
        for (int i = 0; i < tmp_npitches; i++)
                pcs->pitch_content[i] = tmp_pitch_content[i];
        
        pcs->npitches = tmp_npitches;
        
        
        for (int i = 0; i < tmp_ncar; i++)
                pcs->prime_form[i] = tmp_pf[i];
        
        
        pcs->t = tmp_t;
        pcs->inverted = tmp_i;
        
        for (int i = 0; i < 6; i++)
                pcs->icv[i] = tmp_icv[i];
        
        pcs->selected = false;
        pcs->table_index = index;
        
        
        /**     Mark as consistent once that everything has been copied
         */
        pcs->consistent = true;
        
        
        return 0;
}


/**     Remove EOPs. Return a pointer to the new array and write number of
        pitches to an int passed as parameter
 
 */
int * remove_eops(int *delivered, int n, int *npitches)
{
        int *no_eops;
        int tmp_npitches;
        int i, j;

        /**     First, count how many EOPs are in the delivered array
         */
        j = 0;
        for (i = 0; i < n; i++) {
                if (delivered[i] == EOP)
                        j++;
        }

        /**     Find out how many pitches are
         */
        tmp_npitches = n - j;

        /**     Now we can allocate memory for the new array
         */
        no_eops = malloc(tmp_npitches * sizeof(int));

        if (!no_eops)
                return NULL;

        /**     Copy only pitches to the new array
         */
        j = 0;
        for (i = 0; i < n; i++) {
                
                if (delivered[i] != EOP) {
                        no_eops[j] = delivered[i];
                        j++;
                }
        }

        /**     And... return
         */
        *npitches = tmp_npitches;

        return no_eops;
}



