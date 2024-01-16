#include <stdio.h>
char base4Number[5];
void convertToBase4(long binaryNumber);
long Number;

int main() {
int mask = 1 << 11; // Start with the most significant bit (12th bit)
unsigned short int userInput;
    // Get input from the user
    printf("Enter a 12-bit number (0 to 4095): ");
    scanf("%hu", &userInput);

    // Validate the input
    if (userInput < 0 || userInput > 0xFFF) {
        printf("Invalid input. Please enter a number between 0 and 4095.\n");
        return 1; // Exit with an error code
    }

    // Extract the lower 12 bits
    
    // Display the entered number


    for (int i = 0; i < 12; ++i) {
        int bit = (userInput & mask) ? 1 : 0;
        printf("%d", bit);
	Number=Number*10+bit;

        mask >>= 1; // Shift the mask to the right
    }

    printf("\n");
 printf("%ld The long Number\n", Number);

convertToBase4(Number);

//mask = 1 << 11;
    for (int i = 4; i >= 0; i--) {
        	
        printf("%c", base4Number[i]);

        //mask >>= 1; // Shift the mask to the right
    }
printf("\n");

    //printf("You entered: %x\n", userInput & 0xFFF);





} 

void convertToBase4(long binaryNumber) {

int i=0;
//int base4Number = 0;


    if (binaryNumber < 0) {
        printf("Invalid input. Please provide a non-negative binary number.\n");
        return;
    }

   
    

    while (i<=5) {
switch(binaryNumber % 100) {
case 00: base4Number[i]='*';
	binaryNumber=binaryNumber/100;
	i++;
	//printf("****.\n");
	break;

case 01: base4Number[i]='#';
	binaryNumber=binaryNumber/100;
	i++;
	//printf("####.\n");
	break;

case 10: base4Number[i]='%';
	binaryNumber=binaryNumber/100;
	i++;
	//printf("BLabla%.\n");
	break;

case 11: base4Number[i]='!';
	binaryNumber=binaryNumber/100;
	i++;
	//printf("!!!!!.\n");
	break;

default: 
printf("%ld\nError occur while connnnverting to 4Base.\n",binaryNumber);
i++;
break;
}//closing the switch
}// closeing while


   // return 0;
}


