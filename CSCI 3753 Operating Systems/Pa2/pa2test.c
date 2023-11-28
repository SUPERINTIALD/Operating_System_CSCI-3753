#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

void cntrlDfunc() {
    if (feof(stdin)){
        printf("\n");
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char* argv[]) {
    if (argc <= 1){
        printf("error: missing filename\n");
        exit(EXIT_FAILURE);
    }
    else if (argc == 2){
        int userRInput, userOffsetInput, userWhenceInput;
        char *filename = argv[1];
        char *buffer;
        size_t size = 2048;
        char displayOptionInput;
        char null = '\0';
        FILE* file = fopen(filename, "r+");
        if (!file) {
            printf("Error: File couldn't be opened\n");
            exit(EXIT_FAILURE);
        }
        while(1){
            printf("Option (r for read, w for write, s for seek): ");
            scanf("%c", &displayOptionInput);
            cntrlDfunc();
            getchar();

            switch(displayOptionInput){
                case 'r': 
                    printf("Enter the number of bytes you want to read: ");
                    scanf("%d", &userRInput);
                    cntrlDfunc();
                    getchar();
                    buffer = (char*)malloc(userRInput);
                    fread(buffer, 1, userRInput, file);
                    printf("%s\n", buffer);
                    free(buffer);
                    break;
                case 'w':
                    buffer = malloc(2048);
                    printf("Enter the data you want to write: ");
                    fgets(buffer, 2048, stdin);
                    cntrlDfunc();
                    buffer[strcspn(buffer, "\n")] = 0;
                    fprintf(file, "%s", buffer) ;  
                    fprintf(file, "%c", null);                 
                    fflush(file);
                    free(buffer);
                    break;
                case 's':
                    printf("Enter an offset value: ");
                    scanf("%d", &userOffsetInput);
                    cntrlDfunc();
                    getchar();

                    printf("Enter a value for whence: ");
                    scanf("%d", &userWhenceInput);
                    cntrlDfunc();
                    getchar();

                    fseek(file, userOffsetInput, userWhenceInput);
                    break;

                default:
                    printf("Error: Not a valid option\n");
                    break;
            }
        }
    }
    exit(EXIT_SUCCESS);
}
