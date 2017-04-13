/**     types.h by Antonio Ortega Brook

        These are the PCSlib data types
 */

/** The new t_pcs struct
 */
typedef struct {
        int consistent;         //  Wether its data is consistent or not
        int ncar;               //  Cardinal number
        int nord;               //  Ordinal number
        int *pcs_delivered;     //  PCS as delivered
        int nelem;              //  Number of pcs and EOPs in pcs_delivered
        int *pitch_content;     //  Pitch content only (no EOPs)
        int npitches;           //  Number of pcs in pitch_content
        int prime_form[12];     //  Prime form (we no longer write EOC in prime form)
        int t;                  //  T value respect of the prime form
        int inverted;           //  True if inv, false if not
        int icv[6];             //  Interval-class vector
        int selected;           //  Whether the PCS is selected or not for some operation (...?)
        int table_index;        //  Index in the table
} t_pcs;        // add complement?


typedef struct {
        int mat[12][12];
        int NroFilas;
        int NroCols;
        int MaxElemxPos;
} t_cm;
