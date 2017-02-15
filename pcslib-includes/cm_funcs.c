/*cm_funcs.c by Oscar Pablo Di Liscia and Pablo Cetta*/
/*
                    Modificado por Antonio Ortega Brook, Septiembre 2016;
                    todos los cambios est‡n se–alados con //-
*/
/*
void CopiaMatriz(CM* cm, CM* cmdest);
int  MaxElemxPos(int*);
void Inicializar(CM* cm);
void cm_ttog(CM *cm, TTOG *ttog, PCS *pcs);
int ** mataloc(int r, int c);

void RomanSq(CM* cm, PCS *pcs);
void MatTipo1a(CM* cm, PCS* pcs);
void MatTipo1b(CM* cm, PCS* pcs);
void MatTipo2(CM* cm,PCS* pcs1,PCS* pcs2);
int  MatFCycles(CM* cm,PCS *pcs, int TnI);
void Vti(int* vector, int TnI);
void OneXPos(int* vector);
void AllXPos(int* vector);

int  check_rc(CM *mat, int r,int c);
void MTransp(CM* cm, int transp);
void InterCols(CM* cm, int col1,int col2);
void InterFilas(CM* cm, int  fil1, int fil2);
void MInver(CM* cm);
void Mti(CM* cm, int indice , int inv);
void cm_tto(CM* cm, int tto , int t);
void RotaDiag(CM* cm, int Flag);
void Rota90(CM* pcm);
int  Swap(CM* cm);
void Densidad(CM* cm, int dens[MAXROWS][MAXROWS]);
void Insertar(CM* cm, int nf, int nc, int valor);
void Borrar(CM* cm, int nf, int nc, int valor);

void pos_to_pcs(CM *cm, PCS* pcs, int r, int c);
void pc_to_pos(CM *cm, int pc, int r, int c)
void pcs_to_pos(CM *cm, PCS* pcs, int r, int c);
void row_to_pcs(CM *cm, PCS* pcs, int r);
void col_to_pcs(CM *cm, PCS* pcs, int c);
void cm_to_pcs (CM *cm, PCS* pcs);
int  pos_to_npcs(CM *cm, int r, int c);
void empty_pos(CM *cm, int r, int c);
int  get_T(CM *cm);
int  get_pairs(int n);
float frag(CM *cm);
float spar(CM* cm);
void hist(CM *cm, float *harray);
*/
/*find repetitions in cols of a matrix*/
/////////////////////////////////////////////////////////////
int find_rep_cols(int **mat,int r, int c, int *vec)
{
	int i, j, x;
	int pc, rf=false;

	for(i=0; i<r; ++i) {
		vec[i]=0;
	}

	for(i=0; i<c; ++i) {
		for(j=0; j<r; ++j) {
			pc=mat[j][i];
			for(x=0; x<r; ++x) {
				if(x != j && pc==mat[x][i]){
					vec[i]+=1;
					rf=true;
				}
			}
		}
	}
	/*
	for(i=0; i<c; ++i) {
		post(" %d ",vec[i]);
	}
	*/
	return(rf);
}
/*allocate a matrix*/
/////////////////////////////////////////////////////////////
int ** mataloc(int r, int c)
{
	int ** ptr=NULL;
	int i;
	
	ptr=(int**)malloc(sizeof(int*)*r);
	for(i=0; i<c; ++i) {
		ptr[i]=(int*)malloc(sizeof(int)*c);
	}
	
	return(ptr);
}
/////////////////////////////////////////////////////////////
/*deallocate a matrix*/
void matdealoc(int ** ptr, int c)
{
	int i;	
	for(i=0; i<c; ++i) {
		free(ptr[i]);
	}
	free(ptr);
	return;
}
/////////////////////////////////////////////////////////////
/*print a position matrix*/
void post_pmat(int **pmat, int r, int c)
{
	int i,j;
	char *str, *num;
	
	str=(char*)malloc(sizeof(char)*1024);
	num=(char*)malloc(sizeof(char)*32);
	*str=0;*num=0;

	for(i=0; i<r; ++i) {
		*str=0;
		for(j=0; j<c; ++j) {
			*num=0;
			if(pmat[i][j] < 0) {
				strcat(str,"#");
			}
			else {
			pc_2txt(pmat[i][j], num);
			strcat(str,num);
			}
		}
		post("%s",str);
	}
	post("\n");
	free(str);free(num);
	return;
}
/////////////////////////////////////////////////////////////
/*post a matrix*/
void post_mat(int **mat, int **mat2, int r, int c)
{
	int i,j,x;
	char *str, *num, *str2;
	
	str=(char*)malloc(sizeof(char)*1024);
	str2=(char*)malloc(sizeof(char)*1024);
	num=(char*)malloc(sizeof(char)*32);
	*str=0;*num=0;

	for(i=0; i<r; ++i) {
		*str=0;
		for(j=0; j<c; ++j) {
			*num=0;
			pc_2txt(mat[i][j], num);
			strcat(str,num);
		}
		*str2=0;
		for(x=0; x<c; ++x) {
			strcat(str2,str);
			strcat(str2,"\t");
		}
		post("%s",str2);
	}
	post("\n");

	*str2=0;
	for(i=0; i<r; ++i) {
		*str=0;
		for(j=0; j<c; ++j) {
			*num=0;
			pc_2txt(mat2[i][j], num);
			strcat(str,num);
		}
		strcat(str2,str);
		strcat(str2,"\t");
	}
	post("%s\n",str2);

	free(str);
	free(num);
	free(str2);
	return;
}
// --------------------------------------------------------------------
//	Construye una CM usando Grupos de Operadores
//  (Morris, 1987, pps. 154-158)
// --------------------------------------------------------------------
int cm_ttog(CM *cm, TTOG *ttog, PCS *pcs)
{
	int i,j,n /* ,indx */ ;         //- unused variable 'indx'
	int **Smat, **Qmat, **Pmat;
	int pcsv[6];
	int pcsn[6];
	int repvec[6];
	int pc1, pc2;
	int repflag=false;
	
	/*allocate memory for matrices*/
	Smat=mataloc(ttog->or, ttog->or);
	Qmat=mataloc(ttog->or, ttog->or);
	Pmat=mataloc(ttog->or, ttog->or);

	for(i=0; i<ttog->or; ++i){
		for(j=0; j<ttog->or; ++j){
			Pmat[i][j]=-1;
		}
	}
	

	/*take out repetitions*/
	no_rep(pcs, pcsn);
	/*perform TTO Group operations on each row of the Matrix*/
	ttog_perform(ttog, pcsn, Smat);
	j=0;
	for(i=0; i<ttog->or; ++i){
		pcsv[j++]=Smat[i][i];
	}
	ttog_perform(ttog, pcsv, Qmat);
	//post_mat(Smat,Qmat,ttog->or,ttog->or);
	repflag=find_rep_cols(Smat,ttog->or, ttog->or, repvec);
	if(repflag==false) {
		/*write CM*/
		/*first col is obvious*/
		for(i=0; i<ttog->or; ++i){
			pc_to_pos(cm,pcsv[i],i,0);
			Pmat[i][0]=i;
		}
		
		for(i=1; i<ttog->or; ++i){
			for(j=0; j<ttog->or; ++j){
				pc1=pc_ttog_ind(ttog,i,pcsv[j]);
				for(n=0; n<ttog->or; ++n){
					pc2=pc_ttog_ind(ttog,n,pcsn[j]);
					if(pc2==pc1 /*&& pos_is_empty(cm,n,i)==true*/) {
						pc_to_pos(cm,pc1,n,i);
						Pmat[n][i]=j;
						break;
					}
				}
			}
		}
		//post_pmat(Pmat,ttog->or,ttog->or);
        cm->MaxElemxPos = 1;    //- revisar
		cm->NroCols=cm->NroFilas=ttog->or;
	}
	
	/*free memory and exit*/
	matdealoc(Smat, ttog->or);
	matdealoc(Qmat, ttog->or);
	matdealoc(Pmat, ttog->or);

	if(repflag==true) {
		return(false);
	}
	else{
		return(true);
	}
}

// --------------------------------------------------------------------
//	Construye  y carga un cuadrado romano en la struct cm
// --------------------------------------------------------------------

void RomanSq(CM* cm, PCS *pcs)
{
	int vector[PCSL];   //- cuidado con esto! el vector es m‡s grande que las posiciones de la matriz
	int salto;
	int nro_filas_col = 0, i = 0, j, k = 0, incr, nro_pos = 0 /* , h=0 */ ;     //- unused variable 'h'
	
	while(pcs->find[i] != EOC){
		vector[i]=pcs->find[i];       //- original: vector[i++]=pcs->find[i]; "unsequenced modification and access to 'i'"
        i++;
	}
	vector[i]=EOP;
	vector[i+1]=EOC;
    
    i=0;                    //-    agregado por m’; es un parche, quiz‡s poco elegante, pero permite obtener
    int eop_count=0;                //-     un elemento por posici—n cuando no se ingresa EOP igual que los otros;
    while(pcs->find[i] != EOC) {    //-     provisorio hasta evaluar si hay una manera m‡s correcta de hacerlo;
        if(pcs->find[i]==EOP)       //-- REVISAR ---
            eop_count++;
        i++;
    }
	if(eop_count==0)
        OneXPos(vector);    //-                 -------end
    
    Inicializar(cm);
	i=0;
	
	while(vector[k] != EOC)	
		if(vector[k++] == EOP)nro_filas_col++;
		k = 0;
		while(vector[k] != EOC){
			if(vector[k] != EOP)cm->mat[i++][0] = vector[k];
			else(i = ++nro_pos * NEXTC);
			k++;
		}
		salto = NEXTC * nro_filas_col;
		for(j = 1; j < nro_filas_col; j++){
			incr = NEXTC;
			for(i = 0; i < salto; i++){
				if(incr >= salto) incr = 0;
				cm->mat[i][j] = cm->mat[incr++][j-1];
			}
		}
		cm->MaxElemxPos = MaxElemxPos(vector);
		cm->NroCols = cm->NroFilas = nro_filas_col;
		return;
}
// --------------------------------------------------------------------
//	Procesa el comando TIPO1A y carga uns matriz en la struct cm
// --------------------------------------------------------------------
void MatTipo1a(CM* cm, PCS *pcs)
{
	int vector[PCSL], temp[PCSL], tempv;
	int nro_filas_col = 0, i = 0, j, k = 0;
	
	while(pcs->find[i] != EOC){
		temp[i]=pcs->find[i];     //- original temp[i++]=pcs->find[i]; "unsequenced modification and access to 'i'"
        i++;
	}
	temp[i]=EOP;
	temp[i+1]=EOC;
	
	OneXPos(temp);
	Inicializar(cm);
	
	i=0; 
	while(temp[k] != EOC)	
		if(temp[k++] == EOP)
			nro_filas_col++;
		k = 0;
		while(temp[k] != EOC){
			if(temp[k] != EOP) 
				vector[i++] = temp[k];
			k++;
		} 
		for(j = 0; j < nro_filas_col; j++){
			for(i = 0; i < nro_filas_col; i++){
				tempv = vector[i] + vector[j];
				if(tempv > 11)tempv = tempv - 12; 
				cm->mat[i * NEXTC][j] = tempv;
			}
		}
		cm->MaxElemxPos = MaxElemxPos(temp);
		cm->NroCols = cm->NroFilas = nro_filas_col;
		return;
}

// --------------------------------------------------------------------
//	Procesa el comando TIPO1B y carga una matriz en la struct cm
// --------------------------------------------------------------------

void MatTipo1b(CM* cm, PCS* pcs)
{
	int vector[PCSL], temp[PCSL], tempi, tempv;
	int nro_filas_col = 0, i = 0, j, k = 0;
	
	while(pcs->find[i] != EOC){
		temp[i]=pcs->find[i];     //- temp[i++]=pcs->find[i]; "unsequenced modification and access to 'i'"
        i++;
	}
	temp[i]=EOP;
	temp[i+1]=EOC;
	
	OneXPos(temp);
	Inicializar(cm); 
	i=0;
	
	while(temp[k] != EOC)	
		if(temp[k++] == EOP)
			nro_filas_col++;
		k = 0;
		while(temp[k] != EOC){
			if(temp[k] != EOP) 
				vector[i++] = temp[k];
			k++;
		}
		
		for(j = 0; j < nro_filas_col; j++){
			for(i = 0; i < nro_filas_col; i++){
				tempi = 12 - vector[j];  
				if(tempi == 12) 
					tempi = 0;
				tempv = vector[i] + tempi;
				if(tempv > 11)
					tempv = tempv - 12;
				cm->mat[i * NEXTC][j] = tempv;
			}
		}
		cm->MaxElemxPos = MaxElemxPos(temp);
		cm->NroCols = cm->NroFilas = nro_filas_col;
		return;
}
// --------------------------------------------------------------------
//	Procesa el comando TIPO2 y carga uns matriz en la struct cm
// --------------------------------------------------------------------
void MatTipo2(CM* cm, PCS *pcs1, PCS *pcs2)
{
	int i, j, k, vector1[PCSL], vector2[PCSL], temp2[PCSL], tempv;
	int temp1[PCSL];
	
	i=0;
	while(pcs1->find[i] != EOC){
		temp1[i]=pcs1->find[i];   //- temp1[i++]=pcs1->find[i];    "unsequenced modification and access to 'i'"
        i++;
	}
	temp1[i]=EOP;    
	temp1[i++]=EOC; 
	
	i=0;
	while(pcs2->find[i] != EOC){
		temp2[i]=pcs2->find[i];   //- temp2[i++]=pcs2->find[i];   "unsequenced modification and access to 'i'"
        i++;
	}
	temp2[i]=EOP;    
	temp2[i++]=EOC;
	
	
	Inicializar(cm);
	OneXPos(temp1); 
	OneXPos(temp2);
	
	i = 0; k = 0;
	while(temp1[i] != EOC){
		if(temp1[i] != EOP) vector1[k++] = temp1[i];
		else cm->NroCols++;
		i++;
	}
	i = 0; k = 0;
	while(temp2[i] != EOC){
		if(temp2[i] != EOP) vector2[k++] = temp2[i];
		else cm->NroFilas++;
		i++;
	}
	for(j = 0; j < cm->NroFilas; j++){
		for(i = 0; i < cm->NroCols; i++){
			tempv = vector1[i] + vector2[j];
			if(tempv > 11)tempv = tempv - 12;
			cm->mat[i * NEXTC][j] = tempv;
		}
	}
	cm->MaxElemxPos = 1;
	return; 
}
// --------------------------------------------------------------------
//	Procesa el comando MCIC y carga una matriz en la struct cm
// --------------------------------------------------------------------
int MatFCycles(CM* cm, PCS *pcs, int TnI)
{
	PCS vector;
	int nro_pos = 0, k, caso = 0, i, j, m, w, x /* , h */ ;      //- unused variable 'h'
	
	i=0;
	while(pcs->find[i] != EOC){
		vector.find[i]=pcs->find[i];  //- vector.find[i++]=pcs->find[i]; "unsequenced modification and access to 'i'"
        i++;
	}
	vector.find[i]=EOP;
	vector.find[i+1]=EOC;
	Inicializar(cm); 
	
	k = 0;
	while(vector.find[k] != EOC)	
		if(vector.find[k++] == EOP)
			nro_pos++;
		
		if(TnI >= INVERT || TnI == 6)caso = 2;
		else if(TnI == 4 || TnI == 8)caso = 3;
		else if(TnI == 3 || TnI == 9)caso = 4;
		else if(TnI == 2 || TnI == 10)caso = 6;
		
		if(nro_pos > caso){
			return(FALSE);
		}
		
		Inicializar(cm);
		cm->NroFilas = cm->NroCols = caso;
		cm->MaxElemxPos = MaxElemxPos(vector.find);
		
		i = j = k = w = 0;
		for(x = 0; x < caso; x++){
			w = x; i = 0;
			for(m = 0; m < caso; m++){
				if(m < nro_pos){
					while(vector.find[k] != EOP){
						cm->mat[i + w * NEXTC][j] = vector.find[k];
						k++; i++;
					}
					i = 0; k++;
					w++;
					if(w >= caso) w = 0;
				}
				else{
					cm->mat[w * NEXTC][j] = ETY;
					w++;
					if(w >= caso) w = 0;
				}
			}
			j++; k = 0;
			Vti(vector.find, TnI);
			OrdenaPCS(&vector);
		}
		return(TRUE);
}

// -----------------------------------------------------------
//	Copia el contenido de una matriz en otra
// -----------------------------------------------------------
void CopiaMatriz(CM* cm, CM* cmdest)
{
	memcpy(cmdest,cm,sizeof(CM));
	return;
}
// -------------------------------------------------------------------------
//	Calcula el número máximo de elementos que hay por posición en una cadena
// -------------------------------------------------------------------------

int MaxElemxPos(int* vector)
{
	int MaxElem = 0, n = 0, i = 0;
	
	while(vector[i] != EOC){
		while(vector[i++] != EOP){
			n++;
			if(n > MaxElem)MaxElem = n;
		}
		n = 0;
	}
	return MaxElem;
}
// -----------------------------------------------------------
//	Inicializa la matriz combinatoria
// -----------------------------------------------------------

void Inicializar(CM* cm)
{
	int i, j,f=0;

/*added the ETY*/	

	for(j = 0; j < MAXROWS; j++){
		for(i = 0; i < MAXELEM; i+=NEXTC) {
			cm->mat[i][j] = ETY;
			for(f = 1; f < NEXTC; f++) {
				cm->mat[i+f][j] = SPAC;
			}
		}
	}
	
	cm->MaxElemxPos = cm->NroCols = cm->NroFilas = 0;
	return;
}
// -----------------------------------------------------------
//	Corrige un vector, ubicando solo un elemento por posicion
// -----------------------------------------------------------
/*
 Revisar: la cantidad m‡xima de elementos en el vector debe ser igual a (PCSL-1)/2; esto es: tantos espacios para EOPs como elementos, m‡s uno para EOC.
 */
void OneXPos(int* vector)
{
	int k = 0, i = 0, temp[PCSL];
	
	while(vector[k] != EOC){
		if(vector[k] != EOP){
			temp[i++] = vector[k];
			temp[i++] = EOP;
		}
		k++;
	}
	k = 0; temp[i] = EOC;
//	while(temp[k] != EOC) vector[k] = temp[k++];            //- original " unsequenced modification and access to ÔkÕ "
    while(temp[k] != EOC) {
        vector[k] = temp[k];
        k++;
    }
	vector[k] = EOC;
}
// -----------------------------------------------------------
//	Corrige un vector, ubicando todo elemento en una posicion
// -----------------------------------------------------------
void AllXPos(int* vector)
{
	int k = 0, i = 0, temp[PCSL];
	
	while(vector[k] != EOC){
		if(vector[k] != EOP)	temp[i++] = vector[k];
		k++;
	}
	k = 0; temp[i] = EOP; temp[i + 1] = EOC;
//	while(temp[k] != EOC) vector[k] = temp[k++];                //- original " unsequenced modification and access to ÔkÕ "
    while(temp[k] != EOC) {
        vector[k] = temp[k];
        k++;
    }
	vector[k] = EOC;
}
// -----------------------------------------------------------
//	Transforma un vector según TnI
// -----------------------------------------------------------
void Vti(int* vector, int TnI){

	int  k = 0;	

	if(!TnI){
		return;
	}
	if(TnI >= INVERT){
		TnI -= INVERT;
		while(vector[k] != EOC){		//Invierte
			if(vector[k] != EOP){
				vector[k] = 12 - vector[k];
				if(vector[k] == 12)vector[k] = 0;
			}
			k++;
		}
	}
	k = 0;
	while(vector[k] != EOC){			//Transporta
		if(vector[k] != EOP){
			vector[k] += TnI;
			if(vector[k] > 11)vector[k] = vector[k] - 12;
		}
		k++;
	}
}
// -----------------------------------------------------------
//chequea que las columnas/filas esten en rango para una CM
// -----------------------------------------------------------
int check_rc(CM *mat, int r,int c)
{
	
	if(c < 0 || c > mat->NroCols) return(FALSE);
	if(r < 0 || r > mat->NroFilas) return(FALSE);
	
	return(TRUE);
}
// -----------------------------------------------------------
//	Transporta la matriz
// -----------------------------------------------------------
void MTransp(CM* cm, int transp)
{
	int i,j;
	
	for(j = 0; j < cm->NroFilas; j++){
		for(i = 0; i < NEXTC * cm->NroCols; i++){
			if(cm->mat[i][j] >= 0)
				cm->mat[i][j] = (cm->mat[i][j] + transp)%12;
		}
	}
	return;
}
// -----------------------------------------------------------
//	Procesa INTERCOLS. Intercambia columnas entre sí.
// -----------------------------------------------------------
void InterCols(CM* cm, int col1,int col2)
{
	int i,j;
	int temp[NEXTC + 1][9];
	
	for(j = 0; j < cm->NroFilas; j++){
		for(i = 0; i < NEXTC; i++){
			temp[i][j] = cm->mat[i + col1 * NEXTC][j];
			cm->mat[i + col1 * NEXTC][j] = cm->mat[i + col2 * NEXTC][j];
			cm->mat[i + col2 * NEXTC][j] = temp[i][j];
		}
	}
	return;
}
// -----------------------------------------------------------
//	Procesa INTERFILAS. Intercambia filas entre sí.
// -----------------------------------------------------------

void InterFilas(CM* cm, int  fil1, int fil2)
{
	int i   /* ,j */ ;              //- unused variable 'j'
	int temp[MAXELEM];
	
	for(i = 0; i < MAXELEM; i++){
		temp[i] = cm->mat[i][fil1];
		cm->mat[i][fil1] = cm->mat[i][fil2];
		cm->mat[i][fil2] = temp[i];
	}
	return;
}
// -----------------------------------------------------------
//	Procesa MINVER. Invierte la matriz
// -----------------------------------------------------------
void MInver(CM* cm)
{
	int i,j;
	for(j = 0; j < cm->NroFilas; j++){
		for(i = 0; i < NEXTC * cm->NroCols; i++){
			if(cm->mat[i][j] >= 0)
				cm->mat[i][j] = (12 - cm->mat[i][j])%12;
		}
	}
	return;
}

// -----------------------------------------------------------
//	Procesa ROTDIAG. Rota la matriz por las diagonales
// -----------------------------------------------------------
void RotaDiag(CM* cm, int Flag)
{
	int i, j, k;
	CM tempcm;
	
	/*Flag TRUE or FALSE means rotation to right->left or to left->right*/
	
	CopiaMatriz(cm, &tempcm);
	for(j = 0; j < cm->NroFilas; j++){
		for(i = 0; i < cm->NroFilas; i++){
			for(k = 0; k < NEXTC; k++){
				if(Flag==TRUE) cm->mat[i * NEXTC + k][j] = tempcm.mat[j * NEXTC + k][i];
				else cm->mat[i * NEXTC + k][j] = tempcm.mat[(cm->NroFilas - 1 - j) * NEXTC + k][cm->NroFilas - 1 - i];
			}
		}
	}
	return;
}
// -----------------------------------------------------------
//	Procesa una CM con T, IT, MT o MIT (Mti queda en desuso)
// -----------------------------------------------------------
void  cm_tto(CM* cm, int tto , int t)
{
	int i, j, mult;
	
	if(tto==IT_O){
		for(j = 0; j < cm->NroFilas; j++){
			for(i = 0; i < NEXTC * cm->NroCols; i++){
				if(cm->mat[i][j] >= 0)
					cm->mat[i][j] = ((12 - cm->mat[i][j])+t)%12;
			}
		}
		return;
	}
	if(tto == MT_O || tto == IMT_O) {
		mult=tto==MT_O? 5 : 7;
		for(j = 0; j < cm->NroFilas; j++){
			for(i = 0; i < NEXTC * cm->NroCols; i++){
				if(cm->mat[i][j] >= 0)
					cm->mat[i][j] = (mult*cm->mat[i][j]+t)%12;
			}
		}
		return;
	}
	if(t != 0) {
		for(j = 0; j < cm->NroFilas; j++){
			for(i = 0; i < NEXTC * cm->NroCols; i++){
				if(cm->mat[i][j] >= 0)
					cm->mat[i][j] = (cm->mat[i][j]+t)%12;
			}
		}
	}
	return;
}
// -----------------------------------------------------------
//	Procesa MTNI. Transporta y/o invierte la matriz según TnI
// -----------------------------------------------------------
void  Mti(CM* cm, int indice , int inv)
{
	int i, j;
	
	if(inv==TRUE){
		for(j = 0; j < cm->NroFilas; j++){
			for(i = 0; i < NEXTC * cm->NroCols; i++){
				if(cm->mat[i][j] >= 0)
					cm->mat[i][j] = (12 - cm->mat[i][j])%12;
			}
		}
	}
	if(indice > 0){
		for(j = 0; j < cm->NroFilas; j++){
			for(i = 0; i < NEXTC * cm->NroCols; i++){
				if(cm->mat[i][j] >= 0)
					cm->mat[i][j] = (cm->mat[i][j] + indice)%12;
			}
		}
	}
	return;
}
// -------------------------------------------------------------------
//	Rota 90 grados la matriz activa
// -------------------------------------------------------------------
void Rota90(CM* cm)
{
	int i, j, k, z, fc;
	CM tempcm;
	
	Inicializar(&tempcm);
	CopiaMatriz(cm, &tempcm);
	
	if(tempcm.NroFilas > tempcm.NroCols) fc = tempcm.NroFilas;
	else fc = tempcm.NroCols;
	for(j = 0; j < fc; j++){
		for(i = 0; i < fc; i++){
			z = (tempcm.NroCols - 1) - j;
			for(k = 0; k < NEXTC; k++){
				cm->mat[i * NEXTC + k][j] = tempcm.mat[z * NEXTC + k][i];
			}
		}
	}
	cm->NroFilas = tempcm.NroCols;
	cm->NroCols = tempcm.NroFilas;
	cm->MaxElemxPos = tempcm.MaxElemxPos;
	
	return;
}
// -------------------------------------------------------------------
//	swapping
// -------------------------------------------------------------------
int Swap(CM* cm)
{
	int flag = FALSE, tipo = 0;
	int j, i, z, w, h, c, d;
	int dens[MAXROWS][MAXROWS];
	
	for(h = 0; h < 2; h++){
		if(h && flag==FALSE) tipo = TRUE;
		for(z = 0; z < cm->NroFilas; z++){
			for(w = 0; w < NEXTC * cm->NroCols; w++){
				if(cm->mat[w][z] != SPAC && cm->mat[w][z] != ETY){
					for(j = 0; j < cm->NroFilas; j++){
						for(i = 0; i < NEXTC * cm->NroCols; i++){
							if(cm->mat[i][j] != SPAC && cm->mat[i][j] != ETY){
								if(i != w && j != z){
									if(cm->mat[i][j] == cm->mat[w][z]){
										Densidad(cm, dens);
										c = (int)(w/NEXTC); 
										d = (int)(i/NEXTC);
										if(((dens[c][j] + tipo) < dens[c][z]) && ((dens[d][z] + tipo) < dens[d][j])){
											flag = TRUE;
											Insertar(cm, j, c, cm->mat[i][j]);
											Borrar(cm, z, c,  cm->mat[i][j]);
											Insertar(cm, z, d, cm->mat[i][j]);
											Borrar(cm, j, d,  cm->mat[i][j]);
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
	if(flag==FALSE)
		return(FALSE); /*no reduction could be done*/
	
	return(TRUE);
}
// -------------------------------------------------------------------
//	Calcula la densidad de las posiciones de una matriz
// -------------------------------------------------------------------
void Densidad(CM* cm, int dens[MAXROWS][MAXROWS])
{
	int cont, i, j;
	
	for(j = 0; j < cm->NroFilas; j++){
		for(i = 0; i < cm->NroCols; i ++){
			if(cm->mat[i * NEXTC][j] == ETY) dens[i][j] = 0;
			else{
				cont = 0;
				while(cm->mat[i * NEXTC + cont][j] != SPAC) cont++;
				dens[i][j] = cont;
			}
		}
	}
	return;
}
// -------------------------------------------------------------------
//	Inserta un elemento en una posición de una matriz
// -------------------------------------------------------------------
void Insertar(CM* cm, int nf, int nc, int valor)
{
	int cont;
	PCS temp;
	
	cont = 0;		//filas y cols desde 0
	if(cm->mat[nc * NEXTC + cont][nf] == ETY){
		cm->mat[nc * NEXTC + cont][nf] = valor;
		return;
	}
	else{
		while(cm->mat[nc * NEXTC + cont][nf] != SPAC){
			temp.find[cont] = cm->mat[nc * NEXTC + cont][nf];
			cont++;
		}
		temp.find[cont++] = valor;
		temp.find[cont++] = EOP;
		temp.find[cont]   = EOC;
		OrdenaPCS(&temp);
		cont = 0;
		while(temp.find[cont] != EOP){
			cm->mat[nc * NEXTC + cont][nf] = temp.find[cont];
			cont++;
		}
	}
	if(cm->MaxElemxPos < cont)cm->MaxElemxPos++;
	return;
}
// -------------------------------------------------------------------
//	Borra un elemento en una posición de una matriz
// -------------------------------------------------------------------
void Borrar(CM* cm, int nf, int nc, int valor)
{
	int cont, h = 0, i, j, maxdens = 0;
	int dens[MAXROWS][MAXROWS];
	PCS temp;
	
	cont = 0;  
	if(cm->mat[nc * NEXTC + cont][nf] == ETY)
		return;
	else{
		while(cm->mat[nc * NEXTC + h][nf] != SPAC){
			if(cm->mat[nc * NEXTC + h][nf] != valor){
				temp.find[cont] = cm->mat[nc * NEXTC + h][nf];
				cont++;
			}
			h++;
		}
		if(h == 1){
			cm->mat[nc * NEXTC][nf] = ETY;
			cm->mat[nc * NEXTC + 1][nf] = SPAC;
			return;
		}
		temp.find[cont++] = EOP;
		temp.find[cont]	= EOC;
		OrdenaPCS(&temp);
		cont = 0;
		while(temp.find[cont] != EOP){
			cm->mat[nc * NEXTC + cont][nf] = temp.find[cont];
			cont++;
		}
		cm->mat[nc * NEXTC + cont][nf] = SPAC;
	}
	Densidad(cm, dens);
	for(j = 0; j < cm->NroFilas; j++){
		for(i = 0; i < cm->NroCols; i++){
			if(maxdens < dens[i][j])maxdens = dens[i][j];
		}
	}
	cm->MaxElemxPos = maxdens;
	return;
}
// -------------------------------------------------------------------
//	obtiene el PCS en una posicion de una CM
//  si la posicion esta vacia retorna con EOC en pcs.find[0]
// -------------------------------------------------------------------
void pos_to_pcs(CM *cm, PCS* pcs, int r, int c)
{
	int j=0, offset,i;
	
	offset=NEXTC*c;
	for(i=0; i<NEXTC; ++i) {
		if(cm->mat[i+offset][r]==ETY) {
			pcs->find[0]=EOC;
			forma_prima(pcs,tableptr); //NULL set is identified anyway
			return; /*empty position*/
		}
		else {
			if(cm->mat[i+offset][r]!=SPAC){
				pcs->find[j++]=cm->mat[i+offset][r];/*get pcs*/
			}
		}
	}
	pcs->find[j]=EOC;
	forma_prima(pcs,tableptr);
	
	return;
}
// -------------------------------------------------------------------
//	retorna true si una posicion de una CM esta vacia y false si no lo esta
// -------------------------------------------------------------------
int pos_is_empty(CM *cm,int r, int c)
{
int /* j=0, */ offset /*,i */;      //- unused variable 'j','i'
	
	offset=NEXTC*c;	
	
	if(cm->mat[offset][r]== ETY) 
		return(true);
	else
		return(false);


}
// -------------------------------------------------------------------
//	Copia solo un PC en una posicion de una CM
// -------------------------------------------------------------------
void pc_to_pos(CM *cm, int pc, int r, int c)
{
	int /* j=0, */ offset,i;
	
	offset=NEXTC*c;	
	
	cm->mat[offset][r]=pc;
	for(i=1; i<NEXTC; i++) {
		cm->mat[i+offset][r]=SPAC;
	}

	return;
}
// -------------------------------------------------------------------
//	Copia un PCS en una posicion de una CM
// -------------------------------------------------------------------
void pcs_to_pos(CM *cm, PCS* pcs, int r, int c)
{
	int j=0, offset,i;
	
	offset=NEXTC*c;

	for(i=0; i<NEXTC; i++) {
		if(pcs->find[i] == EOC) break;
		if(pcs->find[i] != EOP) {
			cm->mat[offset+j][r]=pcs->find[i];
			j++;
		}
	}
	for(i=j; i<NEXTC; i++) {
		cm->mat[i+offset][r]=SPAC;
	}
	return;
}
// -------------------------------------------------------------------
//	"vacia" una posicion de una CM
// -------------------------------------------------------------------
void empty_pos(CM *cm, int r, int c)
{
	int offset,i;
	
	offset=NEXTC*c;

	cm->mat[offset][r]=ETY;
	for(i=1; i<NEXTC; i++) {
		cm->mat[offset+i][r]=SPAC;
	}

	return;
}
// -------------------------------------------------------------------
//	obtiene el PCS en una fila de una CM
//  inserta un EOP por cada cambio de posicion
// -------------------------------------------------------------------
void row_to_pcs(CM *cm, PCS* pcs, int r)
{
	int index=0,i, j;
	
	for(i = 0; i < cm->NroCols*NEXTC; i+=NEXTC){

		for(j=0; j < NEXTC; ++j) {
			if(cm->mat[i+j][r]!=ETY && cm->mat[i+j][r]!=SPAC) {
				pcs->find[index++]=cm->mat[i+j][r];
			}
			else{
				pcs->find[index++]=EOP;
				break;
			}
			if(index == PCSL-1) break; 
		}
		if(index == PCSL-1) break; 
		
	}
	pcs->find[index]=EOC;
	/*take out the final EOPs if any*/
	for(i=index; i >=0; --i) {
		if(pcs->find[i]==EOP) pcs->find[i]=EOC;
		if(pcs->find[i]!=EOP && pcs->find[i]!=EOC) {
			break;
		}
	}
	forma_prima(pcs,tableptr);
	return;
}
// -------------------------------------------------------------------
//	obtiene el PCS en una columna de una CM
//inserta un EOP por cada cambio de posicion
// -------------------------------------------------------------------
void col_to_pcs(CM *cm, PCS* pcs, int c)
{
	int index=0,j,i;
	int offset=c*NEXTC;
	
	for(i = 0; i < cm->NroFilas; i++){
		
		for(j=0; j < NEXTC; ++j) {
			if(cm->mat[offset+j][i]!=ETY && cm->mat[offset+j][i]!=SPAC) {
				pcs->find[index++]=cm->mat[offset+j][i];
			}
			else{
				pcs->find[index++]=EOP;
				break;
			}
			if(index == PCSL-1) break; 
		}
		if(index == PCSL-1) break; 	
	}

	pcs->find[index]=EOC;
	/*take out the final EOPs if any*/
		for(i=index; i >=0; --i) {
		if(pcs->find[i]==EOP) pcs->find[i]=EOC;
		if(pcs->find[i]!=EOP && pcs->find[i]!=EOC) {
			break;
		}
	}
	forma_prima(pcs,tableptr);
	
	return;
}
// -------------------------------------------------------------------
//	obtiene el PCS en una CM
// -------------------------------------------------------------------
void cm_to_pcs (CM *cm, PCS* pcs)
{
	int j, i;
	int index=0;
	
	for(j = 0; j < cm->NroFilas; j++){
		for(i = 0; i < NEXTC * cm->NroCols; i++){
			if(cm->mat[i][j] != ETY && cm->mat[i][j] != SPAC){
				pcs->find[index++]=cm->mat[i][j];
			}
			if(index == PCSL-1) break; 
		}
		if(index == PCSL-1) break; 
	}
	pcs->find[index]=EOC;
	forma_prima(pcs,tableptr);
	return;
}

// -------------------------------------------------------------------
//	obtiene la cantidad de PCs en una posicion de una CM
// -------------------------------------------------------------------
int pos_to_npcs(CM *cm, int r, int c)
{
	int /* j=0, */ offset,i,n=0;
	
	offset=NEXTC*c;
	for(i=0; i<NEXTC; ++i) {
		if(cm->mat[i+offset][r]==ETY) break; /*empty position*/
		else {
			if(cm->mat[i+offset][r]!=SPAC){
				++n;/*get number of  pcs*/
			}
		}
	}
	return(n);
}

// -------------------------------------------------------------------
//	obtiene T (cantidad de PCs en todas las posiciones de una CM)
// -------------------------------------------------------------------
int get_T(CM *cm)
{
	int T=0 /* ,n=0 */;         //- unused variable 'n'
	int j, i;
	
	for(j = 0; j < cm->NroFilas; j++){
		for(i = 0; i < cm->NroCols; i++){
			T+=pos_to_npcs(cm, j, i);
		}
	}
	return(T);
}
// -------------------------------------------------------------------
//  calcula PAIRS (cantidad de PARES de PCs en un numero dado de PCs)
// -------------------------------------------------------------------
int get_pairs(int n)
{
	return( (n*n-n)/2);
}
// -------------------------------------------------------------------
//  FRAG calcula el coeficiente de fragmentacion de una CM
// -------------------------------------------------------------------
float frag(CM *cm)
{
	int T=0, pT=0;
	int E=0, pE=0,pEsum=0;
	int j, i;
	
	T= get_T(cm);
	pT=get_pairs(T);
	
	for(j = 0; j < cm->NroFilas; j++){
		for(i = 0; i < cm->NroCols; i++){
			E=pos_to_npcs(cm,j,i);
			pE=get_pairs(E);
			pEsum+=pE;
		}
	}
	return(1. - ((float)pEsum/(float)pT));
}
// -------------------------------------------------------------------
//	Calcula el coeficiente de esparcimiento(sparseness) de una matriz
// -------------------------------------------------------------------
float spar(CM* cm)
{
	int i, j;
	int N=cm->NroFilas*cm->NroCols;
	int F=N;
	
	for(j = 0; j < cm->NroFilas; j++){
		for(i = 0; i < cm->NroCols; i++){
			if(cm->mat[i * NEXTC][j] == ETY) --F;
		}
	}

	return((float)(N-F)/(float)N);
}
// -------------------------------------------------------------------
//	Calcula el Histograma de PCs de una CM
// -------------------------------------------------------------------
void hist(CM *cm, float *harray)
{
	int /* cont, */ i, j,x,p;               //- unused variable 'cont'
	
	for(i=0; i<12; ++i) harray[i]=0;
	
	for(j = 0; j < cm->NroFilas; j++){
		for(i = 0; i < cm->NroCols; i ++){
			if(cm->mat[i][j] != ETY){	
				for(p=0; p<NEXTC; p++) {
					for(x=0; x<12; ++x) {
						if(cm->mat[i*NEXTC+p][j]==x && cm->mat[i*NEXTC+p][j]!=SPAC) harray[x]+=1.;
					}
				}
			}
		}
	}
	return;
}

