

/*
 Prevents us of building a matrix from a PCS that is bigger than the matrix itself
 */
int check_size(PCS* pcs);




//-------------------------------------
/*
 Prevents us of building a matrix from a PCS that is bigger than the matrix itself
 */
int check_size(PCS* pcs) {
    
    int elepos=0, auxelepos=0, npos=0, i=0;
    
    while(pcs->find[i] != EOC){
        auxelepos=0;
        while(pcs->find[i] != EOP && pcs->find[i] != EOC){
            auxelepos++;
            i++;
        }
        if(elepos<auxelepos) elepos=auxelepos;
        if(pcs->find[i] != EOC) i++;;
        npos++;
    }
    if(npos>NEXTC || (npos==1 && elepos>NEXTC)) {
        //        object_error((t_object*)x, "Max number of positions per CM is %i", NEXTC);
        return(-1);     // too many positions
    }
    if(elepos>NEXTC && npos>1) {
        //        object_error((t_object*)x, "Max number of elements per position in a CM is %i", NEXTC);
        return(-2);     // too many elements per position
    }
    return(1);
}
