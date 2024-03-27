#include <stdio.h>
#include <string.h>

int main() {
    char num[15]="";
    char Output1[8]="";
    char i =13;
    int j;
    printf("Enter 14 bit number\n");
    scanf("%s", &num);
    while (i>=0){
      
    switch (num[i]) {
    case '1':
        switch ((char)num[i-1]) {
            case '1':
                strcat(Output1, "!"); 
                break;
            case '0':
                strcat(Output1, "%"); 
                break;
            default:
                printf("Error cutting the 14 bit\n");
                break;
        }
        break;
    case '0':
        switch ((char)num[i-1]) {
            case '1':
                strcat(Output1, "#"); 
                break;
            case '0':
                strcat(Output1, "*"); 
                break;
            default:
                printf("Error cutting the 14 bit\n");/**/
                break;
        }
        break;
}
i=i-(char)2;

    }
     for (j = 6; j >= 0; j--) {
       
          printf("%c",Output1[j]);
    }
    /* printf("%s",Output1);*/
    return 0;
}