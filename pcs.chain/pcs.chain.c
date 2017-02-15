/*  pcs_chain object  by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
                                                Requires Max6/7 & OS X 10.7 or later
                                                September 2016
-------
 pcs_chain is designed to create a chain of PCSs with a coherent norm, according
 the method proposed by Robert Morris (Combinatorialty without the aggregate,
 Perspectives of new Music, USA., 1984).
 
 Input (from left to right):
 inlet #2: a PCS structure pointer which will be the norm of the chain.
 (cardinal number between 5 and 6 at present).
 If the chain is already started, there is a change in its norm.
 There may be any change of the norm of a chain, the user is responsible
 for its coherence. As an example, using a norm for the odd positions and
 a different one for the even positions of a chain may be used to produce
 a CM with one norm for its rows and a different one for its columns.
 
 inlet #1:
 -Either a "start n" or a "startr n" (n being the number of partition) message to
 select the first partition to be added in the chain. "startr n" stands for the partition
 in reverse order.
 -The "add n" (n being the number of partition requested to be added) message to add a
 partition to the chain.
 -The "reset" message to initialize the chain to zero size.
 
 Output (from right to left):
 outlet #5: the number of partition candidates that can be connected to enlarge the chain.
 outlet #4: A series of two lists with the partition candidates that can be connected
 to enlarge the chain.
 outlet #3: the size (number of PCS) of the chain.
 outlet #2: A list with the last PCS forming the chain.
 outlet #1: a bang indicating that the precedent four outputs was done. This bang is meant to
 be used as a trigger of iterations.
 
 Output is ALWAYS in right-wise order.
 
 pcs_chain is meant to be used in a kind of interactive fashion in order to produce
 a chain of increasing size each time it receives a certain kind of messages.
 
 The user may analyze the list of candidates available at each iteration and design any
 algorithm to make the selection of one according to some criteria. As an example,
 the candidate may be selected to saturate the aggregate as fast as possible, meanwhile
 selecting the one which have less PCs in common with the ones already forming
 part of the chain. Other possibility may be to exclude the partitions that have
 some specific PCS.
 The chain may be initializated to zero size by means of a "reset" message.
 
 Before the candidate list is sent out, pcs_chain evaluates and sort them according
 a score based on the following equation:
 
 
 chz-1
 --
 \
 /	DIST(pc[n], cand[i])
 --
 i=0
	score(cand[i])= ______________________________
 
 chz * #cand[i]
 
 
 where:
 chz			= the size (number of positions) of the chain.
 #cand		= the cardinal number of the candidate PCS to be added
 
 DIST(pc[n], cand[i]) = chz - ( pos(pc[n], cand[i]) + 2)
 and:
 
 pos(pc[n],cand[i]) is the last position in the chain at which the pc[n] of
 the candidate[i] was found (from 0 to chz-1).
 If not found, (pos([pc],cand[i])=0
 
 
 Example:
 
 chz=3
 
 Consider the following chain:
 
	2  4  |  0  1  5  |  4  3  |
 
 And the following candidates to be added with their scores:
 
	candidate[0]= (  5  1  0  )	score=0.00
	candidate[1]= (  B  0  1  )	score=0.33
	candidate[2]= (  2  0  B  )	score=0.44
	candidate[3]= (  5  7  8  )	score=0.66
	candidate[4]= (  2  6  7  )	score=0.77
	candidate[5]= (  8  7  6  )	score=1.00
 
 The score for the candidate 0 would be:
 
 [(3-3) + (3-3) + (3-3)] / (3*3) = 0
 
 The score for the candidate 1 would be:
 
 [(3-0) + (3-3) + (3-3)] / (3*3) = .33
 
 The score for the candidate 2 would be:
 
 [(3-2) + (3-3) + (3-0)] / (3*3) = .44
 
 The score for the candidate 3 would be:
 
 [(3-3) + (3-0) + (3-0)] / (3*3) = .66
 
 The score for the candidate 4 would be:
 
 [(3-2) + (3-0) + (3-0)] / (3*3) = .77
 
 The score for the candidate 5 would be:
 
 [(3-0) + (3-0) + (3-0)] / (3*3) = 1
 
 */

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS
 
        Borrar variables no utilizadas...?
*/

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

#include "../pcslib-includes/pcslib_max.c"

typedef struct _pcs_chain {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    PCS *pcs; /*pointer to PCS struct1*/
    PCS *ch ; /*pointer to a chain(a row of PCS)*/
    PART  *part;
    
    int chn; /*chain size*/
    int n;   /*number of partition requested to chain*/
    int prev;/*the previous number of partition chained*/
    int max; /*max. partition number allowed according to the cardinal of the norm */
    int min; /*min. partition number allowed according to the cardinal of the norm */
    CAND  candidates[100]; /*must be allocate dinamically?*/
    int   nca; /*the amount of candidates*/
    int	reverse; /*the "reverse" flag*/
    int   verbose; /*the verbose flag*/
    int pcs_received;
    
    //- ptr inlet
    long p_in;
    void *pcs_in;
    //- outlets
    void *ch_out; /*chain output*/
    void *chz_out;/*chain size output*/
    void *cand_out;/*candidates output*/
    void *nc_out;/*number of candidates output*/
    void *done_out; /*bang when all the precedent outputs are done*/
} t_pcs_chain;

// these are prototypes for the methods that are defined below
        //--- messages to pcs_chain
void pcs_chain_start(t_pcs_chain *x, int n);          //- start $1
void pcs_chain_startr(t_pcs_chain *x, int n);          //- startr $1
void pcs_chain_add(t_pcs_chain *x, int n);          //- add $1
void pcs_chain_reset(t_pcs_chain *x);                   //- reset
void pcs_chain_verbose(t_pcs_chain *x, int n);          //- verbose $1
void get_chain_norm(t_pcs_chain *x, t_symbol *s, int argc, t_atom *argv);   //- list pcs pointer

void pcs_chain_add_part(t_pcs_chain *x, int which, int r);
void pcs_chain_start_chain(t_pcs_chain *x, int r);
void pcs_chain_get_candidates(t_pcs_chain *x);
void pcs_chain_output(t_pcs_chain *x);
void pcs_cand_output(t_pcs_chain *x);
void pcs_chain_eval(t_pcs_chain *x);

void pcs_chain_free(t_pcs_chain *x);

void pcs_chain_assist(t_pcs_chain *x, void *b, long m, long a, char *s);              //- assist method
void stdinletinfo();

void *pcs_chain_new();

static t_class *pcs_chain_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("pcs.chain", (method)pcs_chain_new, (method)pcs_chain_free, sizeof(t_pcs_chain), 0L, 0);
    
    class_addmethod(c, (method)get_chain_norm,         MPID,           A_GIMME, 0);    //- pointer to PCS
    class_addmethod(c, (method)pcs_chain_start,        "start",        A_LONG,  0);
    class_addmethod(c, (method)pcs_chain_startr,       "startr",       A_LONG,  0);
    class_addmethod(c, (method)pcs_chain_add,          "add",          A_LONG,  0);
    class_addmethod(c, (method)pcs_chain_reset,        "reset",                 0);
    class_addmethod(c, (method)pcs_chain_verbose,      "verbose",      A_LONG,  0);
    
    class_addmethod(c, (method)pcs_chain_assist,       "assist",       A_CANT,  0);	  // (optional) assistance method
    class_addmethod(c, (method)stdinletinfo,           "inletinfo",    A_CANT,  0);    // method for hot and cold inlets
    
	class_register(CLASS_BOX, c);
	pcs_chain_class = c;
}

//--------------------------------------------------------------------------

void *pcs_chain_new() {
	t_pcs_chain *x;                                             // local variable (pointer to a t_pcs_chain data structure)

	x = (t_pcs_chain *)object_alloc(pcs_chain_class);           // create a new instance of this object

    x->pcs_in = proxy_new((t_object *)x, 1, &x->p_in);     //- second inlet (expects pointer to PCS)
    
    x->nc_out=intout(x);                                 // create outlets
    x->cand_out=listout(x);
    x->chz_out=intout(x);
    x->ch_out=listout(x);
    x->done_out=bangout(x);

    x->pcs=NULL;
    x->ch=NULL;
    
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs->find[0]=EOC;
    
    x->ch=(PCS*)malloc(sizeof(PCS)*2);
    x->chn=0;
    
    x->part=(PART*)malloc(sizeof(PART));
    x->nca=0;
    
    x->verbose=false;
    
    return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void pcs_chain_assist(t_pcs_chain *x, void *b, long m, long a, char *s) {
	if (m == ASSIST_INLET)
		sprintf(s,"PCS, start, add, reset");
    if (m == ASSIST_OUTLET) {
        switch(a) {
            case(0):
                sprintf(s,"Output done! (bang)");
                break;
            case(1):
                sprintf(s,"Last PCS added to chain (list)");
                break;
            case(2):
                sprintf(s,"Chain size (int)");
                break;
            case(3):
                sprintf(s,"Candidate partitions (succession of lists)");
                break;
            case(4):
                sprintf(s,"Number of candidates (int)");
                break;
        }
    }
}

void pcs_chain_free(t_pcs_chain *x){
    
    if(x->pcs != NULL){
        free(x->pcs);
    }
    if(x->ch != NULL){
        free(x->ch);
    }
    if(x->part != NULL){
        free(x->part);
    }
    return;
}

void get_chain_norm(t_pcs_chain *x, t_symbol *s, int argc, t_atom *argv) {        //- Get pointer to PCS & chain norm
    
    long inum = proxy_getinlet((t_object *)x);
    
    if(inum !=1) {
        object_error((t_object*)x, "left inlet doesn't accept pointers");
        return;
    }
    
    x->pcs_received=1;
    
      //------------ get ptr --------------
        t_symbol *temp;
        PCS *tempcs;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
    
        if(tempcs->ncar < 5 || tempcs->ncar > 6 ) {
            object_warn((t_object*)x, "error, allowed PCS must have cardinal 5 or 6");
            return;
        }
                            //if the size of the chain is !=0, the norm is changed during the generation of the chain
                            //this is a special case
        if(x->chn !=0) {
            if(tempcs->ncar != x->pcs->ncar ) {
                object_warn((t_object*)x,"chain already started, norm generators must be of the same cardinal number");
                return;
            }
        CopiaSet(tempcs, x->pcs);
        if(x->verbose==true) {
            object_post((t_object*)x, "chain norm changed, now is %d-%d", x->pcs->ncar, x->pcs->nord);
        }
      //------------ end get --------------

        Particiones(x->pcs,x->part,tableptr); /*make partitions again*/
        pcs_chain_get_candidates(x);		/*compute candidates*/
        pcs_chain_eval(x);				/*eval candidates for the next chain operation*/
        
        pcs_cand_output(x);			/*output candidates*/
        /*we don't have to output the chain in this case*/
        outlet_bang(x->done_out);		/*bang leftmost outlet when done*/
    }
    else {
        /*we start the chain from scratch*/
        if(x->verbose==true) {
            object_post((t_object*)x, "chain norm is %d-%d", tempcs->ncar, tempcs->nord);
        }
        CopiaSet(tempcs, x->pcs);           //- funciona ok
        get_min_max_pnum((int*)&x->min, (int*)&x->max, 0, x->pcs->ncar);
        
        Particiones(x->pcs,x->part,tableptr);
        
        /*output all the partitions in the first time*/
        pcs_cand_output(x);
        
        /*chain size equals 0 now*/
        outlet_int(x->chz_out,0);	/*output the size of the chain*/            //- <-- ex outlet_float
        outlet_bang(x->done_out);		/*bang leftmost outlet when done*/
    }
    return;
}

void pcs_chain_add_part(t_pcs_chain *x, int which, int r)
{
    PCS temp1, temp2;
    
    x->ch=(PCS*)realloc(x->ch,sizeof(PCS)*(x->chn+2));
    
    if(x->chn !=0) {
        
        if(r==false) {
            CopiaSet(&x->part->temp2[x->candidates[which].no],&temp2);
            TrInvPCS(&temp2,x->candidates[which].I,x->candidates[which].T);
            CopiaSet(&temp2, &x->ch[x->chn+1]);
        }
        else {
            CopiaSet(&x->part->temp1[x->candidates[which].no],&temp1);
            TrInvPCS(&temp1,x->candidates[which].I,x->candidates[which].T);
            CopiaSet(&temp1, &x->ch[x->chn+1]);
            
        }
    }
    
    else {
        if(r==false) {
            CopiaSet(&x->part->temp1[which], &x->ch[x->chn]);
            CopiaSet(&x->part->temp2[which], &x->ch[x->chn+1]);
        }
        else {
            CopiaSet(&x->part->temp2[which], &x->ch[x->chn]);
            CopiaSet(&x->part->temp1[which], &x->ch[x->chn+1]);
        }
    }
    
    x->chn++;
    
    return;
}

void pcs_chain_get_candidates(t_pcs_chain *x)
{
    int i /* , j */ ;               //- unused variable j
    int last=x->chn;
/*    PCS temp, temp2;  */          //- unused variabe temp, temp2
    
    
    x->nca=0;
    get_min_max_pnum((int*)&x->min,(int*)&x->max,x->ch[last].ncar,x->pcs->ncar);
    
    for(i=x->min; i<=x->max; ++i){
        if((NombresIguales(&x->part->temp1[i], &x->ch[last]))==true) {
            x->candidates[x->nca].no=i;
            x->candidates[x->nca].re=false;
            /*as there may be more than one candidate using the same partition and a different T operator,
             get_TI compute and store them in x->ch, increasing x->nca if needed*/
            get_TI(&x->part->temp1[i],&x->ch[last], x->candidates, &x->nca);
        }
        if((NombresIguales(&x->part->temp2[i], &x->ch[last]))==true ) {
            x->candidates[x->nca].no=i;
            x->candidates[x->nca].re=true;
            /*ditto, see above*/
            get_TI(&x->part->temp2[i],&x->ch[last], x->candidates,&x->nca);
        }
        
    }
    return;
}

void pcs_chain_output(t_pcs_chain *x)
{
    t_atom chain[PCSL];
    long aux;          //<-- ex float, cambiado por long
    int /* i, */ j;                                         //- unused variable i
    
    /*size goes out first*/
    outlet_int(x->chz_out,(float)(x->chn)+1.);                          //- <-- ex outlet_float
    /*now, output the chain*/
    if(x->chn==1) { /*first time is a special case*/
        for(j=0; j<x->ch[0].ncar; ++j){
            aux=(int)x->ch[0].find[j];
            atom_setlong(&(chain[j]),aux);
        }
        outlet_list(x->ch_out, gensym("list"),j,chain);
    }
    for(j=0; j<x->ch[x->chn].ncar; ++j){
        aux=(int)x->ch[x->chn].find[j];
        atom_setlong(&(chain[j]),aux);
    }
    outlet_list(x->ch_out, gensym("list"),j,chain);
    return;
}

void pcs_cand_output(t_pcs_chain *x)                                //- revisar esta función; ver bucles
{
    int i, j;                                                     //- t_int cambiado por int
    PCS temp1, temp2;
    t_atom set[PCSL];
    t_int aux;            //- <-- ex t_float cambiado por t_int
    
    if(x->chn == 0) {
        /*a new PCS as a norm was sent, output all the partitions*/
        /*size goes out first*/
        outlet_int(x->nc_out,(int)x->max+1);                              //- <-- ex outlet_float
        
        for(i=0; i<=x->max; ++i) {
            for(j=0; j < x->part->temp1[i].ncar; ++j) {
                aux=(int)x->part->temp1[i].find[j];
                atom_setlong(&(set[j]),aux);
            }
            outlet_list(x->cand_out, gensym("list"),j,set);         //
            
            for(j=0; j < x->part->temp2[i].ncar; ++j) {
                aux=(int)x->part->temp2[i].find[j];                         //- <-- ex (float)
                atom_setlong(&(set[j]),aux);
            }
            outlet_list(x->cand_out, gensym("list"),j,set);
        }
    }
    else {
        /*output the candidates only*/
        
        /*size goes out first*/
        outlet_int(x->nc_out,(int)x->nca);                                //- <-- ex outlet_float
        
        for(i=0; i<x->nca; ++i) {
            
            CopiaSet(&x->part->temp1[x->candidates[i].no],&temp1);
            TrInvPCS(&temp1,x->candidates[i].I,x->candidates[i].T);
            CopiaSet(&x->part->temp2[x->candidates[i].no],&temp2);
            TrInvPCS(&temp2,x->candidates[i].I,x->candidates[i].T);
            
            for(j=0; j<temp1.ncar; ++j) {
                aux=(int)temp1.find[j];
                atom_setlong(&(set[j]),aux);
            }
            outlet_list(x->cand_out, gensym("list"),j,set);
            for(j=0; j<temp2.ncar; ++j) {
                aux=(int)temp2.find[j];
                atom_setlong(&(set[j]),aux);
            }
            outlet_list(x->cand_out, gensym("list"),j,set);
        }
    }
    
    return;
}

void pcs_chain_eval(t_pcs_chain *x)
{
    int i,j,h,k;
    PCS temp;
    int position=0, pc /* , index */ ;              //- unused variable 'index'
    char *str;
    char *num;
    CAND tempc;
    int size=x->chn+1;
    float div;
    
    for(h=0; h< 100; ++h) {
        x->candidates[h].s=0.;
    }
    
    /*evaluate candidates and store their score here*/
    for(i=0; i<x->nca; ++i) {
        
        if(x->candidates[i].re==true) {
            CopiaSet(&x->part->temp1[x->candidates[i].no],&temp);
        }
        else{
            CopiaSet(&x->part->temp2[x->candidates[i].no],&temp);
        }
        TrInvPCS(&temp,x->candidates[i].I,x->candidates[i].T);
        div=((int)size * (int)temp.ncar);
        
        for(j=0; j<temp.ncar; ++j) {
            pc=temp.find[j];
            for(h=x->chn; h >= 0; --h) {
                for(k=0; k<x->ch[h].ncar; ++k){
                    if(x->ch[h].find[k] == pc) {
                        position=h+2;
                        break;
                    }
                }
                if(position !=0) break;
            }
            x->candidates[i].s+=(int)size-(int)position;
            position=0;
        }
        position=0;
        x->candidates[i].s/=div;
    }
    
    
    /*based on their score, sort the candidate list in ascending order
     here (Card Player method)
     */
    
    for(j=1; j < x->nca; j++) {
        tempc=x->candidates[j];
        k=j-1;
        while(k >=0 && tempc.s < x->candidates[k].s) {
            x->candidates[k+1]=x->candidates[k];
            k--;
        }
        x->candidates[k+1]=tempc;
    }
    
    /*if the verbose mode is on, print the data*/
    if(x->verbose==true) {
        str=(char*)malloc(sizeof(char)*1024);
        *str=0;
        num=(char*)malloc(sizeof(char)*32);
        *num=0;
        
        for(h=0; h< size; ++h) {
            for(k=0; k<x->ch[h].ncar; ++k){
                *num=0;
                sprintf(num," %d ",x->ch[h].find[k]);
                strcat(str,num);
            }
            strcat(str, " | ");
        }
        object_post((t_object*)x, "chain= %s", str);
        
        
        for(i=0; i<x->nca; ++i) {
            *str=0; *num=0;
            sprintf(num,"candidate[%d]= ( ",i);
            strcat(str,num);
            CopiaSet(&x->part->temp1[x->candidates[i].no],&temp);
            TrInvPCS(&temp,x->candidates[i].I,x->candidates[i].T);
            for(j=0; j<temp.ncar; ++j) {
                *num=0;
                sprintf(num," %d ",temp.find[j]);
                strcat(str,num);
            }
            CopiaSet(&x->part->temp2[x->candidates[i].no],&temp);
            TrInvPCS(&temp,x->candidates[i].I,x->candidates[i].T);
            strcat(str," | ");
            for(j=0; j<temp.ncar; ++j) {
                *num=0;
                sprintf(num," %d ",temp.find[j]);
                strcat(str,num);
            }
            strcat(str," ) ");
            *num=0;
            sprintf(num,"score=%f",x->candidates[i].s);
            strcat(str,num);
            object_post((t_object*)x, "%s",str);
        }
        free(str);free(num);
    }
}                                                                   //- return missing...?

//---------- new methods ---------

void pcs_chain_start(t_pcs_chain *x, int n) {                             //-     start message
    long inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        error_sym(x, gensym("that's not a valid pointer"));
        return;
    }
    
    if(x->pcs_received != 1) {
        object_error((t_object*)x, "no pointer to PCS received");
        return;
    }
    
    x->reverse=false;
    
    x->n=n;
    pcs_chain_start_chain(x,x->reverse);
    outlet_bang(x->done_out);			/*bang leftmost outlet when done*/
    return;

//---
}

void pcs_chain_startr(t_pcs_chain *x, int n) {                            //-     startr message
    long inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        error_sym(x, gensym("that's not a valid pointer"));
        return;
    }
    
    if(x->pcs_received != 1) {
        object_error((t_object*)x, "no pointer to PCS received");
        return;
    }
    
    x->reverse=true;
    
    x->n=n;
    pcs_chain_start_chain(x,x->reverse);
    outlet_bang(x->done_out);			//bang leftmost outlet when done
    return;
    
//---
}

//--- new func start_chain replaces startch
void pcs_chain_start_chain(t_pcs_chain *x, int r) {
    long inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        error_sym(x, gensym("that's not a valid pointer"));
        return;
    }
    
    get_min_max_pnum(&x->min,&x->max,0,x->pcs->ncar);
    
    if(x->n < x->min || x->n > x->max) {
        object_warn((t_object*)x, "partition (%d) requested is out of range, no action taken", x->n);
        return;
    }
    
    x->chn=0;
    pcs_chain_add_part(x,x->n,r);
    
    pcs_chain_get_candidates(x);		//compute candidates
    
    pcs_chain_eval(x);				//eval candidates for the next chain operation
    
    pcs_cand_output(x);				//output candidates first
    pcs_chain_output(x);				//output chain
    
    return;
}


void pcs_chain_add(t_pcs_chain *x, int n) {
    long inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        object_error((t_object*)x, "that's not a valid pointer");
        return;
    }
    
    if(x->chn ==0) {
        object_warn((t_object*)x, "chain not started, 'start n' or 'startr n' previous message needed");
        return;
    }
    x->prev=x->n;
    x->n=n;

    x->min=0; x->max=x->nca-1;
    if(x->n < x->min || x->n > x->max) {
        object_warn((t_object*)x, "partition (%d) requested is out of range, no conexion possible", x->n);
        x->n=x->prev;
        return;
    }
    pcs_chain_add_part(x,x->n,x->candidates[x->n].re);
    
    if(x->reverse)                  //- qué es esto???
        x->reverse=false;
    else
        x->reverse=true;            //-
    
    pcs_chain_get_candidates(x);	/*compute next candidates*/
    
    pcs_chain_eval(x);				/*eval candidates for the next chain operation*/
    
    pcs_cand_output(x);			/*output candidates first*/
    pcs_chain_output(x);			/*output chain*/
    outlet_bang(x->done_out);		/*bang leftmost outlet when done*/
    return;
}

void pcs_chain_reset(t_pcs_chain *x) {                                    //-     reset message
    long inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        error_sym(x, gensym("that's not a valid pointer"));
        return;
    }
    
    x->pcs_received=0;
    x->chn=0;
    x->pcs[0].find[0]=EOC;
    object_post((t_object*)x, "initialized to 0");
    return;
}

void pcs_chain_verbose(t_pcs_chain *x, int n) {                           //-     verbose message
    long inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        error_sym(x, gensym("that's not a valid pointer"));
        return;
    }
    
    if(n ==1) {
        x->verbose = true;
        object_post((t_object*)x, "verbose mode is ON");
    }
    if(n ==0) {
        x->verbose = false;
        object_post((t_object*)x, "verbose mode is OFF");
    }
    return;
}

