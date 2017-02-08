/*part_funcs.c by Pablo Cetta and Pablo Di Liscia*/
/*
                    Modificado por Antonio Ortega Brook, Septiembre 2016;
                    todos los cambios están señalados con //-
 */
/*
void ParticioPcs(PCS* pcs, PART*, int *tableptr);
int  CalculaParticio(PART* pt, int *tableptr);
int  ClasifParticio(PART*);
void Particiones(PCS* pcs, PART* pt, int *tableptr);
int  ClasifParticio(PART* pt);
int  Ppart_row(PART *part, int n, char *str);
void get_min_max_pnum(int *min, int *max, int prev, int ncar);

void get_TI(PCS* source, PCS* target, CAND *cand , int *cnt);
int get_nt(int n,int **itmat, int s);
void MatCad(CM* cm, PART* pt, PCS* pcs, int gencad, int nc);
int ch_close(PCS *start, PCS* end);
*/
  /////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------------
//	Inicializa estructura de particiones
// ----------------------------------------------------------------------------------
void InicializarPart(PART* pt)
{
	int i;
	pt->set.find[0] = EOC;
	for (i = 0; i < 32; i++){
		pt->temp1[i].find[0] = EOC;
		pt->temp2[i].find[0] = EOC;
		pt->clasif[i] = 0;
	}
	pt->cant = 0;
}
// ----------------------------------------------------------------------------------
//	Procesa el comando PARTICIO
// ----------------------------------------------------------------------------------
void ParticioPcs(PCS* pcs, PART* pt, int *tableptr)
{
	Particiones(pcs, pt, tableptr);
}
// ----------------------------------------------------------------------------------
//	Calcula particiones de un set de 5 o 6 elementos y sus posib. para generar cadenas
// ----------------------------------------------------------------------------------
void Particiones(PCS* pcs, PART* pt, int *tableptr)
{
	InicializarPart(pt);
	CopiaSet(pcs, &pt->set);
	forma_prima(&pt->set,tableptr);
	if(!CalculaParticio(pt,tableptr)){
		return;
	}
}
// --------------------------------------------------------------------
//	Calcula las particiones de un set
// --------------------------------------------------------------------

int CalculaParticio(PART* pt, int *tableptr)
{
	int n, nelem = 0, k = 0, mini = 6, maxi = 7, x, z, w, h, l, s, q, p, b = 0  /* ,j */;   //- unused variable 'j'
	int m5[75] = {0,1,2,3,4,
		1,0,2,3,4,
		2,0,1,3,4,
		3,0,1,2,4,
		4,0,1,2,3,
		0,1,2,3,4,
		0,2,1,3,4,
		0,3,1,2,4,
		0,4,1,2,3,
		1,2,0,3,4,
		1,3,0,2,4,
		1,4,0,2,3,
		2,3,0,1,4,
		2,4,0,1,3,
		3,4,0,1,2};
	int m6[186] = {0,1,2,3,4,5,/*1+5*/
		1,0,2,3,4,5,
		2,0,1,3,4,5,
		3,0,1,2,4,5,
		4,0,1,2,3,5,
		5,0,1,2,3,4,
		0,1,2,3,4,5,/*2+4*/
		0,2,1,3,4,5,
		0,3,1,2,4,5,
		0,4,1,2,3,5,
		0,5,1,2,3,4,
		1,2,0,3,4,5,
		1,3,0,2,4,5,
		1,4,0,2,3,5,
		1,5,0,2,3,4,
		2,3,0,1,4,5,
		2,4,0,1,3,5,
		2,5,0,1,3,4,
		3,4,0,1,2,5,
		3,5,0,1,2,4,
		4,5,0,1,2,3,
		0,1,2,3,4,5,/*3+3*/
		0,1,3,2,4,5,
		0,1,4,2,3,5,
		0,1,5,2,3,4,
		0,2,3,1,4,5,
		0,2,4,1,3,5,
		0,2,5,1,3,4,
		0,3,4,1,2,5,
		0,3,5,1,2,4,
		0,4,5,1,2,3};
		/*
		PCS of 5 elements have 15 binary partitions of:   (4 + 1) 5 partitions
		(3 + 2) 10 partitions
		
		 PCS of 6 elements have 31 binary partitions of:   (5 + 1) 6  partitions
		 (4 + 2) 15 partitions
		 (3 + 3) (20/2)=10 partitions
	*/
	while(pt->set.find[k] != EOC) {
		if(pt->set.find[k] != EOP)nelem++;
		k++;
	}
	if(nelem > 6 || nelem < 5)return 0;
	
	AllXPos(pt->set.find); 
	
	for(n = mini; n < maxi; n++){
		h = 0;
		for(x = 0; x < (nelem - 3); x++){
			if(x == 0)l = 6;
			else if(x == 1)l = 15;
			else l = 10;
			if(nelem == 5)p = (5 * (x + 1));
			else p = l;
			for(s = 0; s < p; s++){
				z = 0; w = 0;
				for(q = 0; q < nelem; q++){
					if(nelem == 5){
						if(q < (1 + x)) pt->temp1[b].find[w++] = pt->set.find[m5[h]];
						if(q > x) pt->temp2[b].find[z++] = pt->set.find[m5[h]];
						h++;
					}
					else{
						if(q < (1 + x)) pt->temp1[b].find[w++] = pt->set.find[m6[h]];
						if(q > x) pt->temp2[b].find[z++] = pt->set.find[m6[h]];
						h++;
					}
				}
				pt->temp1[b].find[w++] = EOP; //EOP
				pt->temp2[b].find[z++] = EOP;
				pt->temp1[b].find[w] = EOC; 	//EOC
				pt->temp2[b].find[z] = EOC;
				pt->cant = b;
				forma_prima(&pt->temp1[b], tableptr);
				forma_prima(&pt->temp2[b], tableptr);
				pt->clasif[b] = ClasifParticio(pt);
				b++;
			}/*s*/
		}/*x*/
	}/*n*/
	
	pt->cant = b; 
	pt->temp1[b].find[w++] = EOP; //EOP
	pt->temp2[b].find[z++] = EOP;
	pt->temp1[b].find[w] = EOC; 	//EOC
	pt->temp2[b].find[z] = EOC;
	forma_prima(&pt->temp1[b], tableptr);
	forma_prima(&pt->temp2[b], tableptr);
	pt->clasif[b] = ClasifParticio(pt);
	
	return 1;
}
// --------------------------------------------------------------------
//	Clasifica las particiones por sus propiedades
// --------------------------------------------------------------------
int ClasifParticio(PART* pt)
{
	int i, j, k, q;
	PCS temp3, temp4;
	int InvarK[PCSL], InvarS[PCSL];
	int result = TIPO_NADA;
	int Caso1;
	
	for(i = 0; i < pt->cant; i++){
		if(NombresIguales(&pt->temp1[i], &pt->temp1[pt->cant]) && NombresIguales(&pt->temp2[i], &pt->temp2[pt->cant])){result = TIPO_0; goto SaltearCasos2;}
		if(NombresIguales(&pt->temp1[i], &pt->temp2[pt->cant]) && NombresIguales(&pt->temp2[i], &pt->temp1[pt->cant])){result = TIPO_0; goto SaltearCasos2;}
	}
	
	i = j = 0;									//REGLA 3
	while(pt->temp1[pt->cant].find[i] != EOC) {		//Copia de temporales
		if(pt->temp1[pt->cant].find[i] != EOP) temp3.find[j++] = pt->temp1[pt->cant].find[i];
		i++;
	}
	temp3.find[j] = EOC;
	i = q = 0;
	while(pt->temp2[pt->cant].find[i] != EOC) {
		if(pt->temp2[pt->cant].find[i] != EOP) temp4.find[q++] = pt->temp2[pt->cant].find[i];
		i++;
	}
	temp4.find[j] = EOC;
	
	if(j == q){			//son de igual nro. de elementos
		for(i = 0; i < 12; i++){
			k = 0;
			while(temp3.find[k] != EOC){
				temp3.find[k] = temp3.find[k] + i;
				if(temp3.find[k] > 11)temp3.find[k] = temp3.find[k] - 12;
				k++;
			}
			if(SetsIguales(&temp3, &temp4)){
				k = 0;
				while(temp4.find[k] != EOC){
					temp4.find[k] = temp4.find[k] + i;
					if(temp4.find[k] > 11)temp4.find[k] = temp4.find[k] - 12;
					k++;
				}
				if(!SetsIguales(&pt->temp1[pt->cant], &temp4)){
					result = TIPO_3;
					goto SaltearCasos2;
				}
			}
		}
		
		i = j = 0;					//Copia de temporales
		while(pt->temp1[pt->cant].find[i] != EOC) {
			if(pt->temp1[pt->cant].find[i] != EOP)temp3.find[j++] = pt->temp1[pt->cant].find[i];
			i++;
		}
		temp3.find[j] = EOC;
		i = j = 0;
		while(pt->temp2[pt->cant].find[i] != EOC) {
			if(pt->temp2[pt->cant].find[i] != EOP)temp4.find[j++] = pt->temp2[pt->cant].find[i];
			i++;
		}
		temp4.find[j] = EOC;
		
		k = 0;
		while(temp3.find[k] != EOC){			//invierto
			temp3.find[k] = 12 - temp3.find[k];
			if(temp3.find[k] == 12)temp3.find[k] = 0;
			k++;
		}
		for(i = 0; i < 12; i++){
			k = 0;
			while(temp3.find[k] != EOC){
				temp3.find[k] = temp3.find[k] + i;
				if(temp3.find[k] > 11)temp3.find[k] = temp3.find[k] - 12;
				k++;
			}
			if(SetsIguales(&temp3, &temp4)){
				k = 0;
				while(temp4.find[k] != EOC){
					temp4.find[k] = 12 - temp4.find[k];
					if(temp4.find[k] == 12)temp4.find[k] = 0;
					temp4.find[k] = temp4.find[k] + i;
					if(temp4.find[k] > 11)temp4.find[k] = temp4.find[k] - 12;
					k++;
				}
				if(SetsIguales(&pt->temp1[pt->cant], &temp4)){
					result = TIPO_3;
					goto SaltearCasos2;
				}
			}
		}
	}
	
	Invariancia(&pt->set, InvarK);					//REGLA 1
	Invariancia(&pt->temp1[pt->cant], InvarS);
	Caso1 = FALSE; i = j = 1;
	if(InvarS[1] == EOC)goto SaltearCasos1;
	while(InvarK[i] != EOC){
		while(InvarS[j] != EOC){
			if(InvarK[i] == InvarS[j++])Caso1 = TRUE;
		}
		j = 1; i++;
	}
	if(!Caso1)result = TIPO_1;
SaltearCasos1:
	Invariancia(&pt->temp2[pt->cant], InvarS);			//REGLA 2
	Caso1 = FALSE; i = j = 1;
	if(InvarS[1] == EOC)goto SaltearCasos2;
	while(InvarK[i] != EOC){
		while(InvarS[j] != EOC){
			if(InvarK[i] == InvarS[j++])Caso1 = TRUE;
		}
		j = 1; i++;
	}
	if(!Caso1){
		if(result == 1)result = TIPO_12;
		else result = TIPO_2;
	}
SaltearCasos2:
	return result;
}
// ----------------------------------------------------------------------------
//	Imprime solo la particion "n" en un array de chars
//  retorna false si "n" esta fuera de rango y "n" si se pudo imprimir
// ----------------------------------------------------------------------------
int Ppart_row(PART *part, int n, char *str)
{
char *num;

if(n < 0 || n > part->cant-1 ) {
	return(false);
}
  
num=(char*)malloc(sizeof(char)*64);
*num=0;
*str=0;

	if(n != 0 && part->temp1[n].ncar != part->temp1[n-1].ncar)
		strcat(str,"\n");

	sprintf(num,"%d) ",n);
	strcat(str,num);
	
	strcat(str,"\t");
	Pname(&part->temp1[n],num);
	strcat(str,num);

	strcat(str,"\t");
	Pset(&part->temp1[n],num);
	strcat(str,num);
	
	strcat(str,"\t");
	Pname(&part->temp2[n],num);
	strcat(str,num);
	
	strcat(str,"\t");
	Pset(&part->temp2[n],num);
	strcat(str,num);
	
	*num=0;
	strcat(str,"\t");
	sprintf(num,"(%d)",part->clasif[n]);
	strcat(str,num);

free(num);

	return(n);
}
// ----------------------------------------------------------------------------
//	Determina el indice minimo y maximo de las particiones dentro de una estructura
//  de particiones, de acuerdo con el cardinal de la norma y la particion previa 
//  para encadenar. Asume que el PCS tiene cardinal 5 o 6.
//  En el futuro habra que generalizarlo para otras cardinalidades. 
// ----------------------------------------------------------------------------
void get_min_max_pnum(int *min,int *max, int prev, int ncar)
{
	
	if(prev==0){
		if(ncar==5){
			*min=0; *max=14;
			return;
		}
		else{
			*min=0; *max=30;
			return;
		}
	}
	if(ncar ==5) {
		if(prev==1 || prev==4){
			*min=0; *max=4;
			return;
		}
		if(prev==2 || prev==3){
			*min=5; *max=14;
			return;
		}	
	}
	if(ncar ==6) {
		if(prev==1 || prev==5){
			*min=0; *max=5;
			return;
		}
		if(prev==2 || prev==4){
			*min=6; *max=20;
			return;
		}	
		if(prev==3){
			*min=21; *max=30;
			return;
		}	
	}

	return;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------
//	determina el valor de T o TI de un PCS (source) para transformarlo 
//  en otro (target)
// --------------------------------------------------------------------
void get_TI(PCS *source, PCS *target, CAND *cand , int *cnt)
{
	int i, j, cnt1=0,cnt2=0,t=0,x,n;
	int **itmat;
	int ncar=source->ncar;
	int nc=*cnt;
	int no=cand[nc].no;
	int re=cand[nc].re;

	itmat=(int**)malloc(sizeof(int*)*ncar);
	for(i=0; i<ncar; ++i) {
		itmat[i]=(int*)malloc(sizeof(int)*ncar);
	}
	
	/* do IT matrix*/
	for(i=0; i<ncar; ++i) {
		for(j=0; j<ncar; ++j) {
			itmat[i][j]=(target->find[i]+source->find[j])%12;
		}
	}
	/*check for total invariance numbers (target match by IT)*/
	for(i=0; i<ncar; ++i) {
		for(j=0; j<ncar; ++j) {
			t=-1;
			n=itmat[i][j];
			for(x=0; x<cnt1; x++) {
				if(n==cand[x].T) {
					n=-1;
					break;
				}
			}
			if(n !=-1)
				t=get_nt(n,itmat,ncar);
		}
		if(t==ncar) { /*match found, load data on cand struct*/
			cand[nc].T=n;
			cand[nc].I=true;
			if(nc !=0){
				cand[nc].no=no;
				cand[nc].re=re;
			}
			nc++; cnt1++;
		}
	}
	/* do T invariance matrix*/
	for(i=0; i<ncar; ++i) {
		for(j=0; j<ncar; ++j) {
			itmat[i][j]=((12-source->find[i])+target->find[j])%12;
		}
	}
	/*check for total invariance numbers (target match by T)*/
	for(i=0; i<ncar; ++i) {
		for(j=0; j<ncar; ++j) {
			t=-1;
			n=itmat[i][j];
			for(x=cnt1; x<cnt2; x++) {
				if(n==cand[x].T) {
					n=-1;
					break;
				}
			}
			if(n !=-1)
				t=get_nt(n,itmat,ncar);
		}
		if(t==ncar) {/*match found, load data on cand struct*/
			cand[nc].T=n;
			cand[nc].I=false;
			if(nc !=0){
				cand[nc].no=no;
				cand[nc].re=re;
			}
			nc++;cnt2++;
		}
	}
	*cnt=nc;

	free(itmat);
	
	return;
}
// ----------------------------------------------------------------------------
//	Devuelve el numero de veces que aparece un valor n en una matriz de s*s
//  
// ----------------------------------------------------------------------------
int get_nt(int n,int **itmat, int s)
{
int i,j, result=0;

for(i=0; i<s; ++i) {
	for(j=0; j<s; ++j) {
		if(itmat[i][j]==n)
			++result;
	}
}
return(result);
}
// ----------------------------------------------------------------------------
//	Chequea si la union de los PCS "start" y "end" forman un PCS de la misma SC de "norm"
//  Devuelve true o false
// ----------------------------------------------------------------------------
int ch_close(PCS *start, PCS *end, PCS *norm)
{
	PCS U;

	make_U(start, end, &U);
	if(NombresIguales(&U,norm)) 
		return(true);
	else
		return(false);	
}
