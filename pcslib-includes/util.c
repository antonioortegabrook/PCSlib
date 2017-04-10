

//#include <util.h>



/*
        Check if a given set-class exists or not
*/
int pcs_not_exist(int ncar, int nord)
{
        if (ncar < MIN_CAR || ncar > MAX_CAR)
                return -1;              // invalid ncar
        
        if (nord < 1 || nord > max_ord[ncar])
                return -2;              // invalid nord

        return 0;
}
