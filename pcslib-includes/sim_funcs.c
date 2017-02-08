/*sim_funcs.c by Oscar Pablo Di Liscia*/
/*
                    Modificado por Antonio Ortega Brook, Septiembre 2016;
                    todos los cambios están señalados con //-
 */
/*
float sim(int *icv1, int *icv2);
float asim(int *icv1, int *icv2);
float icvsim(int *icv1, int *icv2);
int fr0(int *icv1, int *icv2);
int fr1(int *icv1, int *icv2);
int fr2(int *icv1, int *icv2);
void frp(PCS *pcs1, PCS *pcs2, int *ordarray);
void roi_dis_scat_cc(PCS *pcs1, PCS *pcs2, SIM *sim);
float opcss(PCS *pcs1, PCS *pcs2);
float eval_dyad_order(PCS *pa,*pb);
float rho_calc(int n, float *l1, float *l2);
*/


// --------------------------------------------------------------------
//	Calcula el coeficiente de similitud SIM(Morris) para dos ICV
// --------------------------------------------------------------------
float sim(int *icv1, int *icv2)
{
	
	int i;
	float result=0.;
    
	for(i = 0; i < ICVL; i++){
		result+=(float)abs(icv1[i]-icv2[i]);
    } 
	
	return(result);	
}
// --------------------------------------------------------------------
//	Calcula el coeficiente de similitud ASIM(Morris) para dos ICV
// --------------------------------------------------------------------
float asim(int *icv1, int *icv2)
{
	int i;
	float result=0.;
	float normfac=0.;
	
	for(i = 0; i < ICVL; i++){
		result+=(float)abs(icv1[i]-icv2[i]);
		normfac+=(float)(icv1[i]+ icv2[i]);
	}
	
	return(result/normfac);
}
// --------------------------------------------------------------------
//	Calcula el coeficiente de similitud ICVSIM(Isaacson) para dos ICV
// --------------------------------------------------------------------
float icvsim(int *icv1, int *icv2)
{
	float result=0., idvm=0.;
	int i;
	float idv[6];
	
	for(i=0; i<ICVL; ++i) idv[i]=0.;
	
	for(i=0; i<ICVL; ++i) {
		idv[i]+=(float)(icv1[i]-icv2[i]);
		idvm+=idv[i];
	}
	
	idvm/=6.;
	
	for(i=0; i<6; ++i) {
		result+=((idv[i]-idvm)*(idv[i]-idvm));
	}
	
	return(sqrt((double)(result/6.))); 
}
// --------------------------------------------------------------------
//	Calcula similitud r0(Forte) para dos ICV(retorna true o false)
// --------------------------------------------------------------------
int fr0(int *icv1, int *icv2)
{
	int cnt=0, i;
	int result=false;
	
	for(i = 0; i < ICVL; i++){
		if(icv1[i] != icv2[i]) 
			cnt++;
	}
	if(cnt==6) result=true;
	return(result);
}
// --------------------------------------------------------------------
//	Calcula similitud r1(Forte) para dos ICV(retorna true o false)
// --------------------------------------------------------------------
int fr1(int *icv1, int *icv2)
{
	int cnt=0,cntdos=0, i;
	int ri1[2], ri2[2];
	
	for(i = 0; i < ICVL; ++i){
		if(icv1[i] == icv2[i]) 
			cnt++;
		else{
			if(cntdos < 2) {
				ri1[cntdos]=icv1[i];
				ri2[cntdos]=icv2[i];
			}
			cntdos++;
		}
	}
	if(cnt==4) {
		if(cntdos==2 && ri1[0]==ri2[1] && ri1[1]==ri2[0] )
			return(true);
		else /*is r2*/
			return(false);
	}
	
	return(false);
}
// --------------------------------------------------------------------
//	Calcula similitud r2(Forte) para dos ICV(retorna true o false)
// --------------------------------------------------------------------
int fr2(int *icv1, int *icv2)
{
	int cnt=0,cntdos=0, i;
	int ri1[2], ri2[2];
	
	for(i = 0; i < ICVL; ++i){
		if(icv1[i] == icv2[i]) 
			cnt++;
		else{
			if(cntdos < 2) {
				ri1[cntdos]=icv1[i];
				ri2[cntdos]=icv2[i];
			}
			cntdos++;
		}
	}
	
	if(cnt==4) {
		if(cntdos==2 && ri1[0]==ri2[1] && ri1[1]==ri2[0])/*is r1*/
			return(false);
		return(true);
	}
	
	return(false);
}
// --------------------------------------------------------------------
//	Calcula similitud rp(Forte) para dos ICV
// (carga #ordinal de las SC en comun)
// --------------------------------------------------------------------
void frp(PCS *pcs1, PCS *pcs2, int *ordarray)
{
	int i,j, cnt=0 /* , nr */;              //- unused variable 'nr'
	PCS tempcs1, tempcs2;
	int ord[3][12];
	
	for(i=0; i<MAX_SIM_SUBS; ++i) {
		ordarray[i]=0;
	}
	
	for(i = 0; i < pcs1->ncar; i++){
		cnt=0;
		for(j = 0; j < pcs1->ncar; j++){
			if(j!=i) {
				tempcs1.find[cnt]=pcs1->fprima[j];
				tempcs2.find[cnt]=pcs2->fprima[j];
				cnt++;
			}
		}
		tempcs1.find[cnt]=EOC;
		tempcs2.find[cnt]=EOC;
		forma_prima(&tempcs1, tableptr); 
		forma_prima(&tempcs2, tableptr); 
		ord[0][i]=tempcs1.nord;
		ord[1][i]=tempcs2.nord;
	}
	cnt=0;
	for(i = 0; i < pcs1->ncar; i++){
		for(j = 0; j < pcs1->ncar; j++){
			if(ord[0][i]==ord[1][j]){
				ordarray[cnt++]=ord[0][i];
			}
		}
	}
	
	return;
}
// --------------------------------------------------------------------
//	Calcula Order Inversions para dos PCS (Babbitt, 1960), Displacement
//  y Scattering(Morris, 1987) y Coeficiente de correlacion(CC)
// --------------------------------------------------------------------
void roi_dis_scat_cc(PCS *pcs1, PCS *pcs2, SIM *sim)
{
	int i,x, /* result=0, */ tfac=0;        //- unused variable 'result'
	int pcs[2][PCSL];
	int ord[PCSL];
	int cnt=0,cnt1=0,cnt2=0, n1,n2,nr;

	/*check initial constraints*/
	
	/*must be a permutation of the same PCS*/
	if(NombresIguales(pcs1,pcs2)==false) {
		sim->oi=sim->dis=sim->scat=NC;
		sim->cc=(float)NC;
		return;
	}
	else{ /*can't be an inversion of the same PCS*/
		if(pcs1->I != pcs2->I ) 
		{
			sim->oi=sim->dis=sim->scat=NC;
			sim->cc=(float)NC;
			return;
		}	
	}
	/*must be of the same size*/
	while(pcs1->find[cnt] !=EOC) {
		if(pcs1->find[cnt] != EOP) {
			pcs[0][cnt1]=pcs1->find[cnt];
			cnt1++;
		}
		cnt++;
	}
	cnt=0;
	while(pcs2->find[cnt] !=EOC) {
		if(pcs2->find[cnt] != EOP) {
			pcs[1][cnt2]=pcs2->find[cnt];
			cnt2++;
		}
		cnt++;
	}
	if(cnt1 != cnt2)
	{
		sim->oi=sim->dis=sim->scat=NC;
		sim->cc=(float)NC;
		return;
	}
	/*
	Because we deal with order relations only,
	if the PCS are the same, but with different
	transpositions, transpose the copy of pcs2(pcs[1][n])
	so it matches pcs1.
	*/
	if(pcs1->T != pcs2->T) {
		tfac= pcs1->T - pcs2->T;
		for(i=0; i<cnt2; ++i) {
			pcs[1][i]=(pcs[1][i]+tfac)%12;
			if(pcs[1][i] < 0) pcs[1][i]=12-abs(pcs[1][i]);
		}
	}
	
	/*start evaluation*/
	for(i=0; i<cnt1; ++i) {
		for(x=0; x<cnt1; ++x) {
			if(pcs[0][i]==pcs[1][x]) {
				ord[x]=i;
				break;
			}
		}
		
	}
	/*order inversions, displacement, scattering*/
	sim->oi=sim->dis=sim->scat=0;
	sim->cc=0.;
	/*OI and DIS*/
	for(i=0; i<cnt1; ++i) {
		for(x=i+1; x<cnt1; ++x) {
			if(ord[i] > ord[x] )
				sim->oi++;	
		}
		sim->dis+=abs(i-ord[i]);
	}
	/*SCAT*/
	for(i=0; i<cnt1-1; ++i) {
		for(x=0; x<cnt1; ++x) {
			if(ord[x]==i) {
				n1=x; break;
			}
		}
		for(x=0; x<cnt1; ++x) {
			if(ord[x]==i+1) { 
				n2=x; break;
			}
		}
		sim->scat+=abs(n1-n2);
	}
	sim->scat-=(cnt1-1);
	
	/*correlation coefficient*/
	n1=n2=nr=0;                     //- nr inicializada a 0
	for(i=0; i<cnt1; ++i) {     //- for(i=0; i<cnt1; ++i)
		n1+=i*i;
		n2+=i*ord[i];
		nr+=i;                      //- variable 'nr' is uninitialized when used here
	}
    
	nr=(nr*nr) / cnt1;
	n1-=nr;
	n2-=nr;
	sim->cc=(float)n2/(float)n1;
	
	/*free(str1);free(str2);*/
    return;
}
// --------------------------------------------------------------------
// Calcula OPCSS	
// Ordered Pitch-Class Set Similarity (Pablo Di Liscia)
// --------------------------------------------------------------------
void opcss(PCS *pcs1, PCS *pcs2, float *SPC, float *SAS)
{
	PCS temp1, temp2, A, B;
	int sub_scal;
	int cnt, nA,nB, nsA, nsB;
	int cnt1,cnt2;
//	float result=0.;                //- unused variable 'result'
	int ssa[2][256], ssb[2][256];
	int i,j,x,k, dba,tint;
	
	*SAS=0.;
	*SPC=0.;
	
	cnt=nA=nB=0;
	/*make a copy without EOPs and count elements of each OPCS*/
	while(pcs1->find[cnt] !=EOC) {
		if(pcs1->find[cnt] != EOP) {
			temp1.find[nA]=pcs1->find[cnt];
			nA++;
		}
		cnt++;
	}
	temp1.find[nA]=EOC;
	cnt=0;
	while(pcs2->find[cnt] !=EOC) {
		if(pcs2->find[cnt] != EOP) {
			temp2.find[nB]=pcs2->find[cnt];
			nB++;
		}
		cnt++;
	} 
	temp2.find[nB]=EOC;
	forma_prima(&temp1, tableptr);
	forma_prima(&temp2, tableptr);
	/*smaller PCS is always A, greater or equal, always B*/
	if(nA <= nB) {
		
		A=temp1;B=temp2;
	}
	else {
		A=temp2;B=temp1;
		tint=nB;
		nB=nA;
		nA=tint;
	}
	/*
	position 1 stores the size of the subsets
	position 0 stores the number of the subsets of that size
	nsA and nsB stores the total of adyacent subsets.
	sub_scal is the sumatory of nsA(n)*n
	where n is the size of the subset
	*/
	nsA=nsB=0;
	for(i=0; i < nA-1; ++i) {
		ssa[1][i]=nA-i;
		ssa[0][i]=i+1;
		nsA+=ssa[0][i];
	}
	dba=nB-nA;
	sub_scal=0;
	for(i=0; i < nA-1; ++i) {
		ssb[1][i]=nA-i;
		ssb[0][i]=i+dba+1;
		nsB+=ssb[0][i];
		sub_scal+=ssa[0][i]*ssa[1][i];
	}
	/*
	Find out first the Pitch-Classes shared between the two OPCSS
	*/
	cnt=0;
	for(i=0; i<nA; ++i) {
		for(j=0; j<nB; ++j) {
			if(A.find[i]==B.find[j]) {
				cnt++;
				break;
			}
		}
	}
	
	
	/*Now do the adjacent subsets comparison */
	for(i=0; i<nA-1; ++i) {
		cnt1=cnt2=0; 
		for(j=0; j<ssa[0][i]; ++j) { 
			
			for(k=0; k<ssa[1][i]; ++k) { /*copy subsets of A*/
				temp1.find[k]=A.find[k+cnt1]; 
			}
			temp1.find[k]=EOC;
			forma_prima(&temp1,tableptr);
			
			for(x=0; x < ssb[0][i]; ++x) {
				for(k=0; k<ssb[1][i]; ++k) { /*copy subsets of B*/
					temp2.find[k]=B.find[k+cnt2]; 
				}
				temp2.find[k]=EOC;
				forma_prima(&temp2,tableptr);
				if(NombresIguales(&temp1,&temp2)){ /*COMPARE*/		
					if(temp1.ncar != 2) {
						if(temp1.I != temp2.I) /*Non-matching inversions scores 50% only*/
							*SAS+=(float)ssa[1][i]*0.5;
						else
							*SAS+=(float)ssa[1][i];
					}
					else { /*cardinal 2 is a special case*/
						if(temp1.nord==6)
							*SAS+=(float)ssa[1][i];
						else
							*SAS+=(float)ssa[1][i]*eval_dyad_order(&temp1,&temp2);
					}
					break;		
				}
				cnt2++;
			}
			cnt1++;
			cnt2=0;
		} /*j*/
	}/*i*/
	/*scale SPC and SAS*/
	*SPC=(float)cnt/(float)nA; 
	*SAS /= (float)sub_scal;

	return;
	
}
// --------------------------------------------------------------------
// Evalua orden de 2 PCS  
// retorna 1 si ambos coinciden en direccion y 0.5 si no coinciden.
// --------------------------------------------------------------------
float eval_dyad_order(PCS *pa,PCS *pb)
{
//	float result=0.;                        //- unused variable 'result'
	int i, pca1,pca2,pcb1,pcb2,d1,d2;
	
	pca1=pa->find[0];
	pcb1=pb->find[0];
	for(i=0; i<PCSL; ++i) {
		if(pa->find[i] == EOC)
			break;
		if(pa->find[i] != pca1 ) {
			pca2=pa->find[i];
		}
		if(pb->find[i] != pcb1 ) {
			pcb2=pb->find[i];
		}
	}
	
	if(pca1 < pca2)
		pca1+=12;
	if(pcb1 < pcb2)
		pcb1+=12;
	d1= pca1-pca2;
	d2= pcb1-pcb2;

	if(d1 != d2) 
		return(0.5);
		
	return(1.);
}

// ------------------------------------------------------------------------
// Calcula el Coeficiente de correlacion RHO entre dos vectores de largo n
// ------------------------------------------------------------------------
float rho_calc(int n, float *l1, float *l2) {

	float rho=0., num=0., den;
	float **roar;
	int *aux;
	int i,j;

	roar=(float**) malloc(sizeof(float*)*6);
	for(i=0; i<6; ++i) {
		roar[i]=malloc(sizeof(float)*n);
	}
	aux=(int*) malloc(sizeof(int)*n);

	for(i=0; i<n; ++i) {
		aux[i]=(int)l1[i];
	}
	qsort((void *)aux, n, sizeof(aux[0]), sort_function); 
	/*step1, 2 and 3*/
	for(i=0; i<n; ++i) {
		roar[0][i]=(float)aux[i];
		roar[1][i]=l2[i];
		roar[2][i]=(float)i+1.;
	}
	/*step 4*/
	for(i=0; i<n; ++i) {
		aux[i]=(int)l2[i];
	}
	qsort((void *)aux, n, sizeof(aux[0]), sort_function); 
	for(i=0; i<n; ++i) {
		for(j=0; j<n; ++j) {
			if((int)l2[j]==aux[i]) {
				roar[3][i]=(float)j+1;
			}
		}
	}
	/*prune order repetitions here*/

	/*step5, 6 and 7*/
	for(i=0; i<n; ++i) {
		roar[4][i]=roar[2][i]-roar[3][i];
		roar[5][i]=roar[4][i]*roar[4][i];
		num+=roar[5][i];
	}
	num*=6.;
	den=(float)n*((float)n*(float)n-1.);
	rho=1. - (num/den);
	
	free(roar);
	free(aux);
	return(rho);
}