#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NOTIFY 10
#define DEBUG 0
#define ITERAZIONI 6



void leggi_prime_otto_righe();
void find_first_two_nops();
void read_gaps();
void read_two_lines();

int main(){
	printf("\n");
	leggi_prime_otto_righe();
	find_first_two_nops();
	read_two_lines();
	read_gaps();
	return 0;
}

void leggi_prime_otto_righe(){
	char line[200];
	for (int i = 0; i < 8; i++){
		gets(line);
		if (DEBUG){
			printf("%s\n", line);
		}
	}
}

void find_first_two_nops(){
	char line1[200];
	char line2[200];

	gets(line1);
	

	while(gets(line1) != NULL){
		if ((strncmp(&line1[132], "c.nop", 5) == 0)){
			gets(line2);
			if ((strncmp(&line2[132], "c.nop", 5) == 0)){
				//printf("%s", line2);
				printf("\n********** STARTING FOR LOOP ********\n");
				break;
			}
		}
	}

	if (DEBUG){
		printf("\n%s\n", line1);
		printf("%s\n", line2);
	}
}

void read_two_lines(){
	char line1[200];
	char line2[200];
	gets(line1);
	gets(line2);
}


void read_gaps(){
	char line[200];
	char line2[200];
	int counter = 0;
	int delta = 0;
	int num1, num2;
	char val1[50];
	char val2[50];


	while (1){
		if (fgets(line, 200, stdin) == NULL)break;
		if (fgets(line2, 200, stdin) == NULL)break;
		if ((strncmp(&line[132], "c.j     pc + 160", 16) == 0) || (strncmp(&line2[132], "c.j     pc + 160", 16) == 0)) {
			
			counter++;
			if (counter == ITERAZIONI){
				printf("Termino con una iterazione in meno, iterazione %d non eseguita\n", counter);
				return;
			}
			printf("************* INIZIO ITERAZIONE %d ************\n", counter);
		}
		strncpy(val1, line + 8, 6);
		num1 = atoi(val1);
		strncpy(val2, line2 + 8, 6);
		num2 = atoi(val2);
		delta = num2 - num1;
		if (delta > NOTIFY) {
			printf("CC: %d\n", num1);
			printf("CC: %d\n", num2);
			printf("Delta: %d\n\n", delta);
		}

	}
}







