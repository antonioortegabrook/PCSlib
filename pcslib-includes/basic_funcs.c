int transpose(int *vector, int nelem, int tf);
// Returns in place
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
// Returns in place
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