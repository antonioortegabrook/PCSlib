/*  pcslib_max_funcs.h by Antonio Ortega Brook - March 2017
 
 --- Only used for Max objects ---
 
 These are the methods for set and retrieve pointer data from t_ptr_mess,
 an atom-like struct to pass pointers from one object to another in a *safe* way
 (see pcslib_max_types.h)
 */

t_pcs *patom_getpcs(const t_patom *p);
t_cm *patom_getcm(const t_patom *p);

int patom_setpcs(t_patom *p, t_pcs *s);
int patom_setcm(t_patom *p, t_cm *s);


/** retrieves a PCS pointer from a t_ptr_mess;
    returns NULL if type doesn't match
 */
t_pcs *patom_getpcs(const t_patom *p) {
    t_pcs *ptr = NULL;
    
    if (p->p_type == P_PCS)
        ptr = p->p_w.pcs;
    
    return ptr;
}

/** retrieves a CM pointer from a t_ptr_mess;
 returns NULL if type doesn't match
 */
t_cm *patom_getcm(const t_patom *p)
{
    t_cm *ptr = NULL;

    if (p->p_type == P_CM)
        ptr = p->p_w.cm;

    return ptr;
}

/** inserts a PCS pointer into a t_ptr_mess and changes its type to P_PCS
 */
int patom_setpcs(t_patom *p, t_pcs *s)
{
    if (!p)
        return -100;    // -100 = null ptr_mess
    if (!s)
        return -101;    // -101 = null PCS
    
    p->p_type = P_PCS;
    p->p_w.pcs = s;
    
    return 0;           // 0 = success!
}

/** inserts a CM pointer into a t_ptr_mess and changes its type to P_CM
 */
int patom_setcm(t_patom *p, t_cm *s)
{
    if (!p)
        return -100;    // -100 = null ptr_mess
    if (!s)
        return -102;    // -101 = null CM
    
    p->p_type = P_CM;
    p->p_w.cm = s;
    
    return 0;           // 0 = success!
}
