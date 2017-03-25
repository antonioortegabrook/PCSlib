int transpose(int *vector, int nelem, int tf);
/**     Transpose a PCS. Puts results in input
                @ parameters: pointer to an array (int), # of elements
                @ returns: error code -1 if array is null
 */
int transpose(int *vector, int nelem, int tf)
{
      if (!vector)
            return -1;
      
      for (int i=0; i<nelem; i++) {
            if (vector[i] != EOP && vector[i] != EOC)
                  vector[i] += tf;
            
            if (vector[i] < 0)
                  vector[i] += 12;
            
            else if (vector[i] > 12)
                  vector[i] = vector[i] % 12;
            
      }

      return 0;
}

int invert(int *vector, int nelem);
/**     Invert a PCS. Puts results in input
                @ parameters: pointer to an array (int), # of elements
                @ returns: error code -1 if array is null
 */
int invert(int *vector, int nelem)
{
      if (!vector)
            return -1;
      
      for (int i=0; i<nelem; i++) {
            if (vector[i] != EOP && vector[i] != EOC)
                  vector[i] = 12 - vector[i];
            
            if (vector[i] == 12)
                  vector[i] = 0;
            
      }

      return 0;
}

//- borrar!!!
int bla(int x)
{
        if (w)
                return z;
}
