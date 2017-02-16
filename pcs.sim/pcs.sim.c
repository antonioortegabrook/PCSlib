/*  pcs_sim object  by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
-------
 The pcs_sim2 object evaluates the similarity degree of two or more PCS according to several criteria.
 
 Input:
 
 Rightmost inlet: a series of PCS to be mutually compared(at least must be two). The PCS
 are accumulated and mutually compared until a "reset" message is sent via the leftmost inlet.
 
 Leftmost inlet: there are several possibilities of messages that can be grouped into three types:
 
 1-The "reset" message clears the list of PCS stored, if any.
 
 2-Any of these symbols will cause the output of the result of the following similarities:
 -"r0" "r1" "r2" or "rp" symbols:  Forte's similarities. (only for different SC of the same
	cardinality which must be >3 and <7).
	-"ICVSIM"  symbol: Isaacson's similarity. (no constraints)
	-"SIM" or "ASIM" symbols: Morris's similarities. (no constraints)
	-"OI" symbol: Babbitt order inversions. (only for different permutations of the same PCS)
	-"DIS" symbol: Morris's Displacement measure.(only for different permutations of the same PCS)
	-"SCAT" symbol: Morris's Scattering measure.(only for different permutations of the same PCS)
	-"CC" symbol: Correlation coefficient.(only for different permutations of the same PCS)
	-"OPSC"symbol, w float: Ordered PCS Similarity Coefficient. Experimentally designed by Pablo Di Liscia.(no constraints,
 but note that this measurement weight both, the common pitch-classes shared and the similarity
 in adjacent PCS, is specially suited for ORDERED PCS). This measuring varies from 0(minimal similarity)
 to 1(maximal similarity). w is a weighting coefficient between Pitch Similarity and Intervalic Similarity.
 w may vary between 0 and 1. w=1 takes in account Intervalic Similarity only, whilst w=0 takes
 in account Pitch similarity only, w=0.5 weights both equally, and so on.
 
 3-The "write" message direct the output of the comparison to a raw text file.
 This message must be followed by a file name AND one of the symbols already detailed.
 Example: "write similarities.txt r0" will produce the r0 similarity matrix to be written in
 the file "similarities.txt".
 
 Output:
 
 Leftmost outlet:
 The output is a series of lists which resembles the ordering of a triangular
 comparison matrix. Here is an example of such matrix:
 Being (A), (B), (C), (D) and (E) PCS delivered in this order, the output will be:
 
 (B)		(C)		(D)		(E)
 -----------------------------------
 (A) |	ab		ac		ad		ae	<--(list 1 of length N-1)
	|
 (B)	|			bc		bd		be	<--(list 2 of length N-2)
	|
 (C)	|					cd		ce	<--(list 3 of length N-3)
 |
 (D)	|							de	<--(list 4 of length N-4)
 
 So, for example, to know the result of the comparison between (B) and (C) you must read
 the content of the position which is the intersection of the row labeled (B) with
 the column labeled (C), which is filled, in this case, by "bc", but actually will be filled
 by a number or a symbol.
 The example shown above is of a series of 5 PCS, so, the output will be a series of FOUR lists, each one
 being of size N-n, being N the number of PCS compared and n the number of list of the output.
 The content of each "cell" is as follows:
 Forte's similarities: A list of floats. "1" will means that the relation stands while "0" will means
 that the relation does not stands. a -100 will indicate that the PCS to be compared did not meet
 the needed constraints for the relation and so, are not comparable.
 Other relations: a list of float values. In the case of the OI, DIS, SCAT and CC coefficients, a
 -100 value will mean that the PCS to be compared did not meet the needed constraints for the
 relation and so, are not comparable.
 
 Rightmost outlet: in the case of Forte's relations, no output. Otherwise, a list of two floats
 being the first one the maximal value and the second the minimal value found out of all the values
 obtained in the comparison.
 
 In the case of the "write" message there will not be output from the outlets and the data written
 in the file will have the same format explained, except that the value of -100 is replaced with
 a "(NC)" (meaning "not comparable") and in the case of Forte's relations "+" or "-" are used instead of
 "0" or "1". Forte's rp relation may include more than one "+" since there may be more than one subset
 shared between two SC.

*/
/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS
 
        Borrar variables no utilizadas...?
*/

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

#include "../pcslib-includes/pcslib_max.c"       //    PCSlib header

typedef struct _pcs_sim {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    PCS* pcs;
    SIM **ms;
    float ranks[8][2];
    float w; /*weighting coefficient for OPSC*/
    float index;                                //- ex t_float
    t_int npcs;
    t_int j; /* the jth col of the similarity matrix*/
    
    long p_in;
    void *pcs_in;
    void *sim_out;
    void *ranks_out;
} t_pcs_sim;

#define max_sim_r	100
#define max_sim_c	100
#define max_sim_n	4950 //((n*n)-n)/2

// these are prototypes for the methods that are defined below
void pcs_sim_pcs_ptr(t_pcs_sim *x, t_symbol *s, long argc, t_atom *argv);

void pcs_sim_rp(t_pcs_sim *x);
void pcs_sim_r0(t_pcs_sim *x);
void pcs_sim_r1(t_pcs_sim *x);
void pcs_sim_r2(t_pcs_sim *x);
void pcs_sim_sim(t_pcs_sim *);
void pcs_sim_asim(t_pcs_sim *x);
void pcs_sim_icvsim(t_pcs_sim *x);
void pcs_sim_oi(t_pcs_sim *x);
void pcs_sim_dis(t_pcs_sim *x);
void pcs_sim_scat(t_pcs_sim *x);
void pcs_sim_cc(t_pcs_sim *x);
void pcs_sim_opsc(t_pcs_sim *x, double f);
void pcs_sim_reset(t_pcs_sim *x);

void pcs_sim_assist(t_pcs_sim *x, void *b, long m, long a, char *s);
void pcs_sim_free(t_pcs_sim *x);
void *pcs_sim_new();

void pcs_sim_analyze(t_pcs_sim *x);
void pcs_sim_output(t_pcs_sim *x, int what);

int check_npcs(t_pcs_sim *x);

static t_class *pcs_sim_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

    c = class_new("pcs.sim", (method)pcs_sim_new, (method)pcs_sim_free, sizeof(t_pcs_sim), 0L, 0);
    
    class_addmethod(c, (method)pcs_sim_pcs_ptr,         MPID,		A_GIMME, 0);    // pointer to PCS
    class_addmethod(c, (method)pcs_sim_rp,              "rp",                0);
    class_addmethod(c, (method)pcs_sim_r0,              "r0",                0);
    class_addmethod(c, (method)pcs_sim_r1,              "r1",                0);
    class_addmethod(c, (method)pcs_sim_r2,              "r2",                0);
    class_addmethod(c, (method)pcs_sim_sim,             "sim",               0);
    class_addmethod(c, (method)pcs_sim_asim,            "asim",              0);
    class_addmethod(c, (method)pcs_sim_icvsim,          "icvsim",            0);
    class_addmethod(c, (method)pcs_sim_oi,              "oi",                0);
    class_addmethod(c, (method)pcs_sim_dis,             "dis",               0);
    class_addmethod(c, (method)pcs_sim_scat,            "scat",              0);
    class_addmethod(c, (method)pcs_sim_cc,              "cc",                0);
    class_addmethod(c, (method)pcs_sim_opsc,            "opsc",     A_FLOAT, 0);
    class_addmethod(c, (method)pcs_sim_reset,           "reset",             0);
    
	class_addmethod(c, (method)pcs_sim_assist,         "assist",	A_CANT, 0);	// (optional) assistance method
    class_addmethod(c, (method)stdinletinfo,           "inletinfo",   A_CANT,  0);

	class_register(CLASS_BOX, c);
	pcs_sim_class = c;
}

//--------------------------------------------------------------------------

void *pcs_sim_new() {
	t_pcs_sim *x;                                             // local variable (pointer to a t_pcs_sim data structure)
    x = (t_pcs_sim *)object_alloc(pcs_sim_class);           // create a new instance of this object

    int i;
    
    x->npcs=0;
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs[0].find[0]=EOC;
    x->j=0;
    
    x->ms=NULL;
    x->ms=(SIM**)malloc(sizeof(SIM*)*max_sim_c);
    for(i=0; i<max_sim_c; ++i) {
        x->ms[i]=(SIM*)malloc(sizeof(SIM)*max_sim_r-i);
    }
    
    /*initialize max and min values counter*/
    for(i=0; i<NRELS-4; ++i) {
        x->ranks[i][0]=0.;
        x->ranks[i][1]=1000.;
    }
    
    x->pcs_in = proxy_new(x, 1, &x->p_in);
    x->ranks_out=listout(x);                                 // create outlets
    x->sim_out=listout(x);
    
	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void pcs_sim_assist(t_pcs_sim *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET) {
        switch(a) {
            case(0):
                sprintf(s,"Relation to compute, 'reset' clears the PCS list");
                break;
            case(1):
                sprintf(s,"PCS");
                break;
        }
    }
    if (m == ASSIST_OUTLET) {
        switch(a) {
            case(0):
                sprintf(s,"Series of lists with the values found");
                break;
            case(1):
                sprintf(s,"Maximal and minimal value found (list)");
                break;
        }
    }
}

void pcs_sim_free(t_pcs_sim *x){
    if(x->pcs != NULL) free(x->pcs);
    for(int i=0; i< max_sim_c; i++) {
        free(x->ms[i]);
    }
    free(x->ms);
    return;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------msgs
void pcs_sim_pcs_ptr(t_pcs_sim *x, t_symbol *s, long argc, t_atom *argv) {      //- get PCS
    long inum = proxy_getinlet((t_object *)x);
    
    if(inum !=1) {
        object_error((t_object*)x, "left inlet doesn't accept pointers");
        return;
    }
    
    if(x->npcs+1 >= max_sim_c) {
        object_warn((t_object*)x, "maximal number of pcs input (%d) pcs reached, pcs rejected", max_sim_c);
        return;
    }
    
    {  //------------ get ptr --------------
        t_symbol *temp;
        PCS *tempcs;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
        x->npcs++;
        x->pcs=(PCS*)realloc(x->pcs,x->npcs*sizeof(PCS));
        CopiaSet(tempcs,&x->pcs[x->npcs-1]);      //- (cuidado con el nombre de la estructura)
    }  //------------ end get --------------

    
    if(x->npcs > 1) {
        pcs_sim_analyze(x);
    }
    return;
}

void pcs_sim_rp(t_pcs_sim *x) {             // rp
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "that's not a valid pointer");
        return;
    }
    
    if(check_npcs(x) == 1)
        pcs_sim_output(x, Rrp);
    return;
}

void pcs_sim_r0(t_pcs_sim *x) {             // r0
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "that's not a valid pointer");
        return;
    }
    
    if(check_npcs(x) == 1)
        pcs_sim_output(x, Rr0);
    return;
}

void pcs_sim_r1(t_pcs_sim *x) {         // r1
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "that's not a valid pointer");
        return;
    }
    
    if(check_npcs(x) == 1)
        pcs_sim_output(x, Rr1);
    return;
}

void pcs_sim_r2(t_pcs_sim *x) {         // r2
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "that's not a valid pointer");
        return;
    }
    
    if(check_npcs(x) == 1)
        pcs_sim_output(x, Rr2);
    return;
}

void pcs_sim_sim(t_pcs_sim *x) {        // sim
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "that's not a valid pointer");
        return;
    }
    
    if(check_npcs(x) == 1)
        pcs_sim_output(x, Rsim);
    return;
}

void pcs_sim_asim(t_pcs_sim *x) {       // asim
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "that's not a valid pointer");
        return;
    }
    
    if(check_npcs(x) == 1)
        pcs_sim_output(x, Rasim);
    return;
}

void pcs_sim_icvsim(t_pcs_sim *x) {     // ricvsim
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "that's not a valid pointer");
        return;
    }
    
    if(check_npcs(x) == 1)
        pcs_sim_output(x, Ricvsim);
    return;
}

void pcs_sim_oi(t_pcs_sim *x) {     // oi
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "that's not a valid pointer");
        return;
    }
    
    if(check_npcs(x) == 1)
        pcs_sim_output(x, Roi);
    return;
}

void pcs_sim_dis(t_pcs_sim *x) {        // dis
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "that's not a valid pointer");
        return;
    }
    
    if(check_npcs(x) == 1)
        pcs_sim_output(x, Rdis);
    return;
}

void pcs_sim_scat(t_pcs_sim *x) {       // scat
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "that's not a valid pointer");
        return;
    }
    
    if(check_npcs(x) == 1)
        pcs_sim_output(x, Rscat);
    return;
}

void pcs_sim_cc(t_pcs_sim *x) {         // cc
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "that's not a valid pointer");
        return;
    }
    
    if(check_npcs(x) == 1)
        pcs_sim_output(x, Rcc);
    return;
}

void pcs_sim_opsc(t_pcs_sim *x, double f) {     // opsc
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "that's not a valid pointer");
        return;
    }
    
    x->w = (float)f;
    
    if(x->w < 0. || x->w > 1.) {
        object_warn((t_object*)x, "out of range weighting coefficient (must be equal or greater than 0. and less than 1.");
        return;
    }
    x->ranks[7][0]=0.;
    x->ranks[7][1]=1000.;
    
    if(check_npcs(x) == 1)
        pcs_sim_output(x, Ropsc);
    
    return;
}

void pcs_sim_reset(t_pcs_sim *x) {      //reset the PCS buffer
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "that's not a valid pointer");
        return;
    }
    
    x->npcs=0;
    x->pcs=(PCS*)realloc(x->pcs,sizeof(PCS));
    x->pcs[0].find[0]=EOC;
    x->j=0;
    //initialize maxs and min values counter
    for(int i=0; i<NRELS-4; ++i) {
        x->ranks[i][0]=0.;
        x->ranks[i][1]=1000.;
    }
    return;
}

//-------------------------------------------

int check_npcs(t_pcs_sim *x) {
    if(x->npcs < 2) {
        object_warn((t_object*)x, "need at least two PCS to compare");
        return 0;
    }
    return 1;
}

void pcs_sim_analyze(t_pcs_sim *x) {
    
    t_int i /*,n*/ ;                                                  //- unused variable 'n'
    /*Do and store analysis on a similarity matrix*/
    /*see above the graphics explanation on the similarity matrix structure*/
    
    for(i=0; i<x->npcs-1; ++i) {
        /*for Forte relations, PCS must meet some constraints if they don't, the result is set to NC*/
        if(x->pcs[i].ncar != x->pcs[x->j+1].ncar || x->pcs[i].nord == x->pcs[x->j+1].nord || x->pcs[i].ncar > 8 || x->pcs[x->j+1].ncar > 8 || x->pcs[i].ncar < 4 || x->pcs[x->j+1].ncar < 4) {
            x->ms[i][x->j].r0=NC;
            x->ms[i][x->j].r1=NC;
            x->ms[i][x->j].r2=NC;
            x->ms[i][x->j].rp[0]=NC;
        }
        /*do Forte analysis*/
        else{
            x->ms[i][x->j].r0=fr0(x->pcs[i].icvmat,x->pcs[x->j+1].icvmat);
            x->ms[i][x->j].r1=fr1(x->pcs[i].icvmat,x->pcs[x->j+1].icvmat);
            x->ms[i][x->j].r2=fr2(x->pcs[i].icvmat,x->pcs[x->j+1].icvmat);
            frp(&x->pcs[i],&x->pcs[x->j+1],x->ms[i][x->j].rp);
        }
        /*do other analysis and store max. and min. values in this case*/
        x->ms[i][x->j].sim=sim(x->pcs[i].icvmat,x->pcs[x->j+1].icvmat);
        x->ranks[0][0]=x->ranks[0][0] >= x->ms[i][x->j].sim? x->ranks[0][0] : x->ms[i][x->j].sim;
        x->ranks[0][1]=x->ranks[0][1] <= x->ms[i][x->j].sim? x->ranks[0][1] : x->ms[i][x->j].sim;
        
        x->ms[i][x->j].asim=asim(x->pcs[i].icvmat,x->pcs[x->j+1].icvmat);
        x->ranks[1][0]=x->ranks[1][0] >= x->ms[i][x->j].asim? x->ranks[1][0] : x->ms[i][x->j].asim;
        x->ranks[1][1]=x->ranks[1][1] <= x->ms[i][x->j].asim? x->ranks[1][1] : x->ms[i][x->j].asim;
        
        
        x->ms[i][x->j].icvsim=icvsim(x->pcs[i].icvmat,x->pcs[x->j+1].icvmat);
        x->ranks[2][0]=x->ranks[2][0] >= x->ms[i][x->j].icvsim? x->ranks[2][0] : x->ms[i][x->j].icvsim;
        x->ranks[2][1]=x->ranks[2][1] <= x->ms[i][x->j].icvsim? x->ranks[2][1] : x->ms[i][x->j].icvsim;
        
        roi_dis_scat_cc(&x->pcs[i],&x->pcs[x->j+1],&x->ms[i][x->j]);
        x->ranks[3][0]=x->ranks[3][0] >= (float)x->ms[i][x->j].oi?  x->ranks[3][0] : (float)x->ms[i][x->j].oi;
        x->ranks[3][1]=x->ranks[3][1] <= (float)x->ms[i][x->j].oi?  x->ranks[3][1] : (float)x->ms[i][x->j].oi;
        x->ranks[4][0]=x->ranks[4][0] >= (float)x->ms[i][x->j].dis? x->ranks[4][0] : (float)x->ms[i][x->j].dis;
        x->ranks[4][1]=x->ranks[4][1] <= (float)x->ms[i][x->j].dis? x->ranks[4][1] : (float)x->ms[i][x->j].dis;
        x->ranks[5][0]=x->ranks[5][0] >= (float)x->ms[i][x->j].scat?x->ranks[5][0] : (float)x->ms[i][x->j].scat;
        x->ranks[5][1]=x->ranks[5][1] <= (float)x->ms[i][x->j].scat?x->ranks[5][1] : (float)x->ms[i][x->j].scat;
        x->ranks[6][0]=x->ranks[6][0] >= x->ms[i][x->j].cc?  x->ranks[6][0] : x->ms[i][x->j].cc;
        x->ranks[6][1]=x->ranks[6][1] <= x->ms[i][x->j].cc?  x->ranks[6][1] : x->ms[i][x->j].cc;
        
        opcss(&x->pcs[i],&x->pcs[x->j+1],&x->ms[i][x->j].opcss[0],&x->ms[i][x->j].opcss[1]);
        
        /**/
        
    }
    
    x->j++; //update the column number
    
    return;
}

void pcs_sim_output(t_pcs_sim *x, int what) {
    
    t_atom *sarray;
    t_atom rarray[2];
    float aux;
    int i,j,cnt;
    
    sarray=(t_atom*)malloc(sizeof(t_atom)*x->npcs);
    
    /*outputs max. and min. values, if addequate*/
    if(what > 4 ) {
        for(i=5; i <= NRELS; ++i) {
            if(what==i) {
                atom_setfloat(&(rarray[0]),(float)x->ranks[i-5][0]);
                atom_setfloat(&(rarray[1]),(float)x->ranks[i-5][1]);
                break;
            }
        }
        outlet_list(x->ranks_out, gensym("list"),2,rarray);
    }
    /*outputs evaluation matrix*/
    for(i=0; i<x->npcs-1; ++i) {
        cnt=0;
        for(j=i; j<x->npcs-1; ++j) {
            switch(what) {
                case Rr0:
                    aux=(float)x->ms[i][j].r0;
                    break;
                case Rr1:
                    aux=(float)x->ms[i][j].r1;
                    break;
                case Rr2:
                    aux=(float)x->ms[i][j].r2;
                    break;
                case Rrp:
                    if(x->ms[i][j].rp[0] !=0) {
                        if(x->ms[i][j].rp[0] ==NC)
                            aux=-100.;
                        else
                            aux=1.;
                    }
                    else
                        aux=0.;
                    break;
                case Rsim:
                    aux=(float)x->ms[i][j].sim;
                    break;
                case Rasim:
                    aux=(float)x->ms[i][j].asim;
                    break;
                case Ricvsim:
                    aux=(float)x->ms[i][j].icvsim;
                    break;
                case Roi:
                    aux=(float)x->ms[i][j].oi;
                    break;
                case Rdis:
                    aux=(float)x->ms[i][j].dis;
                    break;
                case Rscat:
                    aux=(float)x->ms[i][j].scat;
                    break;
                case Rcc:
                    aux=(float)x->ms[i][j].cc;
                    break;
                case Ropsc:
                    aux=x->ms[i][j].opcss[0]*(1.-x->w) + x->ms[i][j].opcss[1]*(x->w);
                    /*evaluation of min-max must be done "on the fly" in this special case*/
                    x->ranks[7][0]=x->ranks[7][0] >= aux? x->ranks[7][0] : aux;	
                    x->ranks[7][1]=x->ranks[7][1] <= aux? x->ranks[7][1] : aux;
                    break;			
            }
            atom_setfloat(&(sarray[cnt]),aux);
            cnt++;
        }
        outlet_list(x->sim_out, gensym("list"), cnt, sarray);
    }
    
    free(sarray);
    return;
}
