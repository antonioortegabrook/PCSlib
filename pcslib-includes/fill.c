

/*
        Fill a t_pcs from its name
 */
int pcs_fill_from_name(t_pcs *pcs, int car, int ord, int tr, int inv)
{
        int index = name_table_index(car, ord);
        int tmp_pitch_content[13];
        int tmp_prime_form[12];
        int tmp_icv[6];
        int err = 0;
        
        if (!pcs)
                return -1;
        /*
         checkear pitch_content. si es mayor o igual que ncar, lo dejamos como está.
         si es menor, liberamos y reasignamos
         */
        if (pcs->pitch_content) {
                if (pcs->nelem < car)
                        pcs->pitch_content = NULL;
        }
        if (!pcs->pitch_content)
                pcs->pitch_content = malloc(car * sizeof(int));
        
        if (!pcs->pitch_content)
                return -2;


        /*
                Set temp data
         */
        err += pf_table(index, tmp_pitch_content);
        
        err += pf_table(index, tmp_prime_form);
        
        err += icv_table(index, tmp_icv);
        
        if (tr)                         // transpose...?
                err += transpose(tmp_pitch_content, car, tr);
        
        if (inv)                        // invert...?
                err += invert(tmp_pitch_content, car);
        
        /*
                Copy temp data to struct
         */
        pcs->ncar = car;
        pcs->nord = ord;
        
        for (int i = 0; i < car; i++)
                pcs->pitch_content[i] = tmp_pitch_content[i];

        pcs->nelem = car;

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

/*
        Fill a t_pcs from pitch content
        t_pcs must be initialized
        Expects no EOC at the end
 */
int pcs_fill_from_pitch_content(t_pcs *pcs, int *vector, int nelem)
{
        int pf_binvalue;
        int index;
        int tmp_ncar;
        int tmp_nord;
        int tmp_pf[12];
        int tmp_t;
        int tmp_i;
        int tmp_icv[6];
        int err = 0;
        
        if (!pcs)
                return -1;
        if (!vector)
                return -2;
        
        /*
                Mark as non consistent first
         */
        pcs->consistent = false;
        
        /*
         checkear pitch_content. si es mayor o igual que nelem+1, lo dejamos como está.
         si es menor, liberamos y reasignamos
         */
        if (pcs->pitch_content) {
                if (pcs->nelem < nelem) {
                        free(pcs->pitch_content);
                        pcs->pitch_content = NULL;
                }
        }
        if (!pcs->pitch_content)
                pcs->pitch_content = malloc(nelem * sizeof(int));
        
        if (!pcs->pitch_content)
                return -3;

        /*
                Get binary value from pitch content
         */
        err += prime_form_data(vector, nelem, &pf_binvalue, &tmp_ncar, &tmp_t, &tmp_i);

        /*
                Get index in table from binary value
         */
        index = binval_table_index(pf_binvalue, tmp_ncar);

        /*
                Get table data from index
         */
        tmp_ncar = ncar_table(index);

        tmp_nord = nord_table(index);

        err += pf_table(index, tmp_pf);

        err += icv_table(index, tmp_icv);

        /*
                Write data to struct
         */
        pcs->ncar = tmp_ncar;
        pcs->nord = tmp_nord;

        for (int i = 0; i < nelem; i++)
                pcs->pitch_content[i] = vector[i];

        pcs->nelem = nelem;

        for (int i = 0; i < tmp_ncar; i++)
                pcs->prime_form[i] = tmp_pf[i];

        pcs->t = tmp_t;
        pcs->inverted = tmp_i;

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
