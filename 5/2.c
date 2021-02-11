#include <fcntl.h>    // open
#include <stdio.h>    // fgetc, fopen, fclose
#include <stdlib.h>   // exit
#include <string.h>   // strlen
#include <sys/stat.h> // S_IRWXU
#include <sys/wait.h>
#include <unistd.h> // fork, write, close

const char * TEST_FILE = "./2.txt";
const int TEST_LINES = 1000;

void printfile(const char * file_path) {
    FILE *fp;
    fp = fopen(file_path, "r");
    int ch;
    printf("\nfile contents: \n");
    while ((ch = fgetc(fp)) != EOF)
      printf("%c", ch);
    fclose(fp);
}

void writeFile(int fd, int lines, const char * type) {
  for(int i=0; i < lines; i++) {
    char line[128];
    int size = sprintf(line, "%s writes a line %d.\n", type, i);
    write(fd, line, size);
  }
}


int main() {
  int fd = open(TEST_FILE, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
  int rc = fork();
  write(fd, "First line.\n", strlen("First line.\n"));

  if (rc < 0) {
    // fork failed; exit
    fprintf(stderr, "fork failed\n");
    exit(EXIT_FAILURE);
  } else if (rc == 0) {
    printf("file descriptor in child process: %d\n", fd);
    writeFile(fd, TEST_LINES, "child");
  } else {
    printf("file descriptor in parent prosess: %d\n", fd);
    writeFile(fd, TEST_LINES, "parent");

    if (wait(NULL) == -1) {
      fprintf(stderr, "wait failed\n");
      exit(EXIT_FAILURE);
    }

    printfile(TEST_FILE);
  }

  close(fd);
  return 0;
}
