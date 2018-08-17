#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// create possible character array
char* possibleKeys = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
char* generatedKey;

int main(int argc, char* argv[]) {

// printf("Arg 1 is: %s\n", argv[1]);

// error message if too few or too many arguments
  if (argc < 2 || argc > 2) {
    fprintf(stderr, "Please provide the correct numbers of arguments\n");
    exit(1);
  }

// current time to seed for rand
srand(time(0));

int i, j;
// i = atoi(argv[1]);
for(i = 0; i < atoi(argv[1]); i++) {
  j = rand() % 27;
  printf("%c", possibleKeys[j]);
}
  printf("\n");

  return 0;
}