/*  cm_maker object  by Oscar Pablo Di Liscia - Max version by Antonio Ortega Brook
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

typedef struct _cm_maker {	// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
    PCS *pcs; 	  /*pointer to PCS struct*/
    int npcs; /*the number of PCS stored*/
    CM  *cm;
    
    long p_in;
    void *pcs_in;
    void  *cm_out, *cm_nrc;
} t_cm_maker;

// these are prototypes for the methods that are defined below
void cm_maker_pcs_ptr(t_cm_maker *x, t_symbol *s, long argc, t_atom *argv);

void cm_maker_bang(t_cm_maker *x);
void cm_maker_reset(t_cm_maker *x);
void cm_maker_close(t_cm_maker *x);

void cm_maker_add(t_cm_maker *x);
void cm_maker_output(t_cm_maker *x);

void cm_maker_assist(t_cm_maker *x, void *b, long m, long a, char *s);
void cm_maker_free(t_cm_maker *x);
void *cm_maker_new();

static t_class *cm_maker_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("cm.maker", (method)cm_maker_new, (method)cm_maker_free, sizeof(t_cm_maker), 0L, 0);
    
    class_addmethod(c, (method)cm_maker_pcs_ptr,        MPID,          A_GIMME, 0);    // pointer to PCS
    
    class_addmethod(c, (method)cm_maker_bang,           "bang",                 0);
    class_addmethod(c, (method)cm_maker_reset,          "reset",                0);
    class_addmethod(c, (method)cm_maker_close,          "close",                0);
    
    class_addmethod(c, (method)cm_maker_assist,         "assist",      A_CANT,  0);	// (optional) assistance method
    class_addmethod(c, (method)stdinletinfo,            "inletinfo",   A_CANT,  0);

	class_register(CLASS_BOX, c);
	cm_maker_class = c;
}

//--------------------------------------------------------------------------

void *cm_maker_new() {
	t_cm_maker *x;                                             // local variable (pointer to a t_cm_maker data structure)

	x = (t_cm_maker *)object_alloc(cm_maker_class);           // create a new instance of this object
    
    x->pcs_in = proxy_new(x, 1, &x->p_in);
                                     // create outlets
    x->cm_nrc=listout(x);
    x->cm_out=outlet_new(x, NULL);
    
    x->pcs=NULL;
    x->npcs=0;
    
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs->find[0]=EOC;
    x->cm=(CM*)malloc(sizeof(CM));
    Inicializar(x->cm);
    
	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void cm_maker_assist(t_cm_maker *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET) {
        switch(a) {
            case(0):
                sprintf(s,"reset, close");
                break;
            case(1):
                sprintf(s,"PCS");
                break;
        }
    }
    if (m == ASSIST_OUTLET) {
        switch(a) {
            case(0):
                sprintf(s,"CM");
                break;
            case(1):
                sprintf(s,"Number of rows and columns (list)");
                break;
        }
    }
}

void cm_maker_pcs_ptr(t_cm_maker *x, t_symbol *s, long argc, t_atom *argv) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=1) {
        object_error((t_object*)x, "left inlet doesn't accept pointers");
        return;
    }

    if(x->cm->NroFilas == MAXROWS) {
        object_warn((t_object*)x, "sorry, maximal number lenght of the chain is %d", x->npcs);
        return;
    }

    {  //------------ get ptr --------------
        t_symbol *temp;
        PCS *tempcs;
        temp = atom_getsym(argv);
        
        sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
        x->npcs++;
        x->pcs=(PCS*)realloc(x->pcs,x->npcs*sizeof(PCS));   //- ojo con esto: puede devolver null
        if(!x->pcs) {
            post("cm.maker: realloc returned NULL in cm_maker_pcs_ptr");
            return;
        }
        CopiaSet(tempcs,&x->pcs[x->npcs-1]);
        
    }  //------------ end get --------------
    
    cm_maker_add(x);
    
    return;
}

void cm_maker_add(t_cm_maker *x)
{
    int pos;
    int oe=x->npcs % 2; /*false if even, true if odd*/
    
    if(x->npcs==1) {
        pcs_to_pos(x->cm, &x->pcs[x->npcs-1], 0, 0);
        
    }
    else {
        pos=x->npcs/2;
        
        if(oe) {
            if(x->npcs/2 >= MAXROWS-1) {    //- más seguro que ==
                object_warn((t_object*)x, "maximal number of rows reached! No action taken.");
                return;
            }
            pcs_to_pos(x->cm, &x->pcs[x->npcs-1], pos, pos);
        }
        else {
            if(x->npcs/2 >= MAXROWS-1) {    //- más seguro que ==
                object_warn((t_object*)x, "maximal number of rows reached! No action taken.");
                return;
            }
            pcs_to_pos(x->cm, &x->pcs[x->npcs-1], pos-1, pos);
        }
    }
    x->cm->MaxElemxPos=(x->cm->MaxElemxPos >= x->pcs[x->npcs-1].ncar ? x->cm->MaxElemxPos : x->pcs[x->npcs-1].ncar );
    
    return;
}

void cm_maker_reset(t_cm_maker *x) {
    t_int inum = proxy_getinlet((t_object *)x);
    
    if(inum !=0) {
        error_sym(x, gensym("that's not a valid pointer"));
        return;
    }
    
    if(x->pcs->find[0]==EOC) {
        object_warn((t_object*)x, "pointer to pcs not received");
        return;
    }
    
    x->npcs=0;
    x->pcs=(PCS*)malloc(sizeof(PCS));
    x->pcs->find[0]=EOC;
    Inicializar(x->cm);
    
    object_post((t_object*)x, "chain initialized");
    return;
}

void cm_maker_close(t_cm_maker *x) {
    t_int inum = proxy_getinlet((t_object *)x);
    PCS start, end, second, norm;
    
    if(inum !=0) {
        error_sym(x, gensym("that's not a valid pointer"));
        return;
    }
    
    if(x->npcs%2 == true || x->npcs < 4) {
        object_warn((t_object*)x, "can't close the CM");
        //just warn, but outputs the CM anyway
        x->cm->NroFilas=x->cm->NroCols=(x->npcs/2)+1;
    }
    else {
        pos_to_pcs(x->cm, &start, 0, 0);
        pos_to_pcs(x->cm, &second, 0, 1);
        pos_to_pcs(x->cm, &end, (x->npcs/2)-1, x->npcs/2);
        
        make_U(&start, &second, &norm);
        
        if(ch_close(&start, &end, &norm)) {
            empty_pos(x->cm, (x->npcs/2)-1, x->npcs/2);
            pcs_to_pos(x->cm, &end, (x->npcs/2)-1,0 );
            x->cm->NroFilas=x->cm->NroCols=(x->npcs/2);
            
        }
        
        else {
            object_warn((t_object*)x, "can't close the CM, non cyclic chain");
            x->cm->NroFilas=x->cm->NroCols=(x->npcs/2)+1;
            //just warn, but outputs the CM anyway
        }
    }
    
    cm_maker_output(x);
    
    return;
}
void cm_maker_bang(t_cm_maker *x) {
    t_int inum=proxy_getinlet((t_object *)x);
    if(inum!=0) {
        object_error((t_object*)x, "wrong message");
        return;
    }
    if(x->npcs==0) {
        object_post((t_object*)x, "nothing to output...");
        return;
    }
    x->cm->NroFilas=x->cm->NroCols=(x->npcs/2)+1;
    cm_maker_output(x);
}
void cm_maker_output(t_cm_maker *x) {
    {                    //output Number of rows and cols
        t_atom plist[2];
        atom_setlong((plist), x->cm->NroFilas);
        atom_setlong((plist+1), x->cm->NroCols);
        outlet_list(x->cm_nrc, gensym("list"),2,plist);
    }
    
    {   //------------- out cm ptr -------------------
        char pstr[STRLP];
        t_atom ptr;
        sprintf(pstr, "%p", x->cm);    //convert pointer to CM struct into symbol //- (cuidado con el nombre de la struct)
        atom_setsym(&ptr, gensym(pstr));
        outlet_anything (x->cm_out, gensym(MMID), 1, &ptr);    //- (cuidado con el nombre del outlet)
    }   //------------- end out -------------------
    
}

void cm_maker_free(t_cm_maker *x){
    
    if(x->pcs != NULL){
        free(x->pcs);
    }
    if(x->cm != NULL){
        free(x->cm);
    }
    return;
}
