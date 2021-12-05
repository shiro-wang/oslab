#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/types.h> 
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#define LENGTH 50
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
int command_count;
int pid_count=0;
int *pidmaps;
char output_filename[50] = {};
FILE *outfile;
void cd(char* exe){
	if(chdir(exe)==-1)
		perror("cd");
		
}
void pwd(){
	char buf[100];
	printf("current working directory: %s\n",getcwd(buf, sizeof(buf)));
}
void export_c(char* exe){
	char origin[10]  = {0};
	char after[5000] = {0};
	int read_now = 0;
	int count=0;
	//before=
	for(read_now=0;read_now<strlen(exe);read_now++){
		if(exe[read_now] != '='){
			origin[count++] = exe[read_now];
		}else{	
			break;
		}
	}
	//printf("origin:%s content:%s\n", origin, getenv(origin));
	read_now++;
	
	char tmp[500];
	char envname[10];
	//after=
	while(read_now<strlen(exe)){
		count=0;
		memset(tmp, 0, 500);
		memset(envname, 0, 10);
		for(;read_now<strlen(exe);read_now++){
			if(exe[read_now] != '$'){
				tmp[count++] = exe[read_now];
			}else{
				read_now++;
				break;
			}
		}
		strcat(after,tmp);
		//printf("tmp:%s\n",tmp);
		//有用到enc variable 
		count=0;
		for(;read_now<strlen(exe);read_now++){
			if((exe[read_now] >='a'&&exe[read_now] <='z') || (exe[read_now] >='A'&&exe[read_now] <='Z')){
				envname[count++] = exe[read_now];
			}else{
				char* env;
				env = getenv(envname);
				strcat(after,env);
				//printf("env:%s\n",env);
				break;
			}
		}
		//printf("envname:%s\n",envname);
		//printf("%s\n",env);
	}
	//printf("%s\n",after);
	//setenv(origin, after, 1);
	//printf("origin:%s after_content:%s\n", origin, getenv(origin));
}
void echo(char* exe){
	char after[500] = {0};
	int read_now = 0;
	int count=0;
	int tf=1;
	char tmp[50];
	char envname[10];
	//start
	while(tf==1){
		//printf("read_now:%d strlen(exe):%d\n", read_now, strlen(exe));
		count=0;
		memset(tmp, 0, 50);
		memset(envname, 0, 10);
		for(;read_now<strlen(exe);read_now++){
			if(exe[read_now] != '$'){
				tmp[count++] = exe[read_now];
			}else{
				read_now++;
				break;
			}
		}
		strcat(after,tmp);
		//printf("read_now:%d\n",read_now);
		//printf("tmp:%s\n",tmp);
		//有用到enc variable 
		
		count=0;
		for(;read_now<strlen(exe);read_now++){
			if((exe[read_now] >='a'&&exe[read_now] <='z') || (exe[read_now] >='A'&&exe[read_now] <='Z')){
				envname[count++] = exe[read_now];
			}else{
				char* env;
				env = getenv(envname);
				strcat(after,env);
				break;
			}
		}
		//printf("envname:%s\n",envname);
		//printf("env:%s\n",env);
		//printf("after:%s\n",after);
		//printf("read_now:%d strlen(exe):%d\n", read_now, strlen(exe));
		if(read_now==strlen(exe)){
			//printf("end");
			tf=0;
		}
	}
	//printf("finish read_now:%d strlen(exe):%ld\n", read_now, strlen(exe));
	printf("%s\n",after);
}
char **sh_split_the_line(char *line)
{
  command_count = 0;
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
    command_count++;
  }
  tokens[position] = NULL;
  return tokens;
}
void failhaha(){
	printf("Invalid situation\n");
}
void output(char** args, int position){
	memset(output_filename, 50, sizeof(char));
	int mode=0;
	if(!strncmp(args[position],">>",2){
		mode=1;
	}
	
	strcpy(output_filename,args[position+1]);
	if(mode==0){
		outfile = fopen(output_filename,"w");
	}else{
		outfile = fopen(output_filename,"a+");
	}
	args[position]=NULL;
	do_command(args);
}
void external(char* input){
	char** args=sh_split_the_line(input);
	int status;
	int bg=0;
	for(int i=0;i<command_count;i++){
		if(!strncmp(args[i], ">", 1) || !strncmp(args[i], ">>", 2)){
			output(args, i);
		}
	}
	if(!strcmp("&",args[command_count-1])){
		bg=1;
		args[command_count-1]=NULL;
	}
	pid_t wpid, pid=fork();
	if(pid == 0){
		if(execvp(args[0], args)==-1)
			perror("external");
		exit(EXIT_FAILURE);
	}else if(pid > 0){
		if(bg == 1){
//			printf("[proc %d started]\n", pid);
//			pidmaps[pid_count]=pid;
//			pid_count++;
		}else{
			do {
		      wpid = waitpid(pid, &status, WUNTRACED);
		    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
	}else{
		failhaha();
	}
	
}
void do_command(char* input){
	int read_now;
	char command[10]={};
	int count;
	char *execute = malloc(sizeof(char));
	read_now = 0;
	memset(command, 0, 10);
	memset(execute, 0, 10);
	count=0;
	//command
	for(read_now=0;read_now<strlen(input)-1;read_now++){
		if(input[read_now] != ' '){
			command[count++] = input[read_now];
		}else{	
			break;
		}
	}
	read_now++;
	count=0;
	//others
	for(;read_now<strlen(input)-1;read_now++){
		execute[count++] = input[read_now];
	}
	/////////////////////
	if(!strncmp(command, "cd",2)){
		cd(execute);
	}else if(!strncmp(command, "pwd",3)){
		pwd();
	}else if(!strncmp(command, "export",6)){
		export_c(execute);
	}else if(!strncmp(command, "echo",4)){
		echo(execute);
	}else {
		external(input);
	} 
	//fflush(stdin);
	//memset(input, 0, LENGTH);
	free(execute);
}

int main(int argc, char **argv) {
	pidmaps = malloc(LSH_TOK_BUFSIZE * sizeof(pid_t));
	char history[500]={};
	//char input[LENGTH]={};
	
	char *input;
	//char *ps;
	char *prompt = "It's my shell, start! : ";
	//printf("%s",prompt);
	input=readline(prompt);
	
	while(strncmp(input, "exit", 4)){
		if(input[0]!=0){
			//printf("your input:%s",input);
			//strcat(history, input);
			add_history(input);
			//printf("%s",history);
			do_command(input);
			//printf("%s",prompt);
			//fgets(input, LENGTH, stdin);
		}
		input=readline(prompt);
	}
	system("PAUSE");
	return 0;
}
