/**     types.h by Antonio Ortega Brook

        These are the PCSlib data types
 */

/** The new t_pcs struct
 */
typedef struct {
    int consistent;         //  Wether its data is consistent or not
    int ncar;               //  Cardinal number
    int nord;               //  Ordinal number
    int *pitch_content;     //  PCS as delivered
    int nelem;              //  Number of pcs and EOPs
    int prime_form[12];     //  Prime form (we no longer write EOC in prime form)
    int t;                  //  T value respect of the prime form
    int inverted;           //  True if inv, false if not
    int icv[6];             //  Interval-class vector
    int selected;           //  Whether the PCS is selected or not for some operation (...?)
    int table_index;        //  Index in the table
} t_pcs;
