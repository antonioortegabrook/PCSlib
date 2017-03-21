/*
 Allocates a new empty p_pcs; if succesful, returns a pointer (null if not)
 */
s_pcs * pcs_new_empty();

/*
 Creates a new s_pcs from its name and t/i status
 */
s_pcs * pcs_new_from_name(int car, int ord, int tr, int inv);

/*
 Prevents us of building a matrix from a PCS that is bigger than the matrix itself
 */
int check_size(PCS* pcs);


/*
 Allocates a new empty p_pcs; returns a pointer (null if unsuccesful)
 */
s_pcs * pcs_new_empty()
{
    s_pcs *pcs = NULL;
    
    pcs = malloc(sizeof(s_pcs));
    if (!pcs)       // return the null pointer from malloc if allocation unsuccesful
        return pcs;
    
    pcs->pitches = NULL;
    pcs->consistent = false;    // mark as non consistent
    
    return pcs;
}

/*
 Creates a new s_pcs from its name and t/i status; returns a pointer (null if unsuccesful) // incompleto
 */
s_pcs * pcs_new_from_name(int car, int ord, int tr, int inv)
{
    int lookup
    s_pcs *pcs = pcs_new_empty();
    if (!pcs)
        return pcs;
    pcs->pitches = malloc(car * sizeof(int));
    if (!pcs->pitches) {
        pcs = NULL;
        return pcs;
    }
    
    
}

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
