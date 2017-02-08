/*
                    Modificado por Antonio Ortega Brook, Septiembre 2016;
                    todos los cambios están señalados con //-
 */

/*
int tto_pc(int pc, int n, int tto);
int ttog_read_group (TTOG *ttog);
int ttog_read_member(TTOG *ttog, int offset);
void ttog_init(TTOG *ttog);
void ttog_perform(TTOG *ttog, int *pcsv, int **mat);
void ttog_perform_on_mat(TTOG *ttog, int **mat);
int pc_ttog_ind(TTOG *ttog, int indx, int pc);
*/
/*****************perform a TTO on a PC*****************/
int tto_pc(int pc, int n, int tto)
{
	if(tto==TO) {
		pc=(pc+n)%12;
		return(pc);
	}
	if(tto==IO) {
		pc=((12-pc)+n)%12;
		return(pc);
	}
	if(tto==MO) {
		pc=(pc*5+n)%12;
		return(pc);
	}
	if(tto==MIO) {
		pc=(pc*7+n)%12;
		return(pc);
	}
	return(-1);
}
/*****************read a group member*******************/
int ttog_read_member(TTOG *ttog, int offset) 
{
	int i,j=0;
	
	for(i=offset; i < offset+ttog->or; ++i) {
		ttog->op[j++]=goptr[i];
	}
	
	return(true);
}
/***************************************************/
int ttog_read_group (TTOG *ttog) {
	int /* aux, */ offset=0,i;              //- unused variable 'aux'
	int nc [5]={NCT1,NCT2,NCT3,NCT4,NCT5};
	int off[5]={T1B,T2B,T3B,T4B,T5B};
	
	
	if(ttog->ty < 1 || ttog->ty > 5 ) 
	{return(false);}
	
	offset=off[(ttog->ty)-1];
	ttog->or=goptr[offset+1];
	
	if(ttog->cl < 1 || ttog->cl > nc[(ttog->ty)-1] )
	{return(false);}
	
	/*locate group on table*/
	for(i=0; i<(ttog->cl)-1; ++i) {
		offset+=(goptr[offset+1]*goptr[offset+2])+3;
	}
	/*group order*/
	ttog->or=goptr[offset+1];
	
	if(ttog->mb < 1 || ttog->mb > goptr[offset+2] )
	{return(false);}
	
	offset+=((ttog->mb-1)*ttog->or)+3;
	ttog_read_member(ttog,offset);
	
	return(true);
}
/************************************************************/
void ttog_init(TTOG *ttog)
{
	int i;
	
	for(i=0; i<MAX_ORDER; ++i) {
		ttog->op[i]=0;
	}
	ttog->op[i]=EOC;            // si el tamaño del array = MAX_ORDER, esta línea escribe en un índice inexistente y eventualmente crashea; agregamos un elemento más al array [MAX_ORDER+1] para EOC.
	ttog->ty=ttog->cl=ttog->mb=ttog->or=0;
	
	return;
}
/************************************************************/
void ttog_perform(TTOG *ttog, int *pcsv, int **mat)
{
	int i, j /*, x */, step=0;          //- unused variable 'x'
	
	
	/*Only T operator groups*/
	if(ttog->ty == 1) {
		for(i=0; i<ttog->or; ++i) {
			for(j=0; j<ttog->or; ++j) {
				mat[i][j]=(pcsv[j]+ttog->op[i])%12;
			}
		}
	}  
	/* 0o0 1o3 2o1 3o4 4o2 5o5    o0 o1 o2 o3 o4 o5*/
	/*IT, MT or IMT operators groups*/
	if(ttog->ty == 2 || ttog->ty == 3 || ttog->ty == 4) {
		for(i=0; i<ttog->or/2; ++i) {			
			for(j=0; j<ttog->or; ++j) {
				mat[i][j]=(pcsv[j]+ttog->op[i])%12;
			}
		}
		for(i=ttog->or/2; i<ttog->or; ++i) {			
			for(j=0; j<ttog->or; ++j) {
				if(ttog->ty==2) {
					mat[i][j]=((12-pcsv[j])+ttog->op[i])%12;
				}
				if(ttog->ty==3) {
					mat[i][j]=((5*pcsv[j])+ttog->op[i])%12;
				}
				
				if(ttog->ty==4) {
					mat[i][j]=((7*pcsv[j])+ttog->op[i])%12;
				}
			}
		}
		
	}
	
	/*T/IT/MT/MI operators groups*/
	if(ttog->ty == 5) {
		step=ttog->or/4;
		for(i=0; i<step; ++i) {
			for(j=0; j<ttog->or; ++j) {
				mat[i][j]=(pcsv[j]+ttog->op[i])%12;
			}		
		}
		for(i=step; i<step+step; ++i) {
			for(j=0; j<ttog->or; ++j) {
				mat[i][j]=((12-pcsv[j])+ttog->op[i])%12;
			}		
		}
		for(i=step*2; i<step*3; ++i) {
			for(j=0; j<ttog->or; ++j) {
				mat[i][j]=((5*pcsv[j])+ttog->op[i])%12;
			}		
		}
		for(i=step*3; i<step*4; ++i) {
			for(j=0; j<ttog->or; ++j) {
				mat[i][j]=((7*pcsv[j])+ttog->op[i])%12;
			}		
		}
	}
	return;
}
/************************************************************/
void ttog_perform_on_mat(TTOG *ttog, int **mat)
{
	int i, j, /* x, */ step=0;             //- unused variable 'x'
	
	
	/*Only T operator groups*/
	if(ttog->ty == 1) {
		for(i=0; i<ttog->or; ++i) {
			for(j=0; j<ttog->or; ++j) {
				mat[i][j]= (mat[i][j]+ttog->op[i])%12;
			}
		}
		return;
	}
	/*IT, MT or IMT operators groups*/
	if(ttog->ty == 2 || ttog->ty == 3 || ttog->ty == 4) {
		for(i=0; i<ttog->or/2; ++i) {			
			for(j=0; j<ttog->or; ++j) {
				mat[i][j]=(mat[i][j]+ttog->op[i])%12;
			}
		}
		for(i=ttog->or/2; i<ttog->or; ++i) {			
			for(j=0; j<ttog->or; ++j) {
				if(ttog->ty==2) {
					mat[i][j]=((12-mat[i][j])+ttog->op[i])%12;
				}
				if(ttog->ty==3) {
					mat[i][j]=((5*mat[i][j])+ttog->op[i])%12;
				}
				
				if(ttog->ty==4) {
					mat[i][j]=((7*mat[i][j])+ttog->op[i])%12;
				}
			}
		}
		return;
	}
	
	/*T/IT/MT/MI operators groups*/
	if(ttog->ty == 5) {
		step=ttog->or/4;
		for(i=0; i<step; ++i) {
			for(j=0; j<ttog->or; ++j) {
				mat[i][j]=(mat[i][j]+ttog->op[i])%12;
			}		
		}
		for(i=step; i<step*2; ++i) {
			for(j=0; j<ttog->or; ++j) {
				mat[i][j]=((12-mat[i][j])+ttog->op[i])%12;
			}		
		}
		for(i=step*2; i<step*3; ++i) {
			for(j=0; j<ttog->or; ++j) {
				mat[i][j]=((5*mat[i][j])+ttog->op[i])%12;
			}		
		}
		for(i=step*3; i<step*4; ++i) {
			for(j=0; j<ttog->or; ++j) {
				mat[i][j]=((7*mat[i][j])+ttog->op[i])%12;
			}
		}
		return;
	}
	return;
}
/*searchs a TTO by its index in a given group  and perform it on a given PC*/
/************************************************************/
int pc_ttog_ind(TTOG *ttog, int indx, int pc)
{
int mult, step;
	
	if(ttog->ty==1) {
		pc=(pc+ttog->op[indx])%12;
		return(pc);
	}
	if(ttog->ty == 2 || ttog->ty == 3 || ttog->ty == 4 ) {
		if(indx < ttog->or/2) {
			pc=(pc+ttog->op[indx])%12;
			return(pc);
		}
		else {
			if(ttog->ty==2) {
				pc=((12-pc)+ttog->op[indx])%12;
				return(pc);
			}
			mult=(ttog->ty==3? 5:7);
			pc=(mult*pc + ttog->op[indx])%12;
			return(pc);
		}
	}
	
	step=ttog->or/4;
	if(indx < step) {
		pc=(pc+ttog->op[indx])%12;
		return(pc);
	}
	if(indx >= step && indx < step*2) {
        post("multiple unsequenced modifications to 'pc'"); //-borrar
		pc=pc=((12-pc)+ttog->op[indx])%12;
		return(pc);
	}
	if(indx >= step*2 && indx < step*3) {
		pc=(5*pc + ttog->op[indx])%12;
		return(pc);
	}
	if(indx >= step*3 && indx < step*4) {
		pc=(7*pc + ttog->op[indx])%12;
		return(pc);
	}

return(-1); /*something wrong happened*/
}