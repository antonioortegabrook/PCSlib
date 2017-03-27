/*
PCSLIB.H by Oscar Pablo Di Liscia and Pablo Cetta
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>

#include "tables.h"


/*DEFINITIONS*/

#define MPID "pcs_ptr_mes"
#define MMID "cm_ptr_mes"
#define STRLP 16

#define TABLE_CHUNK_SIZE 20

#define T6TI 2
#define T48  3
#define T39  4
#define T2A  6


#define TRUE  1
#define FALSE 0
/*
#define true  TRUE          //- redundante en C99
#define false FALSE         //-
*/
#define TIVL		12 /*invariant vector size*/
#define AGL			12 /*Aggregate size*/
#define ICVL		6  /*Interval-Class vector size*/
#define PCSL		36 /*PCS vector size*/ //- Podría ser mayor...
#define EOP		    -1 /*End of position*/
#define EOC		    -2 /*End of chain*/
#define ETY         -3 /*Empty position*/
#define NEXTC		12  /*Maximal Number of PCS per position of a CM*/   //- tal vez debería ser 12, ya que el objeto pcs.pf puede generar PCS de hasta 12 elementos
#define MAXELEM		144 // NEXTC*12    - tal vez podría ser NEXTC*NEXTC o una nueva constante para el nro de columnas
#define MAXROWS		12
#define SPAC		-4
#define MIN_CAD	     2
#define MAX_CAD	     9  
#define INVERT		20

#define TIPO_NADA	 -1	//self invariant
#define TIPO_0		 0
#define TIPO_1		 1
#define TIPO_2		 2
#define TIPO_12	    12
#define TIPO_3		 3
#define MAX_CMA     100


#define TEXT_IN_LEN 100
#define U_CM 		 1
#define U_PCS 		 2
#define U_NOTH		 3


#define NPCS		208 //Number of PCS of 3,4,5,6,7,8, and 9 elements
#define PCS_STRLEN	64  //Number of chars for each string for each PCS 

#define NPCS_3 12       /*number of pcs for each cardinality*/
#define NPCS_4 29
#define NPCS_5 38
#define NPCS_6 50
#define NPCS_7 38
#define NPCS_8 29
#define NPCS_9 12

#define PPCS_3 0      /*position on the table for each cardinality*/
#define PPCS_4 12
#define PPCS_5 41
#define PPCS_6 79
#define PPCS_7 129
#define PPCS_8 167
#define PPCS_9 196

#define ZLOC 11       /*the offset data at which is located the Z if any*/

#define NVIEW_ALL 0
#define NVIEW_3   1
#define NVIEW_4   2 
#define NVIEW_5   3
#define NVIEW_6   4
#define NVIEW_7   5
#define NVIEW_8   6
#define NVIEW_9   7

/*for the similarity relations*/
#define NRELS		12 /*the number of relations*/

#define Rr0			1
#define Rr1			2
#define Rr2			3
#define Rrp			4
#define Rsim		5
#define Rasim		6
#define Ricvsim		7
#define Roi			8
#define Rdis		9
#define Rscat		10
#define Rcc			11
#define Ropsc		12
#define NC			-100. /*not comparable(for Forte's and permutation relations)*/

#define MKS         31    /*max Kh size*/
#define MNK         102   /*number of Kh complexes*/
#define MAXNS		126   /*max subset number*/

#define MAX_SIM_SUBS 16

#define NCT			10 /*the maximal number of candidates transposed in a partition*/

/*Criteria for partition selection*/
#define NCR		    6  /*number of different possible criteria (6 at present)*/

#define LNI			1  /*prefers the partition/s that have less invariant PC with the one to connect*/
#define E_S			2  /*exclude the partition/s which is equal to the one to connect*/
#define E_P			3  /*exclude specific partition/s*/
#define P_P			4  /*prefer  specific partition/s*/
#define AGR			5  /*if the aggregate is complete, prefer the partition/s that favor the PCs with less frequency in the PFA*/
#define END			6  /*prefer the partition/s that close the chain*/

/* GROUP OPERATORS TYPES
	Type 1 (Tn)					labeled 101		0-30
	Type 2 (Tn/ITn)				labeled 102		31-177
	Type 3 (Tn//TnM)			labeled 103		178-300
	Type 4 (Tn///TnMI)			labeled 104		301-399
*/

#define T_O		0
#define IT_O	1
#define MT_O	2
#define IMT_O	3

#define TO	0
#define IO	1
#define MO	2
#define MIO	3

#define T1B	0
#define T2B	31
#define T3B	178
#define T4B	301
#define T5B	400
#define T5E	829

/*number of classes for each type*/
#define NCT1	5	
#define NCT2	9
#define NCT3	9
#define NCT4	9
#define NCT5	15

/*maximal order for TTO groups*/
#define MAX_ORDER	24
/*maximal size of cross-section vector*/
#define CS_MAX_SIZE	6 

//DATA TYPES

/*The PCS struct have now only numerical data*/
typedef struct {
  int  ncar;		/*cardinal number*/
  int  nord;		/*ordinal number*/
  int  nele;		/*number of pcs and EOPs*/
  int  find  [PCSL];/*PCS as delivered*/ 
  int  fprima[AGL+1]; /*Prime Form*/    //- agregamos 1 para EOC
  int  T;			/*T value respect of the prime form*/
  int  I;			/*1 if inverted, 0 if not*/
  int  icvmat[7];	/*interval-class vector*/
  int  selected;	/*whether the PCS is selected or not for some operation*/
  int  tp;			/*position in the table*/
} PCS;
/*these macros are to get data from the PCS table*/
#define GET_NIC(ptr, ic) ptr[tp+13+(ic-1)]



typedef struct {
  int mat[MAXELEM][MAXROWS];
  int NroFilas;
  int NroCols;
  int MaxElemxPos;
} CM;

typedef struct {
  PCS set;
  PCS temp1[33];
  PCS temp2[33];
  int clasif[33];
  int cant;
}PART;

typedef struct{
	int no;		/*the number of partition of the candidate*/
	int re;		/*whether it is in reverse order or not*/
	int T;	    /*the transposition factor to match the target(there may be more than one)*/
	int I;	    /*whether it must be inverted or not*/
	float s;	/*the score of the candidate according the evaluation criteria applied*/
}CAND;

/*Struct SIM*/
typedef struct {
	int   r0;	/*Forte*/
	int   r1;
	int   r2;
	int   rp[MAX_SIM_SUBS];/*store the ordinal number of the set classes of n-1 included*/
	float sim;	 /*Morris*/
	float asim;	 /*Morris*/
	float icvsim;/*Isaacson*/
	int   oi;    /*order inversions eval (Babbitt)*/
	int   dis;   /*displacement and scattering are from Morris, 1987*/
	int   scat;
	float cc;    /*correlation coefficient*/
	float opcss[2]; /*Di Liscia's ordered PCS similarity coefficients, opcss[0]is SPC coef, opcss[1]is SAS coef*/
} SIM;

/*Struct TTO Group*/
typedef struct {
	int ty;		/*group type*/
	int cl;		/*group class*/
	int mb;		/*group member, if more than one*/
	int or;		/*order of the group*/
	int op[MAX_ORDER+1];	/*group's operators*/   //- agregamos 1 para EOC
} TTOG;
//////////////////////////////////////////////////
/*prototypes in pcs_funcs.c*/
int  sort_function( const void *a, const void *b);
int  forma_prima(PCS* pcs, int *tableptr);
int	 pcs_table_loc(int card, int ord);
/*printing*/
void pc_2txt(int pc, char*str);
void Pname(PCS *pcs, char *array);
void Picv(PCS *pcs, char *array);
void Pfp(PCS *pcs, char *array);
void Pstat(PCS *pcs, char *array);
void Pset(PCS* pcs, char *str);
void print_pcs(PCS* pcs, char *str);
/*subsets*/
int  Subsets(PCS* pcs, int ns, PCS *buffer, int *tableptr);
int  get_n_subsets(int set, int sub);
int  factorial(int n);
/*misc*/
void make_U(PCS *pcs1, PCS *pcs2, PCS *U);
int find_intersec(PCS *p1, PCS *p2,PCS *target);
void find_complement(PCS* pcs, PCS* comp, int *tableptr);
int  isz(PCS *pcs, int *tableptr);
void CopiaSet(PCS* source, PCS* target);
int  NombresIguales(PCS* pcs1, PCS* pcs2);
int  SetsIguales(PCS* pcs1, PCS* pcs2);
void Invariancia(PCS* pcs, int* Invar);
void TrInvPCS(PCS* pcs, int inv, int index);
void OrdenaPCS(PCS* pcs);
int  no_rep(PCS* pcs, int *aux); 
void permutations(int *pl, int np, int n);
void get_permutation(int *pl, int np, int n, int num);
int pcs_find(PCS* pcs1, PCS* pcs2);

//////////////////////////////////////////////////
/*prototypes in cm_funcs.c*/
/*CM creation*/
int find_rep_cols(int **mat,int r, int c, int *vec);
int  ** mataloc(int r, int c);
void matdealoc(int ** ptr, int c);
void post_pmat(int **pmat, int r, int c);
void post_mat(int **mat, int **mat2, int r, int c);
int  cm_ttog(CM *cm, TTOG *ttog, PCS *pcs);
void RomanSq(CM* cm, PCS *pcs);
void MatTipo1a(CM* cm, PCS* pcs);
void MatTipo1b(CM* cm, PCS* pcs);
void MatTipo2(CM* cm,PCS* pcs1,PCS* pcs2);
int  MatFCycles(CM* cm,PCS *pcs, int TnI);
/*CM operations*/
void CopiaMatriz(CM* cm, CM* cmdest);
int  MaxElemxPos(int*);
void Inicializar(CM* cm);
void Vti(int* vector, int TnI);
void OneXPos(int* vector);
void AllXPos(int* vector);
int  check_rc(CM *mat, int r,int c);
void MTransp(CM* cm, int transp);
void InterCols(CM* cm, int col1,int col2);
void InterFilas(CM* cm, int  fil1, int fil2);
void MInver(CM* cm);
void cm_tto(CM* cm, int tto , int t);
void Mti(CM* cm, int indice , int inv);
void RotaDiag(CM* cm, int Flag);
void Rota90(CM* pcm);

int  Swap(CM* cm);
void Densidad(CM* cm, int dens[MAXROWS][MAXROWS]);
void Insertar(CM* cm, int nf, int nc, int valor);
void Borrar(CM* cm, int nf, int nc, int valor);

void pos_to_pcs(CM *cm, PCS* pcs, int r, int c);
void pc_to_pos(CM *cm, int pc, int r, int c);
void pcs_to_pos(CM *cm, PCS* pcs, int r, int c);
int pos_is_empty(CM *cm,int r, int c);
void empty_pos(CM *cm, int r, int c);
void row_to_pcs(CM *cm, PCS* pcs, int r);
void col_to_pcs(CM *cm, PCS* pcs, int c);
void cm_to_pcs (CM *cm, PCS* pcs);
int  pos_to_npcs(CM *cm, int r, int c);
/*CM analysis*/
int  get_T(CM *cm);
int  get_pairs(int n);
float frag(CM *cm);
float spar(CM* cm);
void hist(CM *cm, float *harray);

//////////////////////////////////////////////////
/*prototypes in part_funcs.c*/
/*and chains*/
void ParticioPcs(PCS* pcs, PART*, int *tableptr);
int  CalculaParticio(PART* pt, int *tableptr);
int  ClasifParticio(PART*);
void Particiones(PCS* pcs, PART* pt, int *tableptr);
int  ClasifParticio(PART* pt);
int  Ppart_row(PART *part, int n, char *str);
void get_min_max_pnum(int *min, int *max, int prev, int ncar);
void get_TI(PCS* source, PCS* target, CAND *cand , int *cnt);
int get_nt(int n,int **itmat, int s);
int ch_close(PCS *start, PCS *end, PCS *norm);
//////////////////////////////////////////////////
/*prototypes in sim_funcs.c*/
float sim(int *icv1, int *icv2);
float asim(int *icv1, int *icv2);
float icvsim(int *icv1, int *icv2);
int fr0(int *icv1, int *icv2);
int fr1(int *icv1, int *icv2);
int fr2(int *icv1, int *icv2);
void frp(PCS *pcs1, PCS *pcs2, int *ordarray);
void roi_dis_scat_cc(PCS *pcs1, PCS *pcs2, SIM *sim);
void opcss(PCS *pcs1, PCS *pcs2, float *SPC, float *SAS);
float eval_dyad_order(PCS *pa,PCS *pb);
float rho_calc(int n, float *l1, float *l2);
//////////////////////////////////////////////////
/*prototypes in ttog_funcs.c*/
int tto_pc(int pc, int n, int tto);
int ttog_read_group (TTOG *ttog);
int ttog_read_member(TTOG *ttog, int offset);
void ttog_init(TTOG *ttog);
void ttog_perform(TTOG *ttog, int *pcsv, int **mat);
void ttog_perform_on_mat(TTOG *ttog, int **mat);
int pc_ttog_ind(TTOG *ttog, int indx, int pc);
