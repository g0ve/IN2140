#include <stdio.h>
#include <time.h>

struct person{
    int age;
    char name[20];
};

struct person * person_create (void){
    int* ptr = (int*)malloc(24);

    if(ptr == 0){
        printf("Malloc failed\n");
        exit(-1);
    }
}

int main(void){
    struct person jonas;

    jonas.name[0] = 'J';
    jonas.name[1] = 'o';
    jonas.name[2] = 'n';
    jonas.name[3] = 'a';
    jonas.name[4] = 's';

    jonas.age = 32;

}