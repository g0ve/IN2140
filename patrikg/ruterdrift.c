//ruterdrift.c
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binoper.h"

#define FEIL_INNLESNING "Feil innlesning. Ble ikke lest"
#define MAX_CHILDREN 10

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

struct ruter{
  unsigned char ruterID;
  unsigned char FLAGG;
  unsigned char length;
  char modell[253];

  int visited;

  int number_of_children;
  struct ruter* koblinger[MAX_CHILDREN];
};
//-----------------------------------------------------------------------------
//fileName - filnavet som skal åpnes og leses av
FILE* openFile(char* fileName);
//Leser og finner antall ruter
void number_of_ruters();
void writeRuters();
void connectRuters();
//id - er en int - id til ruteren som vil bli skrevet ut
void printRuter(int id);
//id - er en int - id til ruteren som skal endres
//idFlagg - er en int - er id til flagger 0,1,2 eller 4
//bytt - er en int - er hva biten til flaget skal endres til 0-15
void sett_flag(int id, int idFlagg, int bytt);
//id - er en int - id til ruter osm skal endres navn
//name - er en char peker - det nye navnet som skal bli byttet til
void sett_modell(int id, char* name);
//id - er en int - iden til ruteren som skal få en ny kobling
//id2 er en int - iden til ruteren som skal bli koblet til id
void legg_til_kobling(int id, int id2);
//slettId - er en int -  er iden til ruteren som skal slettes
void slett_ruter(int slettId);
void finnes_rute();
void skrivTilByteFil(char* name);
void skrivTilTekstFil();
//-----------------------------------------------------------------------------

//--------------------------MAIN-----------------------------------------------
int main(int argc, char **argv){
  if(argc != 3){
    printf("Legg til fil navn som argument\n");
    exit(EXIT_FAILURE);
  }

  fileInfo = openFile(argv[1]);
  fileCommands = openFile(argv[2]);

  number_of_ruters();
  //Her allokerer jeg minne til dobbelt pekeren min som skal holde oversikt over alle structene
  //Vil tippe ca 256 * 10 bytes?
  alleRutere = malloc(sizeof(struct ruter*) * N);
  if( alleRutere == NULL ){
    printf("FEIL med allokering\n");
  }
  //Her kaller jeg funksjon "writeRuters". Den leser inn fra fil of skriver info til struct
  writeRuters();
  //Her kaller jeg funksjon "connectRuters". Den fortsetter å lese av fil og legger inn koblinger mellom rutere
  connectRuters();
//------------------------------------------------------------------------------
/*I denne while løkken kjører jeg igjennom filen med alle kommandoene som skal utføres.
Den leser inn første ord i hver linje, og sjekker så med if setninger hvilken kommando som skal utføres.
Treffer en av dem blir resten av info fra linjen hentet inn (rute-id osv.).
Det blir så sendt med videre til en egen funksjon som gjør oppgaven kommandoen ville.*/
  while(!feof(fileCommands)){
    int kommandoId; //Første rute-ID fra kommando linja
    int kommandoId2; //Andre rute-ID fra kommando linja
    int kommandoFlaggId; //ID'en til flagget som skal endres fra kommando linja (0,1,2,4)
    int kommandoFlaggVerdi; //Verdien som skal bli byttet til
    char command[20]; //Buffer som kommandoen som skal bli kjørt legges inn i

    fscanf(fileCommands, "%s", command); //Leser inn forste ordet fra linja (kommandoen)

    /*Avsnittet under bruker jeg i alle if setningene bare at alle henter ulik info,
    og tilslutt sender det til funksjonen*/
    // char tmpId1[1]; //Buffer for å legge inn id som kommer etter kommandoen
    // fscanf(fileCommands, "%s", tmpId1); //Leser inn rute-ID fra linja som alle har
    // kommandoId = strtol(tmpId1, NULL, 10); //Gjør den om til int (kunne sikkert brukt vanlig casting menmen)

    //HVIS DEN VIL PRINTE
    if (strcmp("print", command) == 0) {
      char tmpId1[1];
      fscanf(fileCommands, "%s", tmpId1);
      kommandoId = strtol(tmpId1, NULL, 10);

      printRuter(kommandoId);
    }

    //HVIS DEN VIL ENDRE FLAGG
    else if(strcmp("sett_flag", command) == 0){
      char tmpId1[1];
      fscanf(fileCommands, "%s", tmpId1);
      kommandoId = strtol(tmpId1, NULL, 10);

      char tmpFlaggId[1];
      fscanf(fileCommands, "%s", tmpFlaggId);
      kommandoFlaggId = strtol(tmpFlaggId, NULL, 10);

      char tmpByttVerdi[1];
      fscanf(fileCommands, "%s", tmpByttVerdi);
      kommandoFlaggVerdi = strtol(tmpByttVerdi, NULL, 10);

      sett_flag(kommandoId, kommandoFlaggId, kommandoFlaggVerdi);
    }
    //HVIS DEN VIL ENDRE NAVN PÅ MODELL
    else if(strcmp("sett_modell", command) == 0){
      char tmpId1[1];
      fscanf(fileCommands, "%s", tmpId1);
      kommandoId = strtol(tmpId1, NULL, 10);

      char tmpModell[253];
      fscanf(fileCommands, "%[^\n]", tmpModell);
      char* modell;
      strtol(tmpModell, &modell, 3);

      sett_modell(kommandoId, modell);
    }
    //HVIS DEN VIL LEGGE TIL KOBLING
    else if(strcmp("legg_til_kobling", command) == 0){
      char tmpId1[1];
      fscanf(fileCommands, "%s", tmpId1);
      kommandoId = strtol(tmpId1, NULL, 10);

      char tmpId2[1];
      fscanf(fileCommands, "%s", tmpId2);
      kommandoId2 = strtol(tmpId2, NULL, 10);

      legg_til_kobling(kommandoId, kommandoId2);
    }
    //HVIS DEN VIL SLETTE EN RUTER
    else if(strcmp("slett_router", command) == 0){
      char tmpId1[1];
      fscanf(fileCommands, "%s", tmpId1);
      kommandoId = strtol(tmpId1, NULL, 10);

      slett_ruter(kommandoId);
    }
    //HVIS DEN VIL FINNE EN RUTE MELLOM RUTERE
    else if(strcmp("finnes_rute", command) == 0){
      char tmpId1[1];
      fscanf(fileCommands, "%s", tmpId1);
      kommandoId = strtol(tmpId1, NULL, 10);

      char tmpId2[1];
      fscanf(fileCommands, "%s", tmpId2);
      kommandoId2 = strtol(tmpId2, NULL, 10);

      int i;
      for (i = 0; i < N; i++){
        alleRutere[i] -> visited = 0;
      }

      finnes_rute(kommandoId);
/*Koden som forteller om den fant rute mellom rutere eller ikke
      if(alleRutere[id2] -> visited == 1){
        printf("Rute mellom rute: %d og rute: %d finnes.\n", id, id2);
      }else{
        printf("Fant ikke rute mellom: %d og %d\n", id, id2);
      }
*/
    }
  }

  //Skriver til filen igjen med oppdatert info
  skrivTilByteFil(argv[1]);
  skrivTilTekstFil();
  //Lukker filen med alle kommandoene
  fclose(fileCommands);

  /*Her free'er jeg alt minne som er blitt allokert.
  Først free'er jeg hver enkelt ruter, med for løkke.
  For så å etter free dobbelt pekkeren jeg hadde som holdt oversikt.
  Spørsmål i obligen "Hvor ble minnet allokert": Det ble allokert på heapen*/
  for (int i = 0; i < N; i++) {
    free(alleRutere[i]);
  }
  free(alleRutere);
}
//----------------------------------------------------------------------------

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
    //Her blir hver rute allokert, plassen er vel 256 for hver struct
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

    char buffer3[3]; //Buffer som leser inn kobling linjene i slutten av filen
    size_t read4 = fread(buffer3,2, 1, fileInfo);
    if(read4 != 0){
      // printf("Ruter %d -> Ruter %d\n",buffer3[0], buffer3[1]);

      int ruteId1 = buffer3[0];
      int ruteId2 = buffer3[1];
      int antKoblinger = alleRutere[ruteId1] -> number_of_children;

      alleRutere[ruteId1] -> koblinger[antKoblinger] = alleRutere[ruteId2];
      alleRutere[ruteId1] -> number_of_children++;
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
void sett_flag(int id, int idFlagg, int bytt){
  byte_t tmpFlagg = alleRutere[id] -> FLAGG;

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
  alleRutere[id] -> FLAGG = tmpFlagg;
}
//-----------------------------------------------------------------------------

//--------------------------SETT MODELL NAVN-----------------------------------
//Endrer navent til en ruter, med navnet som er gitt
void sett_modell(int id, char* name){

  for (size_t i = 0; i < strlen(name); i++){
    alleRutere[id] -> modell[i] = name[i+1];
  }
  alleRutere[id] -> length = strlen(name);
}
//------------------------------------------------------------------------------

//-------------------------SETT KOBLING----------------------------------------
//Legger til en kobling mellom to rutere
void legg_til_kobling(int id, int id2){
  int ant = alleRutere[id] -> number_of_children;

  if(ant < MAX_CHILDREN){
    alleRutere[id] -> koblinger[ant] = alleRutere[id2];
    alleRutere[id] -> number_of_children++;
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
    for (int x = 0; x < alleRutere[e]-> number_of_children; x++) {
      if(alleRutere[e]->koblinger[x]->ruterID == slettId){
          for(int y = x+1 ; y < N; y++){
            // printf("FJERN: %d\n", alleRutere[e]->koblinger[y+1]->ruterID);
            alleRutere[e]->koblinger[y-1] = alleRutere[e]->koblinger[y];
          }
        alleRutere[e]->number_of_children--;
      }
    }
  }

  if(slettId < 0 || slettId > N){
        printf("Finnes ikke %d", slettId);
  }else{
    //Free'er for at en ruter ble slettet, da trenger jeg ikke lengre plassen
    //Ligger på heapen
    free(alleRutere[slettId]);
    for(int i = slettId + 1; i < N; i++){
      alleRutere[i-1] = alleRutere[i];
    }

    N--;
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
  // printf("FINN VEI FRA: %d\n", finnvei);
  if(alleRutere[finnvei]-> visited != 1){
    // printf("Ruter %d var ikke besøkt, men er det NÅ\n", alleRutere[finnvei]->ruterID);
    alleRutere[finnvei] -> visited = 1;
    int ant = alleRutere[finnvei] -> number_of_children;
    // printf("ANTALL: %d\n", ant);

    for(int i = 0; i < ant; i++){
      int neste = alleRutere[finnvei]->koblinger[i]->ruterID;
      finnes_rute(neste);
    }
  }
}
//---------------------------------------------------------------------------

//---------------------------------SKRIVER TIL BYTE FIL---------------------------
//Skriver all info om hver rute til en BYTE fil leselig
void skrivTilByteFil(char* name){
  FILE *fileSkriv = freopen(name, "w+", fileInfo);
  if (fileSkriv == NULL) {
    printf("Fant ikke fil\n");
    exit(EXIT_FAILURE);
  };

  fwrite(&N, 1, 4, fileSkriv);
  fwrite("\n", sizeof(char), 1, fileSkriv);

  int skriver = 0;
  while (skriver < N) {
    int navnLen = alleRutere[skriver] -> length;

    fwrite(alleRutere[skriver], 3 + navnLen, 1, fileSkriv);
    fwrite("\n", sizeof(char), 1, fileSkriv);
    skriver++;
  }

  int e;
  for (e = 0; e < N; e++) {
    int i;
    for (i = 0; i < alleRutere[e]->number_of_children; i++) {
      char bufferKobling[2];
      bufferKobling[0] = alleRutere[e]->ruterID;
      bufferKobling[1] = alleRutere[e]->koblinger[i]->ruterID;

      fwrite(bufferKobling, sizeof(bufferKobling), 1, fileSkriv);
      fwrite("\n", sizeof(char), 1, fileSkriv);
    }
  }
  fclose(fileSkriv);
}
//---------------------------------------------------------------------------
//---------------------------------SKRIVER TIL FIL---------------------------
//Skriver all info om hver rute til en fil leselig (ikke byte)
void skrivTilTekstFil(){
  FILE *fileSkriv = fopen("rutere", "w+");
  if (fileSkriv == NULL) {
    printf("Fant ikke fil\n");
    exit(EXIT_FAILURE);
  };

  int skriver = 0;
  while (skriver < N) {

    fprintf(fileSkriv, "\n%d %u %d %s",
    alleRutere[skriver] -> ruterID,
    alleRutere[skriver] -> FLAGG,
    alleRutere[skriver] -> length,
    alleRutere[skriver] -> modell);

    skriver++;
  }
  fclose(fileSkriv);
}
//---------------------------------------------------------------------------
