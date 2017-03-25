

#include <util.h>

/*
        Check if a given set-class exist or not
*/
int exist(int ncar, int nord)
{
        if (ncar < 3 || ncar > 9)
                return -1;              // invalid ncar
        
        if (nord < 1 || nord > max_ord[ncar])
                return -2;              // invalid nord

        return 0;
}
