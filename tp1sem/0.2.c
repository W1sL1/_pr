//Prozorov PA
#include <stdio.h>
#include <string.h>

int main() {
    int n;
    scanf("%d",&n);
    char s[20];
    sprintf(s,"%d",n); //в строку
    
    int gate=0;
    if (s[0]=='-') {
        gate = 1; //убираем минус
    }
    
    int length=strlen(s)-gate;
    if (length%2!=0) {
        printf("0");
        return 0; //не соотв. усл.
    }
    
    int halff=length/2;
    int leftt=0, rightt=0;
    for (int i=0; i<halff; i++) {
        leftt+=s[gate+i]; //левая часть
    }
    for (int i=halff; i<length; i++) {
        rightt+=s[gate+i]; //правая часть
    }
    
    if (leftt==rightt) {
        printf("1"); //соотв.
    } else {
        printf("0"); //не соотв.
    }
    return 0;
}
