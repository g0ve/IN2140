//ruterdrift.c
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ruter{
    unsigned char ruterID;
    unsigned char FLAGG;
    unsigned char length;
    char modell[177];
};

struct ruter* ruter_create(unsigned int count)
{
	/*Note that the caller MUST check if the allocation was successful*/
	return (struct ruter*) malloc(count * sizeof(struct ruter));
}
//Åpner filer----------------------------------------------------------
FILE* openFile(char* fileName){
  FILE* tmpfile = fopen(fileName, "rb");

  if (tmpfile == NULL) {
    printf("Fant ikke fil\n");
    exit(EXIT_FAILURE);
  };
  return tmpfile;
}
//main
//----------------------------------------------------------------------
int main(int argc, char **argv){
  FILE* file;
  char buffer[255];
  int N;

  if(argc != 2){
    printf("Legg til fil navn som argument\n");
    exit(EXIT_FAILURE);
  }

  file = openFile(argv[1]);

  size_t read2 = fread(buffer, 4, 4, file); // linje 1
  N = buffer[0];
  printf("%d\n",N);


  //while (i < N) {
    //size_t read = fread(buffer, 1, 1, file);
    //char id = buffer[0];
    //char flagg = buffer[0];
    //printf("ID: %d\n",id );
    //printf("FLAGG: %u\n",flagg );
    //printf("test");
    //i++;
  //}

//   struct ruter* ruters[N];
//   struct ruter* memory = ruter_create(N);
//   int i;
//   for (i = 0; i < N; ++i) {
//   	ruters[i] = memory + i;
//   };
    fseek(file, 0, SEEK_END);
    printf("%ld", ftell(file));
    struct ruter ruter1;

    size_t read = fread((void*)&ruter1, sizeof(struct ruter), 1, file);
    fclose(file);
    printf("%d %d %d %s\n", ruter1.ruterID, ruter1.FLAGG, ruter1.length, ruter1.modell);
    //free(memory);

//    fread(&ruter1.ruterID, sizeof(ruter1.ruterID), 1, file);
//    fread(&ruter1.FLAGG, sizeof(ruter1.FLAGG), 1, file);
//    fread(&ruter1.length, sizeof(ruter1.length), 1, file);
//    fread(ruter1.modell, sizeof(ruter1.modell), 1, file);
//
//    printf("%s %s %s %s\n", ruter1.ruterID, ruter1.FLAGG, ruter1.length, ruter1.modell);


  // struct ruter* ruters = malloc(sizeof(struct ruter) * N);
  // int i = 0;
  // while (i < N) {
  //   size_t read = fread((void*)&ruters[i], sizeof(struct ruter), 1, file);
  //   i++;
  // }
  // for (int i= 0; i < N; i++) {
  //   printf("Ruter id: %d, Flagg: %d, Lengde: %d, Navn: %s\n", ruters[i].ruterID, ruters[i].FLAGG, ruters[i].length, ruters[i].modell);
  // }


  // struct ruter* ruter1 = ruters[0];
  // struct ruter ruter1;
  // size_t read = fread((void*)&ruter1, sizeof(struct ruter), 1, file);
  // fclose(file);
  // printf("Ruter id: %u, Flagg: %u, Lengde: %u, Navn: %s\n", ruter1.ruterID, ruter1.FLAGG, ruter1.length, ruter1.modell);

  // struct ruter ruter1;
  // printf("%ld\n", sizeof(struct ruter) );
  // while (fread((void*)&ruter1, sizeof(struct ruter), 1, file)) {
  //   printf("Ruter id: %u, Flagg: %u, Lengde: %u, Navn: %s\n", ruter1.ruterID, ruter1.FLAGG, ruter1.length, ruter1.modell);
  // }
  // printf("%s\n", buffer);
  // long int tell = ftell(file);
  // printf("%ld\n", tell );
  // for (int i = 1; i < N+1; i++) {
  //   ftell(file);
  //   printf("test\n");
  // };

  // while (fgets(buffer, 255, file)) {
  //   printf("Buffer: %s\n",buffer);
  //   for (int i = 0; i < N; i++) {
  //     printf("ID: %d", buffer[0]);
  //   }
  //   printf("\n");
  // }

  fclose(file);
  //free(ruters);
}
