#include <stdio.h>

// Declare a bufffer for useer input of size 2048
static char input[2048];

int main(int argc, char** argv){
    // Print version and exit information
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+c to exit\n"); 

    while (1) {
        // output prompt
        fputs("lispy> ", stdout); 

        // read line of user input. Maximum is zee 2048
        fgets(input, 2048, stdin);

        // Echo input back to user
        printf("No you're a %s", input);
    }

    return 0; 
}