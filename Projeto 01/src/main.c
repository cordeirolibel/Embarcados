#include <stdio.h>
#include <stdint.h>

extern uint8_t median(uint32_t dim,uint8_t vec_in[], uint8_t vec_out[]);


int main(){
  uint32_t dimension = 8;
  uint8_t resposta;   
  uint8_t vetor_in[] = {7,6,5,4,3,2,1,22};
  uint8_t vetor_out[8];
  resposta = median(dimension,vetor_in,vetor_out);
  
  for (int i=0; i<dimension;i++) {
    printf("%d\n", vetor_out[i]);
  }
  
  printf("mediana: %d\n", resposta);
  return 0;
}
