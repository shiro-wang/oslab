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

void clean_file(){
	memset(output_filename,50,sizeof(char));
	fclose(outfile);
}
void cd(char* exe){
	if(chdir(exe)==-1)
		perror("cd");
		
}
void pwd(){
	char buf[100];
	if(outfile){
		fprintf(outfile,"%s\n",getcwd(buf, sizeof(buf)));
		clean_file();
	}else{
		printf("%s\n",getcwd(buf, sizeof(buf)));
	}
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
	if(outfile){
		fprintf(outfile,"%s\n",after);
		clean_file();
	}else{
		printf("%s\n",after);
	}
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

void external(char* input){
	char** args=sh_split_the_line(input);
	int status;
	int bg=0;	
	
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

int main(int argc, char **argv) {
	pidmaps = malloc(LSH_TOK_BUFSIZE * sizeof(pid_t));
	char *input;
	char *prompt = "It's my shell, start! : ";
	//printf("%s",prompt);
	input=readline(prompt);
	
	while(strncmp(input, "exit", 4)){
		if(input[0]!=0){
			add_history(input);
			//detect > >>
			int redirect=0;
			int position=0;
			int mode=0;
			
			for(int i=0;i<strlen(input);i++){
				if(input[i] == '>'){
					position=i;
					redirect = 1;
					printf("repo:%d\n",position);
					break;
				}
			}
			if(redirect == 1){
				if(input[position+1]=='>'){
					if(input[position+2]!=' '){
						redirect = 0;
					}else{
						position++;
						mode = 1;
					}
				}else if(input[position+1]!=' '){
					redirect = 0;
				}
			}
			if(redirect == 1){
				char afterinput[50]={};
				int fn=0;
				for(int i=position+2;i<strlen(input);i++){
					output_filename[fn++] = input[i];
				}
				if(mode==0){
					outfile = fopen(output_filename,"w");
					strncpy(afterinput, input, position);
					
				}else{
					outfile = fopen(output_filename,"a+");
					strncpy(afterinput, input, position-1);
				}
				//printf("re: %s\n",afterinput);
				strcpy(input,afterinput);
			}
			
			//command
			int read_now;
			char command[10]={};
			int count;
			char *execute = malloc(sizeof(char));
			read_now = 0;
			memset(command, 0, 10);
			memset(execute, 0, 10);
			count=0;
			
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
			//printf("%s",prompt);
			//fgets(input, LENGTH, stdin);
		}
		input=readline(prompt);
	}
	system("PAUSE");
	return 0;
}

