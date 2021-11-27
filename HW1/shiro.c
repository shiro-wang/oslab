#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define LENGTH 50
void cd(char* exe){
	chdir(exe);
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
		for(;;read_now++){
			if(exe[read_now] != '$'){
				tmp[count++] = exe[read_now];
			}else{
				read_now++;
				break;
			}
		}
		printf("tmp:%s\n",tmp);
		//有用到enc variable 
		count=0;
		for(;;read_now++){
			if((exe[read_now] >='a'&&exe[read_now] <='z') || (exe[read_now] >='A'&&exe[read_now] <='Z')){
				envname[count++] = exe[read_now];
			}else{
				break;
			}
		}
		printf("envname:%s\n",envname);
		char* env;
		env = getenv(envname);
		printf("%s\n",env);
		strcat(after,tmp);
		strcat(after,env);
	}
	printf("%s\n",after);
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
		for(;;read_now++){
			if(exe[read_now] != '$'){
				tmp[count++] = exe[read_now];
			}else{
				read_now++;
				break;
			}
		}
		//printf("tmp:%s\n",tmp);
		//有用到enc variable 
		count=0;
		for(;read_now<strlen(exe);read_now++){
			if((exe[read_now] >='a'&&exe[read_now] <='z') || (exe[read_now] >='A'&&exe[read_now] <='Z')){
				envname[count++] = exe[read_now];
			}else{
				break;
			}
		}
		//printf("envname:%s\n",envname);
		
		char* env;
		env = getenv(envname);
		//printf("env:%s\n",env);
		strcat(after,tmp);
		strcat(after,env);
		printf("after:%s\n",after);
		//printf("read_now:%d strlen(exe):%d\n", read_now, strlen(exe));
		if(read_now==strlen(exe)){
			tf=0;
		}
	}
	printf("finish read_now:%d strlen(exe):%ld\n", read_now, strlen(exe));
	printf("%s\n",after);
}

void fail(){
	printf("Invalid situation\n");
}
int main(int argc, char **argv) {
	char history[500]={};
	char input[LENGTH]={};
	char command[10]={};
	int count;
	char *prompt = "It's my shell, start! : ";
	printf("%s",prompt);
	fgets(input, LENGTH, stdin);
	int read_now;
	while(strcmp(input, "exit\n")){
		printf("your input:%s",input);
		strcat(history, input);
		//printf("%s",history);
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
		}
		
		memset(input, 0, LENGTH);
		free(execute);
		printf("%s",prompt);
		fgets(input, LENGTH, stdin);
	}
	system("PAUSE");
	return 0;
}

