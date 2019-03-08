//ruterdrift.c
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binoper.h"

#define FEIL_INNLESNING "Feil innlesning. Ble ikke lest"
#define MAX_CHILDREN 10

struct ruter{
  unsigned char ruterID;
  unsigned char FLAGG;
  unsigned char length;
  char modell[253];
  //Int som er 0 eller 1, (false eller true) om hvorvidt ruter er besøkt
  int visited;
  //Antall barn/koblinger
  int antall_koblinger;
  struct ruter* koblinger[MAX_CHILDREN];
};
//Global dobbelt peker som har oversikt over alle structene
struct ruter** alleRutere;
int N;
FILE* fileInfo;
FILE* fileCommands;
//Buffer som ikke brukes bortsett fra å finne antall, brukes i fgets for linje skift
//Sikker bedre måte og gjøre det på men funket for meg
char tmpBuffer[255];
//Buffer som brukes av fread gjennom koden. Her legger jeg all info om hver struct
char fread_buffer[255];

//-----------------------------------------------------------------------------
//fileName - filnavet som skal åpnes og leses av
FILE* openFile(char* fileName);
void number_of_ruters();
void writeRuters();
void connectRuters();
//id - id til ruteren som vil bli skrevet ut
void printRuter(int id);
//id - id til ruteren som skal endres
//idFlagg er id til flagger 0,1,2 eller 4
//bytt - er hva biten til flaget skal endres til 0-15
void sett_flag(int id, int idFlagg, unsigned char bytt);
//id - id til ruter osm skal endres navn
//name - det nye navnet som skal bli byttet til
void sett_modell(int id, char* name);
//id - iden til ruteren som skal få en ny kobling
//id2 - iden til ruteren som skal bli koblet til id
void legg_til_kobling(int id, int id2);
//slettId er iden til ruteren som skal slettes
void slett_ruter(int slettId);
void finnes_rute();
void skrivTilFil();
//-----------------------------------------------------------------------------
//--------------------------MAIN-----------------------------------------------
int main(int argc, char **argv){
  if(argc != 3){
    printf("Legg til fil navn som argument\n");
    exit(EXIT_FAILURE);
  }

  for (int p = 0; p < 3; p++) {
    printf("MOHAHAH\n");
  }
  //Åpner filene som skal leses fra
  fileInfo = openFile(argv[1]);
  fileCommands = openFile(argv[2]);
  //Leser av fil om hvor mange rutere det er
  number_of_ruters();
  //Allokerer nok minne til peker som skal holde oversikt over alle structene
  alleRutere = malloc(sizeof(struct ruter*) * N);
  if( alleRutere == NULL ){
    printf("FEIL med allokering\n");
  }
  //Leser structs fra fil og skriver+allokerer structene
  writeRuters();
  connectRuters();
// //------------------------------------------------------------------------------
  while(!feof(fileCommands)){
    int kommandoId; //Hvis det er en id involvert i kommanoden, er dette den første id'en
    int kommandoId2; //Hvis der er 2 id'er involvert i kommandoen, er dette den andre
    int kommandoFlaggId;
    char command[20];
    //Leser inn den første stringen som sier noe om hvilken kommando det er
    fscanf(fileCommands, "%s", command);
    //I hver kommando er det en id etter selve kommandoen derfor henter jeg iden her
    char tmpId1[1];
    fscanf(fileCommands, "%s", tmpId1);
    kommandoId = strtol(tmpId1, NULL, 10); //Kan sikkert bruke vanlig casting(sikkert enkelere), men dette funket
    //---------------------------------------------------------------------------------------------
    //HVIS DEN VIL PRINTE
    if (strcmp("print", command) == 0) {
      printRuter(kommandoId);
    }
    //HVIS DEN VIL ENDRE FLAGG
    if(strcmp("sett_flag", command) == 0){
      char tmpFlaggId[1];
      fscanf(fileCommands, "%s", tmpFlaggId);
      kommandoFlaggId = strtol(tmpFlaggId, NULL, 10);
      //Henter inn hvilket tall som skal byttes til 0-15
      unsigned char tmpBytt[1];
      fscanf(fileCommands, "%s", tmpBytt);
      //Kjører funksjon for å endre flagget
      sett_flag(kommandoId, kommandoFlaggId, tmpBytt[0]);
    }
    //Sjekker om kommando er "sett_modell" og kjører så funksojnen for å endre navn
    if(strcmp("sett_modell", command) == 0){
      char tmpModell[253];
      fscanf(fileCommands, "%[^\n]", tmpModell);
      char* modell;
      strtol(tmpModell, &modell, 3);
      //Kjører fuksjon for å endre modell navn
      sett_modell(kommandoId, modell);
    }
    //Sjekker om kommando er "legg_til_kobling" og kjører sp den funksjonen
    if(strcmp("legg_til_kobling", command) == 0){
      char tmpId2[1];
      fscanf(fileCommands, "%s", tmpId2);
      kommandoId2 = strtol(tmpId2, NULL, 10);
      //Kjører funksjon
      legg_til_kobling(kommandoId, kommandoId2);
    }
    //Sjekker om kommanod er "slett_ruter" og kjører den funksjonen
    if(strcmp("slett_router", command) == 0){
      slett_ruter(kommandoId);
    }
    //Sjekker om kommando er "finnes_rute" og kjører den
    if(strcmp("finnes_rute", command) == 0){
      char tmpId2[1];
      fscanf(fileCommands, "%s", tmpId2);
      kommandoId2 = strtol(tmpId2, NULL, 10);
      //Går igjennom alle rutere og setter besøkt til 0 (false)
      int i;
      for (i = 0; i < N; i++){
        alleRutere[i] -> visited = 0;
      }
      //Kjører funksjon
      finnes_rute(kommandoId);
      // if(alleRutere[id2] -> visited == 1){
      //   printf("Rute mellom rute: %d og rute: %d finnes.\n", id, id2);
      // }else{
      //   printf("Fant ikke rute mellom: %d og %d\n", id, id2);
      // }
    }
  }

  skrivTilFil();
  fclose(fileCommands);
  fclose(fileInfo);
  int i;
  for (i = 0; i < N; i++) {
    free(alleRutere[i]);
  }

  free(alleRutere);
}

//-------------------------------ÅPNER FIL-------------------------------------
//Denne funksjonen åpner filen det skal leses av
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
//Leser første linje i fil for å finne antall rutere, og setter den til N
void number_of_ruters(){
  size_t read1 = fread(tmpBuffer, sizeof(N), 1, fileInfo);
  if((read1 = 0)){
    printf("%s\n",FEIL_INNLESNING);
  }

  N = tmpBuffer[0];
}
//------------------------------------------------------------------------------

//------------------------------SKRIVER TIL RUTERE-----------------------------
//Denne funksjonen skriver alle ruterene til egne structer fra fil.
void writeRuters(){
  int teller = 0;
  while(teller < N){

    struct ruter* rute = malloc(sizeof(struct ruter));

    fgets(tmpBuffer, 255, fileInfo);

    size_t read2 = fread(fread_buffer, 3, 1, fileInfo);
    if(read2 == 0){
      printf("%s\n",FEIL_INNLESNING);
      break;
    }

    rute -> ruterID = fread_buffer[0];
    rute -> FLAGG = fread_buffer[1];
    rute -> length = fread_buffer[2];

    int len = rute -> length;

    char tmpModel[len];

    size_t read3 = fread(tmpModel, len-1, 1, fileInfo);

    if(read3 == 0){
      printf("%s\n",FEIL_INNLESNING);
      break;
    }

    for (int i = 0; i < len-1; i++) {
      rute -> modell[i] = tmpModel[i];
    }
    alleRutere[teller] = rute;
    teller++;
  }
}
//-----------------------------------------------------------------------------

//----------------------------KOBLER RUTERE-----------------------------------
//Leser de siste linjene i fil, og legger til alle koblinger mellom rutere.
void connectRuters(){
  while(fgets(tmpBuffer, 255, fileInfo) != NULL){

    char buffer3[3];
    size_t read4 = fread(buffer3,2, 1, fileInfo);
    if(read4 != 0){
      // printf("Ruter %d -> Ruter %d\n",buffer3[0], buffer3[1]);

      int id1 = buffer3[0];
      int id2 = buffer3[1];
      int n = alleRutere[id1] -> antall_koblinger;
      alleRutere[id1] -> koblinger[n] = alleRutere[id2];
      alleRutere[id1] -> antall_koblinger++;
    }
  }
}
//----------------------------------------------------------------------------

//--------------------------PRINTER UT RUTER-----------------------------------
//Skriver ut all info om en gitt ruter
void printRuter(int id){

  printf("Ruter ID: %d, FLAGG: %u, Lengde: %d, Navn: %s\n",
  alleRutere[id] -> ruterID,
  alleRutere[id] -> FLAGG,
  alleRutere[id] -> length,
  alleRutere[id] -> modell);
}
//-----------------------------------------------------------------------------

//-------------------------SETT FLAGG-------------------------------------------
//Endrer flagget til en ruter, med bit opersajoner
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
//Endrer navent til en ruter, med navnet som er gitt
void sett_modell(int id, char* name){

  for (size_t i = 0; i < strlen(name); i++){
    alleRutere[id] -> modell[i] = name[i+1];
  }
}
//------------------------------------------------------------------------------

//-------------------------SETT KOBLING----------------------------------------
//Legger til en kobling mellom to rutere
void legg_til_kobling(int id, int id2){
  int ant = alleRutere[id] -> antall_koblinger;

  if(ant < MAX_CHILDREN){
    alleRutere[id] -> koblinger[ant] = alleRutere[id2];
    alleRutere[id] -> antall_koblinger++;
  }else{
    printf("Maks antall koblinger er oppnad\n");
  }
}
//-----------------------------------------------------------------------------

//-------------------------SLETTER EN RUTER-----------------------------------
//sletter og fjerner koblinger til en gitt rute
void slett_ruter(int slettId){
  //fjerner alle koblinger til ruter
  for (int e = 0; e < N; e++) {
    for (int x = 0; x < alleRutere[e]-> antall_koblinger; x++) {
      if(alleRutere[e]->koblinger[x]->ruterID == slettId){
        for(int y = x+1 ; y < N; y++){
          // printf("FJERN: %d\n", alleRutere[e]->koblinger[y+1]->ruterID);
          alleRutere[e]->koblinger[y-1] = alleRutere[e]->koblinger[y];
        }
        alleRutere[e]->antall_koblinger--;
      }
    }
  }

  if(slettId < 0 || slettId > N){
    printf("Finnes ikke %d", slettId);
  }else{
    free(alleRutere[slettId]);
    for(int i = slettId + 1; i < N; i++){
      alleRutere[i-1] = alleRutere[i];
    }
    N--;
    // for(int i = 0; i < N; ++i){
    //   printf("EN: %u\n", alleRutere + i);
    // }
    // printf("%d\n", N);
    // for (int x = 0; x < N; x++) {
      //   alleRutere[x] = realloc(alleRutere[x], sizeof(struct ruter));
      // }
      //
      // alleRutere = (struct ruter**) realloc(alleRutere, N-1 * sizeof(struct ruter*));
      // for(int e = 0; e < N+1; ++e){
        //   printf("%u\n", alleRutere + e);
        // }
        // printf("%d\n", alleRutere[8]->ruterID);
        // printf("%ld\n", sizeof(tmpMemory));
        // for (int i = 0; i < N-1; i++) {
          //   tmpMemory[i] = alleRutere[i];
          // }
    // alleRutere = &tmpMemory;
    // free(alleRutere[8]);
    // printf("%ld\n", sizeof(alleRutere));
  }
}
//------------------------------------------------------------------------------
//-------------------------FINNES RUTE VEI------------------------------------
//Ser om den kan finne en vei til rute
void finnes_rute(int finnvei){
  if(alleRutere[finnvei]-> visited != 1){
    alleRutere[finnvei] -> visited = 1;
    int ant = alleRutere[finnvei] -> antall_koblinger;
    // printf("ANTALL: %d\n", ant);

    for(int i = 0; i < ant-1; i++){
      int neste = alleRutere[finnvei]->koblinger[i]->ruterID;
      finnes_rute(neste);
    }
  }
}
//---------------------------------------------------------------------------

//---------------------------------SKRIVER TIL FIL---------------------------
//Skriver all info om hver rute til en fil leselig (ikke byte)
void skrivTilFil(){
  FILE *fileSkriv = fopen("ruter", "w+");
  if (fileSkriv == NULL) {
    printf("Fant ikke fil\n");
    exit(EXIT_FAILURE);
  };

  // size_t written4 = fwrite(&N, 1, 4, fileSkriv);
  // size_t written3 = fwrite("\n", sizeof(char), 1, fileSkriv);

  int skriver = 0;
  while (skriver < N) {
    // int navnLen = alleRutere[skriver] -> length;
    //
    // size_t written6 = fwrite(alleRutere[skriver], 3 + navnLen, 1, fileSkriv);
    // size_t written7 = fwrite("\n", sizeof(char), 1, fileSkriv);

    /*Koden som er kommentert ut under er koden som skriver til filen med data som er
    lesbart
    */
    fprintf(fileSkriv, "\n%d %u %d %s",
    alleRutere[skriver] -> ruterID,
    alleRutere[skriver] -> FLAGG,
    alleRutere[skriver] -> length,
    alleRutere[skriver] -> modell);

    skriver++;
  }

  // int e;
  // for (e = 0; e < N; e++) {
    //   int i;
    //   for (i = 0; i < alleRutere[e]->antall_koblinger; i++) {
      //     char bufferKobling[2];
      //     bufferKobling[0] = alleRutere[e]->ruterID;
      //     bufferKobling[1] = alleRutere[e]->koblinger[i]->ruterID;
      //
      //     size_t written8 = fwrite(bufferKobling, sizeof(bufferKobling), 1, fileSkriv);
      //     size_t written9 = fwrite("\n", sizeof(char), 1, fileSkriv);
      //   }
      // }
      fclose(fileSkriv);
    }
//---------------------------------------------------------------------------
//----------------------------------------------------------------------------
