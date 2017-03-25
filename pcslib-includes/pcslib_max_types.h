/*  pcslib_max_types.h by Antonio Ortega Brook - March 2017
 
    --- Only used for Max objects ---
 
    These data types are used to send pointers from one object to
    another in a *safe* way, instead of passing them as symbols inside
    atoms. There are also methods for set and retrieve pointer data
    from them (see pcslib_max_funcs.c)
 */


/** PCSlib types
 */
typedef enum {
    P_PCS = 30,     // < pointer to a PCS
    P_CM            // < pointer to a CM
} e_pcslib_ptr_types;

/** Union for packing the PCSlib types defined above
 */
union pcs_word {
    t_pcs *pcs;
    t_cm *cm;
};

/** Atom-like struct for wrapping pointers to be sent to other objects;
    we cast *t_ptr_mess to *t_atom to pass it through an outlet_anything and
    use A_GIMME to receive them
 */
typedef struct ptr_mess {
    short           p_type;
    union pcs_word  p_w;
} t_ptr_mess;
