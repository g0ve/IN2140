//ruterdrift.c
//

struct ruter{
    unsigned char ruterID;
    unsigned char FLAGG;
    unsigned char length;
    char[] modell;
}

int main(){
    unsigned char buffer[255];
    FILE *ptr;

    ptr = fopen("test.bin","rb");  // r for read, b for binary

    fread(buffer,sizeof(buffer),1,ptr); // read 10 bytes to our buffer
}
