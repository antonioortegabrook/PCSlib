/*                                              Max version by Antonio Ortega Brook
 Requires Max6/7 & OS X 10.7 or later
 September 2016
 -------
 pcs.2dict object by Antonio Ortega Brook
 input: a PCS struct pointer
 Converts a PCS to a Dictionary
 */

/*
 TODOS LOS COMENTARIOS QUE COMIENZAN CON //- SON MÍOS; EL OBJETIVO DE ESTO ES FACILITAR LA BÚSQUEDA DE ELLOS MIENTRAS ESTOY ESCRIBIENDO; LUEGO DEBERÁN SER BORRADOS O MODIFICADOS
 
 Borrar variables no utilizadas...?
 */

#include "ext.h"				//	Max headers
#include "ext_obex.h"				//
#include "ext_dictobj.h"

#include "../pcslib-includes/pcslib_max.c"      //	PCSlib header

typedef struct _pcs_2dict {		// defines our object's internal variables for each instance in a patch
	t_object x_obj;			// object header - ALL objects MUST begin with this...
	
	PCS		*pcs;   // pointer to a PCS struct
	t_dictionary	*d;
	t_symbol	*pcs_dict_name;
	
	t_atom		*original_pcs;
	t_atom		*prime_form;
	t_atom		*icv;
	
	long p_in;
	void *dict_out;
} t_pcs_2dict;

// these are prototypes for the methods that are defined below
void pcs_2dict_pcs_ptr(t_pcs_2dict *x, t_symbol *s, long argc, t_atom *argv);
void pcs_2dict_bang(t_pcs_2dict *x);

void pcs_2dict_assist(t_pcs_2dict *x, void *b, long m, long a, char *s);
void stdinletinfo();

void pcs_2dict_free(t_pcs_2dict *x);              //-
void *pcs_2dict_new();

static t_class *pcs_2dict_class;		// global pointer to the object class - so max can reference the object
//static t_symbol *pcs_dict_name;
static t_symbol *pcs_dictionary;

static t_symbol *ncar;
static t_symbol *nord;
static t_symbol *find;
static t_symbol *fprima;
static t_symbol *transp;
static t_symbol *inv;
static t_symbol *icvmat;


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;
	
	c = class_new("pcs.2dict", (method)pcs_2dict_new, (method)pcs_2dict_free, sizeof(t_pcs_2dict), 0L, 0);
	
	class_addmethod(c, (method)pcs_2dict_pcs_ptr,	MPID,		A_GIMME,	0);
	class_addmethod(c, (method)pcs_2dict_bang,	"bang",				0);
	class_addmethod(c, (method)pcs_2dict_assist,	"assist",	A_CANT,		0);	// (optional) assistance method
	class_addmethod(c, (method)stdinletinfo,	"inletinfo",	A_CANT,  0);
	
	class_register(CLASS_BOX, c);
	pcs_2dict_class = c;
	
//	pcs_dict_name = gensym("PCS");
	pcs_dictionary = gensym("dictionary");
	
	ncar = gensym("Cardinal");
	nord = gensym("Ordinal");
	find = gensym("PCS");
	fprima = gensym("Prime form");
	transp = gensym("T");
	inv = gensym("I");
	icvmat = gensym("ICV");
}

//--------------------------------------------------------------------------

void *pcs_2dict_new() {
	t_pcs_2dict *x;                                             // local variable (pointer to a t_pcs_2dict data structure)
	
	x = (t_pcs_2dict *)object_alloc(pcs_2dict_class);           // create a new instance of this object
	
	x->dict_out = outlet_new(x, NULL);
	
	x->pcs=NULL;
	
	x->pcs = (PCS *)malloc(sizeof(PCS));
	x->pcs->find[0] = EOC;
	
	x->original_pcs	= (t_atom *)malloc(PCSL * sizeof(t_atom));
	x->prime_form	= (t_atom *)malloc(12 * sizeof(t_atom));
	x->icv		= (t_atom *)malloc(6 * sizeof(t_atom));
	
	x->d = dictionary_new();
	dictobj_register(x->d, &x->pcs_dict_name);
	
	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void pcs_2dict_assist(t_pcs_2dict *x, void *b, long m, long a, char *s) {
	if (m == ASSIST_INLET) {
		switch(a) {
			case(0):
				sprintf(s,"bang, PCS");
				break;
			case(1):
				sprintf(s,"PCS");
				break;
		}
	}
	if (m == ASSIST_OUTLET)
		sprintf(s,"dictionary");
}

void pcs_2dict_free(t_pcs_2dict *x){
	if(x->pcs != NULL) free(x->pcs);
	object_free((t_object *)x->d);
	return;
}

void pcs_2dict_pcs_ptr(t_pcs_2dict *x, t_symbol *s, long argc, t_atom *argv) {
	int i;
	
	{  //------------ get ptr --------------
		t_symbol *temp;
		PCS *tempcs;
		temp = atom_getsym(argv);
		
		sscanf(temp->s_name, "%p", &tempcs);    // get the pointer to a PCS struct
		CopiaSet(tempcs,x->pcs);                    //- (cuidado con el nombre de la estructura)
	}  //------------ end get --------------
	
	dictionary_clear(x->d);	//
	
	i = 0;
	while (x->pcs->find[i] != EOC) {
		atom_setlong(x->original_pcs + i, x->pcs->find[i]);
		i++;
	}
	dictionary_appendatoms(x->d, find, i, x->original_pcs);
	
	i = 0;
	while (x->pcs->fprima[i] != EOC) {
		atom_setlong(x->prime_form + i, x->pcs->fprima[i]);
		i++;
	}
	dictionary_appendatoms(x->d, fprima, x->pcs->ncar, x->prime_form);
	
	dictionary_appendlong(x->d, ncar, x->pcs->ncar);
	dictionary_appendlong(x->d, nord, x->pcs->nord);
	dictionary_appendlong(x->d, transp, x->pcs->T);
	dictionary_appendlong(x->d, inv, x->pcs->I);
	
	i = 0;
	while (i < 6) {
		atom_setlong(x->icv + i, x->pcs->icvmat[i]);
		i++;
	}
	dictionary_appendatoms(x->d, icvmat, 6, x->icv);
	
	pcs_2dict_bang(x);
}

void pcs_2dict_bang(t_pcs_2dict *x) {
	{
		t_atom a;
		atom_setsym(&a, x->pcs_dict_name);
		outlet_anything (x->dict_out, pcs_dictionary, 1, &a);    //- (cuidado con el nombre del outlet)
	}
	return;
}
