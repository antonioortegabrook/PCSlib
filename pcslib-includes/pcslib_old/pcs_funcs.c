/*pcs_funcs.c by Oscar Pablo Di Liscia and Pablo Cetta*/
/*
                        Modificado por Antonio Ortega Brook, Septiembre 2016;
                        todos los cambios est‡n se–alados con //-
*/

/*prototypes in pcs_funcs.c*/
/*
int  sort_function( const void *a, const void *b);
int  forma_prima(PCS* pcs, int *tableptr);
int	 pcs_table_loc(int card, int ord);
*/
/*printing*/
/*
void pc_2txt(int pc, char*str);
void Pname(PCS *pcs, char *array);
void Picv(PCS *pcs, char *array);
void Pfp(PCS *pcs, char *array);
void Pstat(PCS *pcs, char *array);
void Pset(PCS* pcs, char *str);
void print_pcs(PCS* pcs, char *str);
*/
/*subsets*/
/*
int  Subsets(PCS* pcs, int ns, PCS *buffer, int *tableptr);
int  get_n_subsets(int set, int sub);
int  factorial(int n);
*/
/*misc*/
/*

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
int pcs_find(PCS* pcs1, PCS* pcs2);
*/

// ----------------------------------------------------------------------------
// sort function
// ----------------------------------------------------------------------------
int  sort_function( const void *a, const void *b)
{
	return( strcmp((char *)a,(char *)b) );
}
// ----------------------------------------------------------------------------
// Calcula la forma prima de un conjunto y ubica los datos en la estructura PCS
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Calcula la forma prima de un conjunto y ubica los datos en la estructura PCS
// ----------------------------------------------------------------------------
int forma_prima(PCS* pcs, int *tableptr)
{
	int pa = 0, mat[12], nmat[12], paso, norden, parz = 0, inv = 0, n = 0, nuevoj, otroparz;
	int i = 0, menor, comple, j = 0, c, ndif, h = 0 , k, transp = 0, xerror = 0, x,y, mpc[2]  /*  ,ic*/;    //- unused variable 'ic'
	int result[9][9], dif[9];
	int ubica[] = {2,2,2,2,242,822,1582,2582,3342,3922};
	int cant_pcs[10] = {0,0,0,12,29,38,50,38,29,12};
	
	
	if(pcs->find[0]==EOC) { //NULL SET
		pcs->ncar=0;
		pcs->nord=1;
		for(k = 0; k < AGL; k++) {
			pcs->fprima[k] = 0; 
		}
		pcs->fprima[AGL] = EOC;                             //- revisar
         //   post("pcs_funcs.c Array index 12 is past the end of the array :: 1"); //- borrar
		pcs->T=0;
		pcs->I=0;
		for(k = 0; k < ICVL; k++) {
			pcs->icvmat[k] = 0; 
		}
		return(xerror);
	}
	
	
	/*eliminar EOPs, EOC y repeticiones y hallar nroelems */
	n = 0;
	while(pcs->find[i] != EOC){
		if(pcs->find[i] >= 0)
			result[0][j++] = pcs->find[i]; 
		i++;
	}
	while(n + h < j){
		mat[n] = result[0][n + h]; 
		x = 0;
    
		for(i = n + h + 1;i < j; i++) 
			if(mat[n] == result[0][i]) 
				x = 1;
			if(x == 0) n++; else h++;
	}
	pcs->ncar=n;
	
	/**/
	if(n==12){/*pcs 12-1*/
		pcs->ncar=12;
		pcs->nord=1;
		for(k = 0; k < n; k++) {
			pcs->fprima[k] = k; 
		}
		pcs->fprima[12] = EOC;
         //   post("pcs_funcs.c Array index 12 is past the end of the array :: 2"); //- borrar
		pcs->T=0;
		pcs->I=0;
		for(k = 0; k < ICVL; k++) {
			if(k < 5)
				pcs->icvmat[k] = 12; 
			else
				pcs->icvmat[k] = 6;
		}
		return(xerror);
	}
	
	if(n == 11){				/*pcs 11-1*/
		pcs->ncar=11;
		pcs->nord=1;
		for(k = 0; k < n; k++) {
			pcs->fprima[k] = k; 
			}
		pcs->fprima[11] = EOC;
		for(k = 0; k < 6; k++) {
			if(k==5)
				pcs->icvmat[k] = 5;
			else
				pcs->icvmat[k] = 10;  
		}  
		for(k = 0; k <= 11; k++) { /*find out the missing pc*/
			mpc[0]=0;
			for(y = 0; y < n; y++) {
				if(pcs->find[y]==k)
					++mpc[0];
			}
			if(mpc[0]==0) {
				if(k != 11)
					mpc[0]=k+1;
				else
					mpc[0]=0;
				break;
			}
		}
		pcs->T=mpc[0];
		pcs->I=0;
		return(xerror);
	}
	
	if(n==10){
		mpc[0]=mpc[1]=0;
		i=0;
		for(k = 0; k < 12; k++) { /*find out the two missing pcs*/
			mpc[i]=0;
			for(y = 0; y < n; y++) {
				if(pcs->find[y]==k){
					mpc[i]=1;
					break;
				}
			}
			if(mpc[i]==0) {
				mpc[i]=k;
				++i;
			}
		}
		pcs->nord=abs(mpc[0]-mpc[1]);
		pcs->nord=(pcs->nord <= 6? pcs->nord : 12-pcs->nord);
		for(k = 0; k < 5; k++){
			pcs->icvmat[k]=8;
			if(pcs->nord==k+1) pcs->icvmat[k]+=1;
		}
		if(pcs->nord==6) pcs->icvmat[5]=5;
		else {
			pcs->icvmat[5]=4;
		}
		transp=11; /*revisar transp, esta mal*/
		for(k = 0; k < 12; k++) {
			if(pcs->find[k] < transp) transp=pcs->find[k];
		}
		pcs->T=0;
		pcs->I=0;
		for(k = 0; k < n-(pcs->nord-1); k++) {
			pcs->fprima[k]=k;
		}
		for(i = k; i < n; i++) {
			pcs->fprima[i]=i+1;
		}
		return(xerror);
	}
	

	if(n==1) {
		for(k = 0; k < 6; k++) pcs->icvmat[k] = 0;
		pcs->T=pcs->find[0];
		pcs->I=0;
		pcs->fprima[0] = 0;  pcs->fprima[1] = EOC;
		pcs->ncar=1;
		pcs->nord=1;
		goto final;
	}
	
	if(n == 2){				/*pcs 2-x*/
		pcs->fprima[0] = 0;
		pcs->fprima[1] = mat[1] - mat[0];
		if(pcs->fprima[1] < 0) { pcs->fprima[1] *= -1; transp = -1; }
		if(pcs->fprima[1] > 6) {
			pcs->fprima[1] = 12 - pcs->fprima[1];
			if(transp==-1) transp = mat[0];
			else transp = mat[1];
		}
		else {
			if(transp==-1) transp = mat[1];
			else transp = mat[0];
		}
		pcs->fprima[2] = EOC;
		for(k = 0; k < 7; k++){
			pcs->icvmat[k] = 0;
			if(k == pcs->fprima[1])pcs->icvmat[k - 1] = 1;
		}
		pcs->ncar=2;
		pcs->nord=abs(pcs->fprima[0]-pcs->fprima[1]);
		if(pcs->nord > 6) 
			pcs->nord=12-pcs->nord;
		pcs->T=transp;
		pcs->I=0;
		goto final;
	}
	
	for(i = 0;i < n; i++)nmat[i] = mat[i];	/*copia de trabajo*/
	
	do{
		qsort((void *)mat, n, sizeof(mat[0]), sort_function);  /* ordeno de menor a mayor */
		dif[0] = mat[n - 1] - mat[0];    /* busco menor dif.*/
		menor = dif[0];
		
		for(i = 1; i < n; i++) {
			dif[i] = mat[i - 1] + 12 - mat[i];
			if(dif[i] < menor) menor = dif[i];
		}
		x = comple = 0;
		for(i = 0; i < n; i++) {
			if(dif[i] == menor) {
				comple = 0;
				for(j = 0; j < n; j++) {
					c = i + j;
					if(c > n-1 ) {
						comple = 12; c = i + j - n;
					}
					result[x][j] = mat[c] + comple;
				}
				x++;
			}
		}
		if(x != 1) {
			for(j = 1; j < n; j++){
				menor = 100;
				for(i = 0; i <= x - 1; i++){
					ndif=result[i][j] - result[i][0];
					if (ndif < menor) menor = ndif;
				}
				k = 0;
				for(i = 0; i <= x-1; i++){
					if (result[i][j] - result[i][0] == menor){
						for(h = 0; h < n; h++){
							result[k][h] = result[i][h];
						}
						k++;
					}
				}
				x = k;
				if(k == 1) goto salir;
			}
		}
salir:
		for(i = 0; i < n; i++) result[8][i] = result[0][i] - result[0][0];
		for(j = 0; j <= cant_pcs[n] * 20 - 20; j += 20){	/*busqueda en la tabla*/
			norden = tableptr[ubica[n] + j - 1];
			parz = tableptr[ubica[n] + j + 9];
			otroparz = parz;		//porque despues uso parz para hallar el complemento
			for(k = 0; k < 6; k++) pcs->icvmat[k] = tableptr[ubica[n] + j + 11 + k];
			paso = 0;
			for(i = 0; i < n; i++){
				if(tableptr[ubica[n] + i + j] != result[8][i]) paso = 1;
			}
			if(paso == 0) break;
		}
		if(paso != 0){				/*hay que invertir*/
			inv = 20;
			for(i = 0; i < n; i++){
				mat[i] = 12 - nmat[i];
			}
		}
	}while(paso != 0 && pa++ < 2);  		/*fin del bucle do */
	
	nuevoj = j;
	pcs->tp=j;
	
	for(j = 0; j < n; j++) pcs->fprima[j] = result[8][j]; /*copiar resultado a devolver*/
	pcs->fprima[j] = EOC;	                        /*coloco un EOC*/
	transp = result[0][0];
	if(inv == 20) transp = 12-result[0][0];
	
	pcs->nord=norden;
	pcs->T=transp;
	pcs->I=(inv==20? 1: 0);
	/* deteccion de errores */
	if(pa >= 2 || n < 3 || n > 9) xerror = 1;
	
final:
	if(n < 10) {
		for(i=0;i<6;i++){
			if(pcs->icvmat[i]>9)pcs->icvmat[i]-=10;
		}
	}
	
	return(xerror);
}
// ----------------------------------------------------------------------------------
//	retorna la posicion del bloque de datos de un PCS en la tabla
//  el PCS tiene que tener cardinal >2 y <10, sino retorna -1
// ----------------------------------------------------------------------------------
int	 pcs_table_loc(int card, int ord)
{
	int pos=-1;

	if(card > 9 || card < 3) {
		return(pos);
	}
	
	switch(card) {
	case 3:
		pos=PPCS_3*TABLE_CHUNK_SIZE + TABLE_CHUNK_SIZE*(ord-1);
		break;
	case 4:
		pos=PPCS_4*TABLE_CHUNK_SIZE + TABLE_CHUNK_SIZE*(ord-1);
		break;
	case 5:
		pos=PPCS_5*TABLE_CHUNK_SIZE + TABLE_CHUNK_SIZE*(ord-1);
		break;
	case 6:
		pos=PPCS_6*TABLE_CHUNK_SIZE + TABLE_CHUNK_SIZE*(ord-1);
		break;
	case 7:
		pos=PPCS_7*TABLE_CHUNK_SIZE + TABLE_CHUNK_SIZE*(ord-1);
		break;
	case 8:
		pos=PPCS_8*TABLE_CHUNK_SIZE + TABLE_CHUNK_SIZE*(ord-1);
		break;
	case 9:
		pos=PPCS_9*TABLE_CHUNK_SIZE + TABLE_CHUNK_SIZE*(ord-1);
		break;
	}

	return(pos);

}
// ----------------------------------------------------------------------------------
//	Imprime un PC en un array of chars que se le entrega
// ----------------------------------------------------------------------------------
void pc_2txt(int pc, char*str)
{
	if(pc < 10) {
		sprintf(str,"%d",pc);
		return;
	}
	else {
		if(pc == 10) 
			sprintf(str,"A");
		if(pc == 11) 
			sprintf(str,"B");
	}
	return;
}
// ----------------------------------------------------------------------------------
//	Imprime el nombre de un PCS en un array of chars que se le entrega
// ----------------------------------------------------------------------------------
void Pname(PCS *pcs, char *array)
{

	*array=0;
	
	if(isz(pcs,tableptr)==false) {
		sprintf(array,"%d-%d",pcs->ncar,pcs->nord);
	}
	else{
		sprintf(array,"%d-Z%d",pcs->ncar,pcs->nord);
	}
	
	return;
}
// ----------------------------------------------------------------------------------
//	Imprime el icv de un PCS en un array of chars que se le entrega
// ----------------------------------------------------------------------------------
void Picv(PCS *pcs, char *array)
{
	int i;
	char *num;
	
	num=(char*)malloc(sizeof(char)*4);
	
	*num=0;
	*array=0;
	
	strcat(array,"[");
	for(i=0; i<ICVL; ++i) {
		if(pcs->icvmat[i] >= 10) {
			switch(pcs->icvmat[i]) {
			case 10: 
				strcat(array,"A");
				break;
			case 11:
				strcat(array,"B");
				break;
			case 12:
				strcat(array,"C");
				break;
			}
		}
		else {
			sprintf(num,"%d",pcs->icvmat[i]);
			strcat(array,num);
		}
	}
	strcat(array,"]");
	
	free(num);
	return;
}
// ----------------------------------------------------------------------------------
//	Imprime la forma prima de un PCS en un array of chars que se le entrega
// ----------------------------------------------------------------------------------
void Pfp(PCS *pcs, char *array)
{
	int i;
	char *num;
	
	num=(char*)malloc(sizeof(char)*4);
	
	*num=0;
	*array=0;
	
	for(i=0; i<pcs->ncar; ++i) {
		pc_2txt(pcs->fprima[i],num);
		strcat(array,num);
	}
	
	
	free(num);
	return;
}
// ----------------------------------------------------------------------------------
//	Imprime el Status de un PCS en un array of chars que se le entrega
// ----------------------------------------------------------------------------------
void Pstat(PCS *pcs, char *array)
{
	*array=0;
	
	sprintf(array,"T%d",pcs->T);
	if(pcs->I !=0) {
		strcat(array,"I");
	}
	
	return;
}
// --------------------------------------------------------------------------------
// imprime un PCS en un array de caracteres que se le entregue
// --------------------------------------------------------------------------------
void print_pcs(PCS* pcs, char *str){
	
	char *aux;
	
	aux=(char*)malloc(sizeof(char)*PCSL);
	
	*str=0;
	*aux=0;
	if(pcs->ncar >=1) { 
		strcat(str, "PCS: ");
		Pset(pcs, aux);
		strcat(str,aux);

		strcat(str, " NAME: ");
		Pname(pcs,aux);
		strcat(str,aux);
		
		strcat(str, " PRIME FORM: ");
		Pfp(pcs,aux);
		strcat(str,aux);
		
		strcat(str, " STATUS: ");
		Pstat(pcs,aux);
		strcat(str,aux);
		
		strcat(str, " ICV: ");
		Picv(pcs,aux);
		strcat(str,aux);
	}
	else {
		strcat(str, "NULL SET");
	}
	free(aux);
	return; 
}
// --------------------------------------------------------------------
//	Imprime un Set (sin nombre ni otros datos adicionales)
//  en un array de caracteres que se le entregue
// --------------------------------------------------------------------
void Pset(PCS* pcs, char* str)
{
	int j = 0;
	char num[2];

	*str=0;

	while(pcs->find[j] != EOC) {
		if(pcs->find[j] == EOP) strcat(str," ");
		else{
			pc_2txt(pcs->find[j],num);
			strcat(str,num);
		}
		j++;
	}
		
	return;
}
// --------------------------------------------------------------------------------
// Une dos PCS
// --------------------------------------------------------------------------------
void make_U(PCS *pcs1, PCS *pcs2, PCS *U)
{
	int i, cnt=0;
	
	for(i=0; i<PCSL; ++i) {
		if(pcs1->find[i] == EOC) break;
		if(pcs1->find[i] != EOP)
			U->find[cnt++]=pcs1->find[i];
	}
	for(i=0; i<PCSL; ++i) {
		if(pcs2->find[i] == EOC) break;
		if(pcs2->find[i] != EOP)
			U->find[cnt++]=pcs2->find[i];
	}
	U->find[cnt]=EOC;
	forma_prima(U, tableptr);

	return;
}
// --------------------------------------------------------------------------------
// Encuentra la interseccion entre dos PCS (p1 y p2) y la ubica en el PCS target
// retorna false si la interseccion no existe y true si existe
// --------------------------------------------------------------------------------
int find_intersec(PCS *p1, PCS *p2,PCS *target)
{
	int i, x, cont=0;
	
	for(i=0; i<PCSL; ++i) {
		if(p1->find[i] == EOC) break;
		if(p1->find[i] != EOP) {
			for(x=0; x<PCSL; ++x) {
				if(p2->find[x] == EOC) break;
				if(p1->find[i] == p2->find[x]) {
					target->find[cont++]=p1->find[i];
				}
			}
		}
	}

	if(cont==0) {
		target->find[0]=EOC;	
		return(false);
	}
	else {
		target->find[cont]=EOC;
		forma_prima(target,tableptr);	
		return(true);
	}
}
// --------------------------------------------------------------------------------
// Encuentra el complemento del set pcs, lo carga en comp y averigua su forma prima
// --------------------------------------------------------------------------------
void find_complement(PCS* pcs, PCS* comp, int *tableptr)
{
	int i,x,j,c;
	
	i=0;
	for(j=0; j<12; ++j) {
		c=0; x=0;
		while(pcs->find[x] !=EOC) {
			if(pcs->find[x]==j) {
				++c;
				break;
			}
			++x;
		}
		if(c==0) {
			comp->find[i]=j;
			++i;
		}
	}
	comp->find[i]=EOC; 
	forma_prima(comp,tableptr);
	
	return;
}
// -------------------------------------------------------------------------------
// Retorna ubicacion en la tabla del par z de un PCS, y FALSE si no tiene par z.
// --------------------------------------------------------------------------------
int  isz(PCS *pcs, int *tableptr)
{
	int pos=pcs_table_loc(pcs->ncar,pcs->nord);

	if(pos < 0 || tableptr[ZLOC+pos] ==0) 
		return(false);
	else
		return(tableptr[ZLOC+pos]);
}

// --------------------------------------------------------------------
//	Copia un set.
// --------------------------------------------------------------------
void CopiaSet(PCS* source, PCS* target){
	memcpy(target,source,sizeof(PCS));
}
// ----------------------------------------------------------------------------
//	Determina si dos PCS tienen igual nombre
// ----------------------------------------------------------------------------
/*      Por quŽ comparar ncar y nord en lugar de s’mplemente comparar el ’ndice?    //-
 */
int NombresIguales(PCS* pcs1, PCS* pcs2)
{
	if(pcs1->ncar == pcs2->ncar && pcs1->nord == pcs2->nord)
		return(true);
	else
		return(false);
}
// ----------------------------------------------------------------------------
//	Determina si dos PCS son iguales en contenido, sin importar sus posiciones
// ----------------------------------------------------------------------------
/*      Esta funci—n no reconocer’a PCSs en t/i equivalentes    //-
 */
int SetsIguales(PCS* pcs1, PCS* pcs2)
{
	
 if(NombresIguales(pcs1,pcs2)==true && pcs1->T==pcs2->T && pcs1->I==pcs2->I)
	 return(true);
 else
	 return(false);
}
// --------------------------------------------------------------------
//	Calcula índices de invariancia por TnI de un set
// --------------------------------------------------------------------
void Invariancia(PCS* pcs, int* Invar)
{
	int clean[PCSL], temp[PCSL], i, j, m, k = 0, h = 0, n = 0, invar = 0;
	
 	while(pcs->find[k] != EOC) {
//      if(pcs->find[k] != EOP)clean[h] = temp[h++] = pcs->find[k];     //- original - unseq. mod and access to h
        if(pcs->find[k] != EOP) {
            clean[h] = temp[h] = pcs->find[k];
            h++;
        }
		k++;				//h = nro. de elementos del set
	}
	
	for(j = 0; j < 2; j++){
		if(j){
			k = 0;
			while(k < h) {
				temp[k] = 12 - clean[k];	//invierte
				if(temp[k] == 12) temp[k] = 0;
				k++;
			}
		}
		
		for(i = 0; i < 12; i++){
			for(k = 0; k < h; k++){
				if(i) temp[k] = temp[k] + 1;			//transporta
				if(temp[k] > 11) temp[k] = temp[k] - 12;
			}
			for(k = 0; k < h; k++){				//busca invariancia
				for(m = 0; m < h; m++){
					if(temp[m] == clean[k]) invar++;
				}
			}
			if(invar >= h)Invar[n++] = i + j * INVERT;
			invar = 0;
		}
	}
	Invar[n] = EOC;
}

// --------------------------------------------------------------------
//	Transporta y/o invierte un PCS según índ. de transp. y flag de inv.
// --------------------------------------------------------------------
void TrInvPCS(PCS* pcs, int inv, int index)
{
	int m = 0;
	if(inv == TRUE){
		while(pcs->find[m] != EOC) {      // Invierte
			if(pcs->find[m] != EOP){
				pcs->find[m] = 12 - pcs->find[m];
				if(pcs->find[m] == 12)pcs->find[m] = 0;
			}
			m++;
		}
	}
	m = 0;			     //Transporta
	while(pcs->find[m] != EOC) {
		if(pcs->find[m] != EOP){
			pcs->find[m] += index;
			while(pcs->find[m] > 11)pcs->find[m] -= 12;
		}
		m++;
	}
	
	forma_prima(pcs, tableptr);
	return;
}


// --------------------------------------------------------------------
//	Ordena posiciones de un PCS, de menor a mayor
// --------------------------------------------------------------------

void OrdenaPCS(PCS* pcs)
{
	int i = 0, h, j = 0, n, temp[PCSL];
	int flag = false;
	while(pcs->find[i] != EOC){
		if(pcs->find[i] != EOP)	temp[j++] = pcs->find[i++];
		else{
			qsort((void *)temp, j, sizeof(int), sort_function);
			n = 0;
			for(h = (i - j); h < i; h++) pcs->find[h] = temp[n++];
			j = 0; i++;
			flag = true;
		}
	}
	if(!flag){
		qsort((void *)temp, j, sizeof(int), sort_function);
		n = 0;
//		while(pcs->find[n] != EOC)pcs->find[n] = temp[n++];     //- unsequenced modification and access to n
        while(pcs->find[n] != EOC) {
            pcs->find[n] = temp[n];
            n++;
        }
	}
}
// --------------------------------------------------------------------
// elimina repeticiones y EOP en un PCS
// carga el resultado en un array que se le pase como segundo argumento
// retorna el numero de PCs diferentes en el array
// --------------------------------------------------------------------
int no_rep(PCS* pcs, int *aux) 
{
	int i, j, cnt=0;
	int flag;
	
	for(i=0; i<PCSL; ++i){
		if(pcs->find[i]==EOC) break;
		flag=false;
		for(j=0; j<i; ++j){
			if(pcs->find[i] == aux[j]) {
				flag=true;
				break;
			}
		}
		if(flag==false && pcs->find[i] != EOP) 
			aux[cnt++]=pcs->find[i];
	}
	return(cnt);
}
// --------------------------------------------------------------------
// Compute all the different permutations of a given set
// of size n which is stored in the first n-1 positions
// of a vector pl. The permutations are stored lexicographically 
// in the next locations and so, the user is responsible for the memory
// allocation needed (n*np*sizeof(int))
// n must be <=7
// --------------------------------------------------------------------
void permutations(int *pl, int np, int n)
{ 
	int fac=np/n, fact=0;
	int num, i,x,j, temp=0, tempi=0;
	int index=0;
	int s[8];

	if(n > 7) return;
	
	for(num = 0; num <np  ; num++){
		fact=fac;
		for(x=0; x<n; ++x) 
			s[x]=pl[x];
		for(i = 0; i < n - 1; i++){
			tempi = (num / fact) % (n - i);
			temp = s[i + tempi];
			for(j = i + tempi; j > i; j--) {
				s[j] = s[j-1];
			}
			s[i] = temp; 
			fact /= (n - (i + 1));	
		}
		for(x=0; x<n; ++x) {
			pl[index+x]=s[x];
		}
		index+=n;
	}		
	return;
}
// --------------------------------------------------------------------
// Compute the permutation of order "num" (lexicographycal order) of a given set
// of size n which is stored in the first n-1 positions
// of a vector pl. The permutation will be stored in the next n locations of pl.
// --------------------------------------------------------------------
void get_permutation(int *pl, int np, int n, int num)
{
	int fac=np/n;
	int i,x,j, temp=0, tempi=0;
	int s[12];
	
	/*copy the set to s*/
	for(x=0; x<n; ++x) s[x]=pl[x];
	/*get the permutation of order "num"*/
	for(i = 0; i < n - 1; i++){
		tempi = (num / fac) % (n - i);
		temp = s[i + tempi];
		for(j = i + tempi; j > i; j--) {
			s[j] = s[j-1];
		}
		s[i] = temp; 
		fac /= (n - (i + 1));	
	}

	for(x=0; x<n; ++x) {
		pl[n+x]=s[x];
	}
	
	return;
}
// -------------------------------------------------------------------
//	Procesa comando SUBS. Calcula subsets de 3 a 8 elementos
// -------------------------------------------------------------------
int Subsets(PCS *pcs, int ns, PCS *buffer, int *tableptr){
	PCS spcs;
	PCS tempcs2;
//	FILE *fp2;          //- unused variable 'fp2'
	/*               4/3 5/4 5/3 6/5 6/4 6/3 7/6 7/5  7/4  7/3  8/7 8/6  8/5  8/4  8/3  9/8 9/7  9/6  9/5  9/4  9/3 */
	int subin[22] = {12, 20, 30, 30, 60, 60, 42, 105, 140, 105, 56, 168, 280, 280, 168, 72, 252, 504, 630, 504, 252};
	int h = 0, incrtot = 0, i, NroElemSet = 0, NroElemSubset,m,sub=0,x;
	long n = 0;
	int valor;
	
	NroElemSet   =pcs->ncar;
	NroElemSubset=ns;
	
	if(NroElemSubset >= NroElemSet){
		return(-1);
	}
	/*hacer copia de trabajo eliminando repeticiones, EOP y EOC*/
	for(i = 0; i < PCSL; i++){
		h=0;
		if(pcs->find[i]==EOC) break;
		for(x=i+1; x < PCSL; x++){
			if(pcs->find[x]==EOC) break;
			if(pcs->find[i] == pcs->find[x] )
				++h;
		}
		if(h==0 && pcs->find[i]!=EOP) {
			tempcs2.find[n]=pcs->find[i];
			++n;
		}
	}
	tempcs2.find[n++]=EOC;
	tempcs2.ncar=pcs->ncar;
	
	n = 0;
	for(i = NroElemSet - 1; i > 3; i--)incrtot = incrtot + i - 3;
	incrtot = incrtot + NroElemSet - NroElemSubset - 1;
	
	for(i = 0; i < incrtot; i++) n = n + subin[i];
    for(m = 1; m < subin[incrtot] + 1; m++){
		valor=subsetptr[n];
		n++;
		spcs.find[h++] = tempcs2.find[valor];
		if(h >= NroElemSubset){
			spcs.find[h++] = EOC; 
			spcs.ncar=NroElemSubset;
			h = 0;	i = 0; 
			forma_prima(&spcs, tableptr);
			buffer[sub]=spcs;
			++sub;
		}
    }
	return(0);
}
// -------------------------------------------------------------------
//	Calcula cuantos subsets de n cardinal hay en un set de N
// -------------------------------------------------------------------
int get_n_subsets(int set, int sub)
{
	if(sub==set-1) return(set);
	return(factorial(set)/(factorial(sub)*factorial(set-sub))); 
}
// -------------------------------------------------------------------
//	factorial de n
// -------------------------------------------------------------------
int factorial(int n)
{
	int i, fact=1;
	for(i=1; i<=n; i++) fact*=i;
	return(fact);
}
// ----------------------------------------------------------------------------
//	Busca al pcs1 en el pcs2, retorna true si lo encuentra y false si no
// ----------------------------------------------------------------------------
int pcs_find(PCS* pcs1, PCS* pcs2)
{
	int i, j, cnt=0,n1,n2;
	int v1[12], v2[12];

	if(pcs1->find[0]==EOC || pcs2->find[0]==EOC) return(false);

	n1=no_rep(pcs1, v1); 
	n2=no_rep(pcs2, v2);

	if(pcs1->ncar > pcs2->ncar) return(false);

	for(i=0; i<n1; i++) {
		for(j=0; j<n2; j++) {
			if(v1[i]==v2[j]) {
				++cnt;
				break;
			}
		}
	}
	if(cnt == n1) return(true);
	return(false);	
 }



