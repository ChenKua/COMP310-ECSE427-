#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// typedef is basically just renaming this type of struct to Student. It does not change the logic behind.
// It just becomes easier to declare while coding.
typedef struct
{
	char name[30];
	int age;
	double gpa;
} Student;

typedef struct Stack {
	Student student;
	struct Stack* node_below;
} Stack;

// Global variable accessible from anywhere.
Stack* stack = NULL;

void printStudent(Student* student) {
	printf("Name: %s\n", student->name);
	printf("Age : %d\n", student->age);
	// Using '%.2lf' will only print two decimals after the '.' .
	printf("GPA : %.2lf\n", student->gpa);
	printf("\n");
}

int promptUser(Student* student)
{
	printf("Please fill out the following information:\n");
	printf("Name:\n");
	printf("> ");
	// The logic here is that you dereference the 'student' pointer and get the value at that memory space using the *.
	// You then access the field of the struck using the '.' operator
	// And finally scanf needs a pointer to save the input from the user so you use the '&' to give it the address of 
	// the space which what pointer variables hold.

	// The scanf function is not safe and does not assure that you only take the first 29 characters of the input. 
	// Tried it out, thought it might work but nope :(. Basically this works but does not check if the input is valid.
	// I would suggest using fgets like the prof's solutions. It will also get rid of the warning.
  	scanf("%29s", &((*student).name));

	printf("Age:\n");
	printf("> ");  
    scanf("%d", &((*student).age));
	
	printf("GPA:\n");
	printf("> ");
	// The 'student->' operator is equivalent to writing '(*student).'
	scanf("%lf", &(student->gpa));
}

void push(Student* student)
{
	Stack* newNode = (Stack*) malloc(sizeof(Stack));
	// memcpy will copy sizeof(Student) characters from student to the newNode's field.
	memcpy(&((*newNode).student), student, sizeof(Student));
	(*newNode).node_below = stack;
	stack = newNode;
}

void pop(Student* student)
{
	Stack* currentNode = stack;
	stack = (*stack).node_below;
	// It is still pertinent to copy the value of the node into the input parameter since we will use it in the print fct.
	memcpy(student, &(currentNode->student), sizeof(Student));
	free(currentNode);
}

int main()
{	
	Student student;

	printf("\n\tWelcome to my data center.\n\n");

	while (1) {
		promptUser(&student);
		push(&student);
		// consume the remaining character from previous input
		while (getchar() != '\n');
		printf("continue? [y/n]");
		char choice = getchar();
		if (!(choice == 'y' || choice == 'Y')) break;
		// consume the remaining character from previous input
		while (getchar() != '\n');
	}

	printf("\n\n***\tHere is the information I have recorded through my stack.\t***\n\n");
	while (stack != NULL) 
	{
		pop(&student);
		printStudent(&student);
	}
	// Don't forget to clear all the used memory allocated in the heap!
	free(stack);

	return 0;
}
