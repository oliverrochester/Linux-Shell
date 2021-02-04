#include <iostream>	
#include <stdlib.h>
#include <string>
#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fstream>
#include <fcntl.h>
#include <sys/wait.h>

using namespace std;


void execStuff(string pPath, string value, char* arg[]){
	string execStr = "";
	for(int i = 0; i < pPath.length(); i++){
		if(pPath[i] == ':'){
			execStr = execStr + '/' + value;
			execv(execStr.c_str(), arg);
			execStr = "";
		}
		else{
			execStr += pPath[i];
		}
	}
}

void doit(string value){
		
		
		bool badCMD = false;
		
		char *arg[6];
		int count = sscanf(value.c_str(), "%ms %ms %ms %ms %ms %ms", &arg[0], &arg[1], &arg[2], &arg[3], &arg[4], &arg[5]);
		string pPath = (string)getenv("PATH");
		
		string cmds[5] = { "ls", "pwd", "cd", "mkdir", "printenv"};
		for(int i = 0; i < 5; i++){
			if(arg[0] == cmds[i]){
				badCMD = false;
				break;
			}
			else{
				badCMD = true;
			}
		}
		
		if(badCMD){
			for(int i = 0; i < 4; i++){
				if(arg[0][0] == cmds[i][0]){
					cout << "did you mean " + cmds[i] + " ?" << endl;
					break;
				}
			}
		}
		
		if(value == "exit"){
			exit(0);
		}
		else if((string)arg[0] == "cd"){
			string cd = (string)arg[1];
			chdir(cd.c_str());
		}
		else if(arg[1] != nullptr && (string)arg[1] == "="){
			int set = setenv(arg[0], arg[2], 1);
			if(set != 0){
				cout << "did not set environment variable" << endl;
			}
		}
		else{
			int pid = fork();
			if(pid == 0){
				if(arg[1] != nullptr && (string)arg[1] == ">" ){
					int fd = open(arg[2],O_CREAT | O_WRONLY, 0660);//;O_CREAT | O_RDWR, 0660
					if(fd == -1){
						cout << "bad file open" << endl;
					}
					int x = dup2(fd,1);	
					if(x == -1){
						cout << "bad dup2" << endl;
					}
					value = (string)arg[0];
					arg[1] = nullptr;
					arg[2] = nullptr;
					execStuff(pPath, value, arg);
				}
				else if(arg[1] != nullptr && (string)arg[1] == "<"){
					int fd = open(arg[2],O_RDWR);
					if(fd == -1){
						cout << "bad file open" << endl;
					}
					int x = dup2(fd,0);
					if(x == -1){
						cout << "bad dup2" << endl;
					}
					value = (string)arg[0];
					arg[1] = nullptr;
					arg[2] = nullptr;
					execStuff(pPath, value, arg);
					
				}
				else if(arg[1] != nullptr && (string)arg[1] == ">>"){
					int fd = open(arg[2],O_APPEND | O_WRONLY);
					if(fd == -1){
						cout << "bad file open" << endl;
					}
					int x = dup2(fd,1);	
					if(x == -1){
						cout << "bad dup2" << endl;
					}
					value = (string)arg[0];
					arg[1] = nullptr;
					arg[2] = nullptr;
					execStuff(pPath, value, arg);
					
				}
				else{
					execStuff(pPath, value, arg);
				}
				exit(0);
			}
			
				
			if(pid != 0){
				int rets = 1;
				while(rets >= 1){
					rets = waitpid(-1, &pid, 0);
					if(rets < 1){
						break;
					}
				}
			}
		}				
	}

int main (int argc, char *argv[])
{
	string bang[20];
	int cnt = 0;
	int x = 1;
	bool full = false;
	while(x == 1){

		char* buf;
		while ((buf = readline(">> ")) != nullptr) {
			string value = (string)buf;
			if(value == ""){
				continue;
			}
			if(value[0] == '.'){
				string ans = "";
				for(int i = 2; i < value.length(); i++){
					ans += value[i];
				}
				cout << ans << endl;
				ifstream file(ans);
				string str; 
				while (getline(file, str)) {
					sleep(1);
					string st = str.substr(0, str.size()-1);
					doit(st);
				}
			}
			if (strlen(buf) > 0) {
			  add_history(buf);
				if(value[0] == '!' && isdigit(value.at(1)) && bang[value[1] - '0'] != ""){
					value = bang[value[1] - '0'];
				}
				else{
					//cout << "cannot find command" << endl;
				}
				if(value[0] == '!'){
					for(int i = 0; i < 20; i++){
						if(bang[i][0] == value[1]){
							value = bang[i];
							//cout << value << endl;
						}
					}
				}
				if(cnt == 19){
					cnt = 0;
					full = true;
					bang[cnt] = value;
				}
				else{
					bang[cnt] = value;
				}
				cnt++;
				doit(value);
			}
			//printf("[%s]\n", buf);
				
		}
	}
}

/*
completed

2 Tab Completion and Arrow History (see the readline library)
1 Control-L clears the screen
1 Bang last command that starts with some letter.
1 Knows how to change directory
1 Can run an executable.  I type in some full path, and you exec it.
3 Can do file output redirection ">"
3 Can do file input redirection "<"
3 Can do file output redirection append mode ">>"
1 Can set enviornment variables
3 When they misspell a command, offer a suggestion
1 Bang !# command
2 Can run commands from a file

*/