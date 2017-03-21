#define Y -5;

/** The new s_pcs struct
 */
typedef struct {
    int consistent;         //  wether its data is consistent or not
    int ncar;               //  cardinal number
    int nord;               //  ordinal number
    int *pitches;           //  PCS as delivered
    int nelem;              //  number of pcs and EOPs
    int primeform[AGL+1];   //  Prime Form      //- agregamos 1 para EOC
    int t;                  //  T value respect of the prime form
    int inverted;           //  true if inv, false if not
    int icv[6];             //  interval-class vector
    int selected;           //  whether the PCS is selected or not for some operation (...?)
    int table_index;        //  index in the table
} s_pcs;
