//ruterdrift.c
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binoper.h"

#define FEIL_INNLESNING "Feil innlesning. Ble ikke lest"
#define MAX_CHILDREN 10

struct ruter** alleRutere;
struct ruter* memory;
int N;
FILE* fileInfo;
FILE* fileCommands;
char buffer[255];
char buffer2[255];
char bufferC[255];

struct ruter{
  unsigned char ruterID;
  unsigned char FLAGG;
  unsigned char length;
  char modell[253];

  int visited;

  size_t number_of_children;
  struct ruter *koblinger[MAX_CHILDREN];
};

struct ruter* ruter_create(unsigned int count){
  return (struct ruter*) malloc(count * sizeof(struct ruter));
}


//-------------------------------ÅPNER FIL-------------------------------------
FILE* openFile(char* fileName){
  FILE* tmpfile = fopen(fileName, "r");

  if (tmpfile == NULL) {
    printf("Fant ikke fil\n");
    exit(EXIT_FAILURE);
  };
  return tmpfile;
}
//-----------------------------------------------------------------------------

//------------------------FINN ANTALL RUTER-----------------------------------
void number_of_ruters(){
  size_t read1 = fread(buffer, sizeof(N), 1, fileInfo);
  if((read1 = 0)){
    printf("%s\n",FEIL_INNLESNING);
  }

  N = buffer[0];
  printf("Antall rutere: %d\n",N);
}
//------------------------------------------------------------------------------

//------------------------------SKRIVER TIL RUTERE-----------------------------
void writeRuters(){
  struct ruter* lstRuter[N];
  for (int i = 0; i < N; ++i) {
    lstRuter[i] = memory + i;
  };

  int teller = 0;
  while(teller < N){
    fgets(buffer, 255, fileInfo);

    size_t read2 = fread(buffer2, 3, 1, fileInfo);
    if(read2 == 0){
      printf("%s\n",FEIL_INNLESNING);
      break;
    }

    lstRuter[teller] -> ruterID = buffer2[0];
    lstRuter[teller] -> FLAGG = buffer2[1];
    lstRuter[teller] -> length = buffer2[2];

    int len = (*lstRuter[teller]).length;

    char tmpModel[len];

    size_t read3 = fread(tmpModel, len-1, 1, fileInfo);
    if(read3 == 0){
      printf("%s\n",FEIL_INNLESNING);
      break;
    }

    for (int i = 0; i < len-1; i++) {
      lstRuter[teller] -> modell[i] = tmpModel[i];
    }

    printf("Ruter ID: %d, FLAGG: %u, Lengde: %d, Navn: %s\n",
    lstRuter[teller] -> ruterID,
    lstRuter[teller] -> FLAGG,
    lstRuter[teller] -> length,
    lstRuter[teller] -> modell);
    teller++;
  }

}
//-----------------------------------------------------------------------------

//----------------------------KOBLER RUTERE-----------------------------------
void connectRuters(){
  while(fgets(buffer, 255, fileInfo) != NULL){

    char buffer3[3];
    size_t read4 = fread(buffer3,2, 1, fileInfo);
    if(read4 != 0){
      printf("Ruter %d -> Ruter %d\n",buffer3[0], buffer3[1]);

      int id1 = buffer3[0];
      int id2 = buffer3[1];
      size_t n = alleRutere[id1] -> number_of_children;

      for (size_t i = 0; i <= n; i++) {
        alleRutere[id1] -> koblinger[n] = alleRutere[id2];
        // printf("RUTER %ld\n", n);
        alleRutere[id1] -> number_of_children++;
        // printf("INDEX %ld\n", ruters[id1] -> number_of_children);

      }
    }
  }
  // printf("%d\n", ruters[9] -> koblinger[0] ->ruterID);
}
//----------------------------------------------------------------------------

//--------------------------PRINTER UT RUTER-----------------------------------
void printRuter(int id){

  printf("Ruter ID: %d, FLAGG: %u, Lengde: %d, Navn: %s\n",
  alleRutere[id] -> ruterID,
  alleRutere[id] -> FLAGG,
  alleRutere[id] -> length,
  alleRutere[id] -> modell);
}
//-----------------------------------------------------------------------------

//-------------------------SETT FLAGG-------------------------------------------
void sett_flag(int id, int idFlagg, unsigned char bytt){
  byte_t tmpFlagg = alleRutere[id] -> FLAGG;
  // printf("FÅR INN: ");
  // print_binary(tmpFlagg);

  if(idFlagg == 0 || idFlagg == 1 || idFlagg == 2){
    if(GET_BIT_N(tmpFlagg, idFlagg) != bytt){
      TOGGLE_BIT_N(tmpFlagg, idFlagg);
    }
  }

  if(idFlagg == 3){
    printf("Ugyldig! Kan ikke endre flagg 3...\n");
  }

  if(idFlagg == 4){
    byte_t bytt1 = bytt;
    //SETTER DET VI SKAL BYTTE TIL DE FØRSTE FIRE BIT
    //START :0000 1010
    bytt1 = bytt1 << 4;
    //FÅR : 1010 0000

    //NULLE UT 4 BITENE TIL FLAGGET
    //START :1011 0101
    tmpFlagg = tmpFlagg & 0x0f;
    //SLUTT :0000 0101

    //LEGGE BYTET INN I FLAGGET
    //tmpFlagg = 0000 0101
    //bytt = 1010 0000
    tmpFlagg = tmpFlagg | bytt1;
    //TILSAMMEN = 1010 0101


  }
  // printf("FERDIG:");
  // print_binary(tmpFlagg);
  alleRutere[id] -> FLAGG = tmpFlagg;
}
//-----------------------------------------------------------------------------

//--------------------------SETT MODELL NAVN-----------------------------------
void sett_modell(int id, char* name){

  for (size_t i = 0; i < strlen(name); i++){
    alleRutere[id] -> modell[i] = name[i+1];
  }
}
//------------------------------------------------------------------------------

//-------------------------SETT KOBLING----------------------------------------
void legg_til_kobling(int id, int id2){
  int ant = alleRutere[id] -> number_of_children;

  if(ant < MAX_CHILDREN){
    printf("KJORER\n");
    alleRutere[id] -> koblinger[ant] = alleRutere[id2];
    alleRutere[id] -> number_of_children++;
  }else{
    printf("Maks antall koblinger er oppnad\n");
  }

}
//-----------------------------------------------------------------------------

//-------------------------SLETTER EN RUTER-----------------------------------
// void slett_ruter(int id){
//   // struct ruter** tmpListe;
//   // struct ruter* tmpMemory;
//   //
//   // struct ruter* tmpLstRuter[N-1];
//   // tmpMemory = ruter_create(N-1);
//   // for (int i = 0; i < N; ++i) {
//   //   tmpLstRuter[i] = tmpMemory + i;
//   // };
//   // tmpListe = tmpLstRuter;
//   //
//   // for (int i = 0; i < N; i++) {
//   //   if(alleRutere[i]-> ruterID != id){
//   //     tmpListe[i] = alleRutere[i];
//   //   }
//   // }
//   // alleRutere = tmpListe;
//   //
//   // free(tmpMemory);
// }
//------------------------------------------------------------------------------

//-------------------------FINNES RUTE VEI------------------------------------
void finnes_rute(int s){
  if(alleRutere[s]-> visited != 1){
    alleRutere[s] -> visited = 1;
    int ant = alleRutere[s] -> number_of_children;
    // printf("ANTALL: %d\n", ant);

    for(int i = 0; i < ant-1; i++){
      int neste = alleRutere[s]->koblinger[i]->ruterID;
      finnes_rute(neste);
    }
  }
}
//---------------------------------------------------------------------------

//---------------------------------SKRIVER TIL FIL----------------------------
void skrivTilFil(){
  FILE *fileSkriv = fopen("ruter.txt", "w+");
  if (fileSkriv == NULL) {
    printf("Fant ikke fil\n");
    exit(EXIT_FAILURE);
  };

  int skriver = 0;
  while (skriver < 10) {
    char skrivBuffer[511];

    snprintf(skrivBuffer, sizeof(skrivBuffer), "\n%d %u %d %s",
    alleRutere[skriver] -> ruterID,
    alleRutere[skriver] -> FLAGG,
    alleRutere[skriver] -> length,
    alleRutere[skriver] -> modell);
    size_t written = fwrite(skrivBuffer, 7 + alleRutere[skriver] -> length, 1, fileSkriv);
    skriver++;
  }
}
//---------------------------------------------------------------------------

int main(int argc, char **argv){

  if(argc != 3){
    printf("Legg til fil navn som argument\n");
    exit(EXIT_FAILURE);
  }

  fileInfo = openFile(argv[2]);
  fileCommands = openFile(argv[1]);


  number_of_ruters();

  struct ruter* lstRuter[N];
  memory = ruter_create(N);
  for (int i = 0; i < N; ++i) {
    lstRuter[i] = memory + i;
  };
  alleRutere = lstRuter;


  if( memory == NULL ){
    printf("FEIL med allokering\n");
  }

  writeRuters();
  connectRuters();
//------------------------------------------------------------------------------
  while(!feof(fileCommands)){
    int id;
    int id2;
    int flaggId;

    char command[20];



    fscanf(fileCommands, "%s", command);

    //HVIS DEN VIL PRINTE
    if (strcmp("print", command) == 0) {
      char tmpId[1];
      fscanf(fileCommands, "%s", tmpId);
      id = strtol(tmpId, NULL, 10);

      printRuter(id);
    }

    //HVIS DEN VIL ENDRE FLAGG
    if(strcmp("sett_flag", command) == 0){
      char tmpId[1];
      fscanf(fileCommands, "%s", tmpId);
      id = strtol(tmpId, NULL, 10);

      char tmpFlaggId[1];
      fscanf(fileCommands, "%s", tmpFlaggId);
      flaggId = strtol(tmpFlaggId, NULL, 10);

      unsigned char tmpBytt[1];
      fscanf(fileCommands, "%s", tmpBytt);

      sett_flag(id, flaggId, tmpBytt[0]);
    }

    if(strcmp("sett_modell", command) == 0){
      char tmpId[1];
      fscanf(fileCommands, "%s", tmpId);
      id = strtol(tmpId, NULL, 10);

      char tmpModell[253];
      fscanf(fileCommands, "%[^\n]", tmpModell);
      char* modell;
      strtol(tmpModell, &modell, 3);

      sett_modell(id, modell);
    }

    if(strcmp("legg_til_kobling", command) == 0){
      char tmpId1[1];
      fscanf(fileCommands, "%s", tmpId1);
      id = strtol(tmpId1, NULL, 10);

      char tmpId2[1];
      fscanf(fileCommands, "%s", tmpId2);
      id2 = strtol(tmpId2, NULL, 10);

      legg_til_kobling(id, id2);
    }

    if(strcmp("slett_router", command) == 0){
      char tmpId1[1];
      fscanf(fileCommands, "%s", tmpId1);
      id = strtol(tmpId1, NULL, 10);

      // slett_ruter(id);
    }

    if(strcmp("finnes_rute", command) == 0){
      char tmpId1[1];
      fscanf(fileCommands, "%s", tmpId1);
      id = strtol(tmpId1, NULL, 10);

      char tmpId2[1];
      fscanf(fileCommands, "%s", tmpId2);
      id2 = strtol(tmpId2, NULL, 10);

      if (id == id2){
        printf("Det er den samme ruteren...\n");
      }

      for (int i = 0; i < N; i++){
        alleRutere[i] -> visited = 0;
      }

      finnes_rute(id);

      // if(alleRutere[id2] -> visited == 1){
      //   printf("Rute mellom rute: %d og rute: %d finnes.\n", id, id2);
      // }else{
      //   printf("Fant ikke rute mellom: %d og %d\n", id, id2);
      // }
    }
  }

  printf("%d\n", lstRuter[0].ruterID);
  skrivTilFil();

  fclose(fileCommands);
  fclose(fileInfo);
  // free(memory);
}
