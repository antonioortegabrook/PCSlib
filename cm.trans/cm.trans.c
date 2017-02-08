/*  cm_trans object  by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
-------
 
*/

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS
 
        Borrar variables no utilizadas...?
*/

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

#include "../pcslib-includes/pcslib_max.c"       //    PCSlib header

typedef struct _cm_trans {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    CM *cm1;		  /*pointers to CM structs*/
    CM *cm2;
    PCS *pcs;
    t_int n;        /*transposition operator*/
    t_int cmnew;
    t_int row[2];
    t_int col[2];
    
    long p_in;
    void *pcs_in;
    void *cm_in;
    void *cm_out;
} t_cm_trans;

// these are prototypes for the methods that are defined below
void cm_trans_pcs_ptr(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv);
void cm_trans_cm_ptr(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv);

void cm_trans_ec(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv);
void cm_trans_er(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv);
void cm_trans_t(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv);
void cm_trans_i(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv);
void cm_trans_m(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv);
void cm_trans_mi(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv);
void cm_trans_r90(t_cm_trans *x);
void cm_trans_rd(t_cm_trans *x, t_symbol *s, long n);
void cm_trans_swap(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv);
void cm_trans_del(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv);
void cm_trans_wri(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv);
void cm_trans_add(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv);

void output (t_cm_trans *x);

void cm_trans_assist(t_cm_trans *x, void *b, long m, long a, char *s);
void stdinletinfo();
void cm_trans_free(t_cm_trans *x);
void *cm_trans_new();

static t_class *cm_trans_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("cm.trans", (method)cm_trans_new, (method)cm_trans_free, sizeof(t_cm_trans), 0L, 0);
    
    class_addmethod(c, (method)cm_trans_pcs_ptr,        MPID,          A_GIMME, 0);    // pointer to PCS
    class_addmethod(c, (method)cm_trans_cm_ptr,         MMID,          A_GIMME, 0);    // pointer to CM
    
    class_addmethod(c, (method)cm_trans_ec,             "ec",          A_GIMME, 0);
    class_addmethod(c, (method)cm_trans_er,             "er",          A_GIMME, 0);
    class_addmethod(c, (method)cm_trans_t,              "t",           A_GIMME, 0);
    class_addmethod(c, (method)cm_trans_i,              "i",           A_GIMME, 0);
    class_addmethod(c, (method)cm_trans_t,              "m",           A_GIMME, 0);
    class_addmethod(c, (method)cm_trans_i,              "mi",          A_GIMME, 0);
    class_addmethod(c, (method)cm_trans_r90,            "r90",                  0);
    class_addmethod(c, (method)cm_trans_rd,             "rd",          A_LONG,  0);
    class_addmethod(c, (method)cm_trans_swap,           "swap",        A_GIMME, 0);
    class_addmethod(c, (method)cm_trans_del,            "del",         A_GIMME, 0);
    class_addmethod(c, (method)cm_trans_wri,            "wri",         A_GIMME, 0);
    class_addmethod(c, (method)cm_trans_add,            "add",         A_GIMME, 0);
    
    class_addmethod(c, (method)cm_trans_assist,         "assist",      A_CANT,  0);	// (optional) assistance method
    class_addmethod(c, (method)stdinletinfo,            "inletinfo",   A_CANT,  0);

	class_register(CLASS_BOX, c);
	cm_trans_class = c;
}

//--------------------------------------------------------------------------

void *cm_trans_new() {
	t_cm_trans *x;                                             // local variable (pointer to a t_cm_trans data structure)

	x = (t_cm_trans *)object_alloc(cm_trans_class);           // create a new instance of this object
    
    x->pcs_in = proxy_new(x, 2, &x->p_in);
    x->cm_in = proxy_new(x, 1, &x->p_in);
                                     // create outlets
    x->cm_out=outlet_new(x, NULL);
    
    x->cm1=NULL;
    x->cm2=NULL;
    x->pcs=NULL;
    
    x->cm1=(CM*)malloc(sizeof(CM));
    x->cm2=(CM*)malloc(sizeof(CM));
    Inicializar(x->cm1);
    Inicializar(x->cm2);
    x->cmnew=FALSE;
    
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs->find[0]=EOC;
    
    x->n=0.; /*transposition factor initialized to 0.*/
    
	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void cm_trans_assist(t_cm_trans *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET) {
        switch(a) {
            case(0):
                sprintf(s,"ec #row #column, er #row #column, t(n), it(n), r90, rd (0=R->L, 1=L->R), swap, wri #row #column, del #row #column, add #row #column");
                break;
            case(1):
                sprintf(s,"CM");
                break;
            case(2):
                sprintf(s,"PCS");
                break;
        }
    }
    if (m == ASSIST_OUTLET) {
            sprintf(s,"CM");
    }
}


void cm_trans_pcs_ptr(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=2) {
        object_error((t_object*)x, "wrong message");
        return;
    }
    
    {  //------------ get pcs ptr --------------
        t_symbol *temp;
        PCS *tempcs;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
        CopiaSet(tempcs, x->pcs);      //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------
    
    return;
}


void cm_trans_cm_ptr(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=1) {
        object_error((t_object*)x, "wrong message");
        return;
    }
    
    {  //----------- get cm ptr ------------
        t_symbol *temp;
        CM *tempcm;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcm);    // get the pointer to a PCS struct
        CopiaMatriz(tempcm, x->cm1);                    //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------
    x->cmnew=TRUE;
    
    return;
}

void cm_trans_ec(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv) {     // column exchange
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "left inlet doesn't accept pointers");
        return;
    }
    
    t_int i, n[2];
    
    CopiaMatriz(x->cm1,x->cm2);
    
    if (argc != 2){
        object_error((t_object*)x, "missing or extra arguments for message 'ec'");
        return;
    }
    for (i=0; i<2; i++) {
        if (atom_gettype(argv+i) != A_LONG && atom_gettype(argv+i) != A_FLOAT) {
            object_error((t_object *)x, "bad arguments for message ec");
            break;
            return;
        }
        n[i]= atom_getlong(argv+i);
    }
    if(n[0] > x->cm2->NroCols || n[1] > x->cm2->NroCols){
        object_warn((t_object*)x, "the CM doesn't have the number of column requested");
        return;
    }
    InterCols(x->cm2,n[0],n[1]);
    output (x);
    return;
}

void cm_trans_er(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv) {     // row exchange
    t_int i, n[2];
    
    CopiaMatriz(x->cm1,x->cm2);     //- deberíamos copiar la cm después de checkear los argumentos
    
    if (argc != 2){
        object_error((t_object*)x, "missing or extra arguments for message 'ec'");
        return;
    }
    for (i=0; i<2; i++) {
        if (atom_gettype(argv+i) != A_LONG && atom_gettype(argv+i) != A_FLOAT) {
            object_error((t_object *)x, "bad arguments for message ec");
            break;
            return;
        }
        n[i]= atom_getlong(argv+i);
    }
    if(n[0] > x->cm2->NroFilas || n[1] > x->cm2->NroFilas ){
        object_warn((t_object*)x, "the CM doesn't have the number of row requested");
        return;
    }
    InterFilas(x->cm2,n[0],n[1]);
    output (x);
    return;
}

void cm_trans_t(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "left inlet doesn't accept pointers");
        return;
    }
    if(argc > 1){
        object_error((t_object*)x, "extra arguments for message t");
        return;
    }
    if(argc==1) {
        if(atom_gettype(argv) != A_LONG && atom_gettype(argv) != A_FLOAT) {
            object_error((t_object *)x, "bad arguments for message t");
            return;
        }
        x->n = atom_getlong(argv);
    } else {
        x->n=0; // default 0
    }
    
    CopiaMatriz(x->cm1,x->cm2);
    cm_tto(x->cm2, T_O, x->n);
    
    output (x);
    return;
}

void cm_trans_i(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "left inlet doesn't accept pointers");
        return;
    }
    if(argc > 1){
        object_error((t_object*)x, "extra arguments for message i");
        return;
    }
    if(argc==1) {
        if(atom_gettype(argv) != A_LONG && atom_gettype(argv) != A_FLOAT) {
            object_error((t_object *)x, "bad arguments for message i");
            return;
        }
        x->n = atom_getlong(argv);
    } else {
        x->n=0; // default 0
    }
    
    CopiaMatriz(x->cm1,x->cm2);
    cm_tto(x->cm2, IT_O, x->n);
    
    output (x);
    return;
}

void cm_trans_m(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "left inlet doesn't accept pointers");
        return;
    }
    if(argc > 1){
        object_error((t_object*)x, "extra arguments for message m");
        return;
    }
    if(argc==1) {
        if(atom_gettype(argv) != A_LONG && atom_gettype(argv) != A_FLOAT) {
            object_error((t_object *)x, "bad arguments for message m");
            return;
        }
        x->n = atom_getlong(argv);
    } else {
        x->n=0; // default 0
    }
    
    CopiaMatriz(x->cm1,x->cm2);
    cm_tto(x->cm2, MT_O, x->n);
    
    output (x);
    return;
}
void cm_trans_mi(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "left inlet doesn't accept pointers");
        return;
    }
    if(argc > 1){
        object_error((t_object*)x, "extra arguments for message mi");
        return;
    }
    if(argc==1) {
        if(atom_gettype(argv) != A_LONG && atom_gettype(argv) != A_FLOAT) {
            object_error((t_object *)x, "bad arguments for message mi");
            return;
        }
        x->n = atom_getlong(argv);
    } else {
        x->n=0; // default 0
    }
    
    CopiaMatriz(x->cm1,x->cm2);
    cm_tto(x->cm2, IMT_O, x->n);
    
    output (x);
    return;
}

void cm_trans_r90(t_cm_trans *x) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "left inlet doesn't accept pointers");
        return;
    }
    
    CopiaMatriz(x->cm1,x->cm2);
        // 90 degrees rotation
    Rota90(x->cm2);
    
    output (x);
    return;
}

void cm_trans_rd(t_cm_trans *x, t_symbol *s, long n) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "left inlet doesn't accept pointers");
        return;
    }
    if(x->cm2->NroFilas != x->cm2->NroCols){
        object_warn((t_object*)x, "need a square CM for this operation");
        return;
    }
    
    x->n = n;
    CopiaMatriz(x->cm1,x->cm2);
    RotaDiag(x->cm2, x->n);
    
    output (x);
    return;
}

void cm_trans_swap(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv) {   // swapping
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "left inlet doesn't accept pointers");
        return;
    }
    
    t_int i;
    t_int flag = FALSE, tipo = 0, nop=0;
    t_int j, z, w, h, c, d;
    int dens[MAXROWS][MAXROWS];
    PCS p1, p2, target;
    
    CopiaMatriz(x->cm1,x->cm2);
    x->n=0.;
    
    if(argc==0){            // we get the swap message with no arguments
        if(Swap(x->cm2)==FALSE) {   // perform all the possible swapping operations but outputs just the final result
            object_warn((t_object*)x, "could not reduce the position density of this CM");
            return;
        }
        output (x);
        return;
    }
    
    //- ...
    if(argc==1) {           // we get the swap message with one argument
        if(atom_gettype(argv) != A_LONG && atom_gettype(argv) != A_FLOAT) {
            object_error((t_object*)x, "bad arguments for message 'swap'");      // mostramos un mensaje de error si recibimos no longs o floats
            return;
        }
                            // perform the requested number of swapping operations and outputs all the results
        x->n = atom_getlong(argv);
        
        for(h = 0; h < 2; h++){
            if(h && flag==FALSE) tipo = TRUE;
            for(z = 0; z < x->cm2->NroFilas; z++){
                for(w = 0; w < NEXTC * x->cm2->NroCols; w++){
                    if(x->cm2->mat[w][z] != SPAC && x->cm2->mat[w][z] != ETY){
                        for(j = 0; j < x->cm2->NroFilas; j++){
                            for(i = 0; i < NEXTC * x->cm2->NroCols; i++){
                                if(x->cm2->mat[i][j] != SPAC && x->cm2->mat[i][j] != ETY){
                                    if(i != w && j != z){
                                        if(x->cm2->mat[i][j] == x->cm2->mat[w][z]){
                                            Densidad(x->cm2, dens);
                                            c = (int)(w/NEXTC);
                                            d = (int)(i/NEXTC);
                                            if(((dens[c][j] + tipo) < dens[c][z]) && ((dens[d][z] + tipo) < dens[d][j])){
                                                flag = TRUE;
                                                
                                                Insertar(x->cm2, j, c, x->cm2->mat[i][j]);
                                                Borrar(x->cm2, z, c,  x->cm2->mat[i][j]);
                                                Insertar(x->cm2, z, d, x->cm2->mat[i][j]);
                                                Borrar(x->cm2, j, d,  x->cm2->mat[i][j]);
                                                
                                                if(nop++ < (int)x->n)
                                                    output(x);
                                                else
                                                    break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if(flag==FALSE) {
            object_warn((t_object*)x, "could not reduce the position density of this CM");
            return;
        }
        else
            return;
        
    }
    //- ...
    if(argc==4) {           // we get the swap message with two arguments
        for(int n=0; n<4; n++) {
            if(atom_gettype(argv+n) != A_LONG && atom_gettype(argv) != A_FLOAT) {
                object_error((t_object*)x, "wrong arguments for message 'swap'");      // mostramos un mensaje de error si recibimos no longs o floats
                break;
                return;
            }
        }
                            // swap two specific positions of a CM without density checking
        for(int i=0; i<2; i++) {
            x->row[i] = atom_getlong(argv + i*2);
            x->col[i] = atom_getlong(argv + (i*2+1));
            if(x->row[i] > x->cm2->NroFilas-1 || x->col[i] > x->cm2->NroCols-1) {
                object_warn((t_object*)x, "out of range Row and/or Column number, no action taken");
                return;
            }
        }
        pos_to_pcs(x->cm2, &p1, x->row[0], x->col[0]);
        pos_to_pcs(x->cm2, &p2, x->row[1], x->col[1]);
        find_intersec(&p1, &p2,&target);
        if(target.find[0]==EOC) {
            object_warn((t_object*)x, "can't swap positions, no common set betweem them");
            return;
        }
        for(i=0; i<PCSL; ++i) {
            if(target.find[i]==EOC) break;
            Borrar(x->cm2, x->row[0], x->col[0], target.find[i]);
            Insertar(x->cm2, x->row[1], x->col[0], target.find[i]);
            Borrar(x->cm2, x->row[1], x->col[1], target.find[i]);
            Insertar(x->cm2, x->row[0], x->col[1], target.find[i]);
        }
        output (x);
        return;
    }
    
    else {              // we get the swap message with three or more than four arguments
        object_error((t_object*)x, "missing or extra arguments for message 'swap'");
    }
}

void cm_trans_del(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    t_int i;
    
    if(inum !=0) {
        object_error((t_object*)x, "left inlet doesn't accept pointers");
        return;
    }
    
    if(argc !=2) {
        object_error((t_object*)x, "missing or extra arguments for message 'del'");
        return;
    }
    for (i=0; i<2; i++) {
        if (atom_gettype(argv+i) != A_LONG && atom_gettype(argv+i) != A_FLOAT) {
            object_error((t_object *)x, "bad arguments for message ec");
            break;
            return;
        }
    }
    
    CopiaMatriz(x->cm1,x->cm2);
    x->row[0]= atom_getlong(argv);
    x->col[0]= atom_getlong(argv+1);
    
    if(x->row[0] > x->cm2->NroFilas || x->col[0] > x->cm2->NroCols) {
        object_warn((t_object*)x, "out of range Row and/or Column number, no action taken");
        return;
    }
    
    empty_pos(x->cm2, x->row[0], x->col[0]);
    
    output (x);
    return;
}

void cm_trans_wri(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    t_int i;
    
    if(inum !=0) {
        object_error((t_object*)x, "left inlet doesn't accept pointers");
        return;
    }
    
    if(argc !=2) {
        object_error((t_object*)x, "missing or extra arguments for message 'wri'");
        return;
    }
    for (i=0; i<2; i++) {
        if (atom_gettype(argv+i) != A_LONG && atom_gettype(argv+i) != A_FLOAT) {
            object_error((t_object *)x, "bad arguments for message ec");
            break;
            return;
        }
    }
    
    CopiaMatriz(x->cm1,x->cm2);
    x->row[0]= atom_getlong(argv);
    x->col[0]= atom_getlong(argv+1);
    
    if(x->row[0] > x->cm2->NroFilas || x->col[0] > x->cm2->NroCols) {
        object_warn((t_object*)x, "out of range Row and/or Column number, no action taken");
        return;
    }
    
    if(x->pcs->find[0]==EOC){
        object_warn((t_object*)x, "no PCS delivered to write, no action taken");
        return;
    }
    empty_pos(x->cm2, x->row[0], x->col[0]);
    pcs_to_pos(x->cm2, x->pcs, x->row[0], x->col[0]);
    
    output (x);
    return;
}

void cm_trans_add(t_cm_trans *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    t_int i;
    
    if(inum !=0) {
        object_error((t_object*)x, "left inlet doesn't accept pointers");
        return;
    }
    
    if(argc !=2) {
        object_error((t_object*)x, "missing or extra arguments for message 'add'");
        return;
    }
    for (i=0; i<2; i++) {
        if (atom_gettype(argv+i) != A_LONG && atom_gettype(argv+i) != A_FLOAT) {
            object_error((t_object *)x, "bad arguments for message ec");
            break;
            return;
        }
    }
    
    CopiaMatriz(x->cm1,x->cm2);
    x->row[0]= atom_getlong(argv);
    x->col[0]= atom_getlong(argv+1);
    
    if(x->row[0] > x->cm2->NroFilas || x->col[0] > x->cm2->NroCols) {
        object_warn((t_object*)x, "out of range Row and/or Column number, no action taken");
        return;
    }
    
    if(x->pcs->find[0]==EOC){
        object_warn((t_object*)x, "no PCS delivered to add, no action taken");
        return;
    }
    for(i=0; i<PCSL; ++i) {
        if(x->pcs->find[i]==EOC) break;
        if(x->pcs->find[i]!=EOP) {
            Insertar(x->cm2, x->row[0], x->col[0],x->pcs->find[i]);
        }
    }
    
    output (x);
    return;
}

void output (t_cm_trans *x) {
    {   //------------- out cm ptr -------------------
        char pstr[STRLP];
        t_atom ptr;
        sprintf(pstr, "%p", x->cm2);    //convert pointer to CM struct into symbol //- (cuidado con el nombre de la struct)
        atom_setsym(&ptr, gensym(pstr));
        outlet_anything (x->cm_out, gensym(MMID), 1, &ptr);    //- (cuidado con el nombre del outlet)
    }   //------------- end out -------------------
}

void cm_trans_free(t_cm_trans *x){
    
    if(x->cm1 != NULL){
        free(x->cm1);
    }
    if(x->cm2 != NULL){
        free(x->cm2);
    }
    if(x->pcs != NULL){
        free(x->pcs);
    }
    return;
}
