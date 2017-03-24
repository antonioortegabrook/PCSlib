/*
 Allocates a new empty p_pcs; if succesful, returns a pointer (null if not)
 */
t_pcs * pcs_new_empty();

/*
 Creates a new s_pcs from its name and t/i status
 */
t_pcs * pcs_new_from_name(int car, int ord, int tr, int inv);

/*
 Prevents us of building a matrix from a PCS that is bigger than the matrix itself
 */
int check_size(PCS* pcs);


/*
 Allocates a new empty p_pcs; returns a pointer (null if unsuccesful)
 */
t_pcs * pcs_new_empty()
{
    t_pcs *pcs = NULL;
    
    pcs = malloc(sizeof(t_pcs));
    if (!pcs)       // return the null pointer from malloc if allocation unsuccesful
        return NULL;
    
    pcs->pitches = NULL;
    pcs->consistent = false;    // mark as non consistent
    
    return pcs;
}

/*
 Creates a new s_pcs from its name and t/i status; returns a pointer (null if unsuccesful) // incompleto
 */
t_pcs * pcs_new_from_name(int car, int ord, int tr, int inv)
{
t_pcs *pcs = pcs_new_empty();
    
    if (!pcs)
        return NULL;
        
    pcs->pitches = malloc(car * sizeof(int) + sizeof(int));
    
    if (!pcs->pitches) {
          free(pcs);
          pcs = NULL;
          return NULL;
    }
    
    int index = name_table_index(car, ord);
    
    int *tmp_pitch_content = pf_table(index);
    if (!tmp_pitch_content) {
          /* free everything & return NUlL or mark non consistent */
    }
    
    int tmp_prime_form[13];
    for (int i=0; i<car; i++)
          tmp_prime_form[i] = tmp_pitch_content[i];
    
    int *tmp_icv = icv_table[index];
    if (!tmp_icv) {
          /* free everything & return NUlL or mark non consistent */
    }
    
    int err_t = transpose(tmp_pitch_content, car, tr);
    if (err_t) {
          /* free everything & return NUlL or mark non consistent */
    }
    
    int err_i = invert(tmp_pitch_content, car);
    if (err_i) {
          /* free everything & return NUlL or mark non consistent */
    }
    
    
    pcs->ncar = car;
    pcs->nord = ord;
    
    for (int i=0; i<car; i++)
          pcs->pitch_content[i] = tmp_pitch_content[i];
    pcs->pitch_content[car] = EOC;
    
    pcs->nelem = car + 1;
    
    for (int i=0; i<car; i++)
          pcs->prime_form[i] = tmp_prime_form[i];
    pcs->prime_form[car] = EOC;
    
    pcs->t = tr;
    pcs->inverted = inv;
    
    for (int i=0; i<6; i++)
          pcs->icv[i] = tmp_icv[i];
    
    pcs->selected = false;
    pcs->table_index = index;
    
    pcs->consistent = true;
    
    return pcs;
}


//-------------------------------------
/*
 Prevents us of building a matrix from a PCS that is bigger than the matrix itself
 */
int check_size(PCS* pcs) {
    
    int elepos=0, auxelepos=0, npos=0, i=0;
    
    while(pcs->find[i] != EOC){
        auxelepos=0;
        while(pcs->find[i] != EOP && pcs->find[i] != EOC){
            auxelepos++;
            i++;
        }
        if(elepos<auxelepos) elepos=auxelepos;
        if(pcs->find[i] != EOC) i++;;
        npos++;
    }
    if(npos>NEXTC || (npos==1 && elepos>NEXTC)) {
        //        object_error((t_object*)x, "Max number of positions per CM is %i", NEXTC);
        return(-1);     // too many positions
    }
    if(elepos>NEXTC && npos>1) {
        //        object_error((t_object*)x, "Max number of elements per position in a CM is %i", NEXTC);
        return(-2);     // too many elements per position
    }
    return(1);
}
