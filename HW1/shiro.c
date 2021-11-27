#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char **argv) {
	char *ps;
	char *ss;
	char *prompt = "It's my shell, start! : ";
	printf("%s",&prompt);
	ss = readline(ps);
	while(ss!=NULL){
		printf("your input:%s\n",ss);
		printf("%s",&prompt);
		ss = readline(ps);
	}
	system("PAUSE");
	return 0;
}
