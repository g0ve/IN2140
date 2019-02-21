//ruterdrift.c
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ruter{
    unsigned char ruterID;
    unsigned char FLAGG;
    unsigned char length;
    char modell[];
};

int main(int argc, char **argv){
  FILE* file;
  char buffer[255];
  int N;

  if(argc != 2){
    printf("Legg til fil navn som argument\n");
    exit(EXIT_FAILURE);
  }


  file = fopen(argv[1], "rb");
  if (file == NULL) {
    printf("Fant ikke fil\n");
    exit(EXIT_FAILURE);
  }

  fgets(buffer, 255, file);
  N = buffer[0];
  printf("%d\n",N);


  while (fgets(buffer, 255, file)) {
    int len = strlen(buffer);
    for (int i = 1; i < len; i++) {
      printf("ID: %d", buffer[0]);
    }
    printf("\n");
  }

  fclose(file);
}
