#include<stdio.h>
#include<string.h>

int main(){

FILE *f = fopen("backingstore.txt","r+");

for (int i = 0 ; i < 5; i++) {
	
	int value;

	printf ("Enter a value from 0 to 25");

	scanf("%d", &value);

	fseek(f,(value*10),0);

	char read[10];

	fread(&read, sizeof(char),10,f); 

	printf("%s", read);

	fseek(f,(value*10),0);

	read[4] = '*';

	fwrite(&read, sizeof(char), 10, f);
}

fclose(f);


return 1;
}
