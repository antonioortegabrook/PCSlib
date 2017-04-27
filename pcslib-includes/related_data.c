/*
	THESE FUNCTIONS SHOULD BE DEPRECATED SOON...
	To compute literal complement use literal_complement() in basic_funcs
	To get complement data from a t_pcs search its index in table
*/

/*      Get individual data from related PCSs
 */

/**      Get literal complement
                @ params: pointer to t_pcs, pointer to target array
                @ return: error code if NULL pointers
                @ warning: requires target of index 12 or complement's cardinal
 */
int pcs_literal_complement(t_pcs *pcs, int *target)
{
        int ncar, transp, inv;
        int tmp_vector[12];

        if (!pcs)
                return -1;
        if (!target)
                return -2;

        ncar   = pcs->ncar;
        transp = pcs->t;
        inv    = pcs->inverted;
        
        for (int i = 0; i < ncar; i++)
                tmp_vector[i] = pcs->prime_form[i];

        if (transp)
                transpose(tmp_vector, ncar, transp);
        
        if (inv)
                invert(tmp_vector, ncar);
        
        literal_complement(tmp_vector, ncar, target);


        return 0;
}


/**      Get complement's prime form
                @ params: pointer to t_pcs, pointer to target array
                @ return: error code if NULL pointers
                @ warning: requires target of index 12 or complement's cardinal
 */
int pcs_complement_pf(t_pcs *pcs, int *target)
{
        int pcs_index  = pcs->table_index;
        int comp_index = complement_table_index(pcs_index);

        if (!pcs)
                return -1;
        if (!target)
                return -2;

        pf_table(comp_index, target);

        return 0;
}
