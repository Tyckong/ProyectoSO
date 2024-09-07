#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>		//System specific headers for file process management spoken during class, like the wait() function for example.

using namespace std;

// Colores ANSI para la terminal
const string RESET_COLOR = "\033[0m";
const string PROMPT_COLOR = "\033[1;32m";   // Verde
const string ERROR_COLOR = "\033[1;31m";    // Rojo
const string INFO_COLOR = "\033[1;34m";     // Azul

const int READ = 0;
const int WRITE = 1;

void print_prompt() {
    cout << PROMPT_COLOR << "Myshell Bachelet " << RESET_COLOR << "$ ";
}

//Reading and Writing variables for pipes.

bool parser(string command, long long &maxim, vector<vector<string>> &commands){
    /*The parser function takes a string command representing the command to be parsed (analyzed); 
    ll &maxim, which is a variable that stores the maximum number of tokens found in any of the subcommands; 
    and a 2D vector that will hold the parsed commands and their arguments.*/

    vector<string> temp_commands;   //temporary vector to store the parts of the command as they are parsed.

    if(command == "end")            //If the inputed command is "end", the program will be terminated.
        exit(0);
    else if(command.size() == 0){   //If there's no input then you should love yourself, now.
        return 1;
    }
    else if(command == "wah"){
        cout << INFO_COLOR << "Wah!" << RESET_COLOR << endl;
        return 1;
    }

    //Now, for VALID commands.
    string subcommand;
    stringstream S1(command);
    while(getline(S1, subcommand, '|')){
        //Command string is spit into subcommands using the pipe character as a delimiter.
        string argument;
        stringstream S2(subcommand);
        while(getline(S2, argument, ' ')){
            //Subcommands are further split into arguments using spaces as delimiters.
            string trash_chars = "\"";        //Characters to be removed from the parsed command arguments.
            argument.erase(remove_if(argument.begin(), argument.end(), [&trash_chars](const char &c){
                return trash_chars.find(c) != string::npos;
            }),
            argument.end());
            //As said earlier, quitation marks are removed from the arguments.
            temp_commands.push_back(argument);
        }
        temp_commands.erase(remove(temp_commands.begin(), temp_commands.end(), ""), temp_commands.end());
        //We don't welcome empty strings in this household.

        long long history_size = temp_commands.size();
        maxim = max(history_size, maxim);
        commands.push_back(temp_commands);
        temp_commands.clear();
        /*Here we compare the size of the commands vector to the current 'maxim' value. If it's larger, maxim is updated.
          Then, the fully parsed subcommand is added to the commands vector and we clear the temp_commands for the next iteration.*/
    } 
    return 0;
}

void pipeless_command(vector<vector<string>> commands){
    //The commands 2D vector will contain the parsed command and its arguments. The first element represents the command and any subsequent elements the arguments.
    //This is going to be very fun.
    if(commands[0][0] == "cd"){         //We check for the 'cd' command used to change directory.
        if(commands[0].size() < 2){     //"If no directory is specified"...
            const char *newDir = getenv("HOME");     //We try to change the directory to the user's home directory 
            if(newDir == nullptr){                  //I get PTSD whenever pointers are brought back. 
                cout << ERROR_COLOR << "Error. Por favor especifica un directorio." << RESET_COLOR << endl; /*This error happens if the 'HOME' 
                                                                                                                      environment variable is not set*/
            }else{
                if(chdir(newDir) != 0){
                    cout << ERROR_COLOR << "Error al moverse hacia el directorio " << newDir << RESET_COLOR << endl;
                }
            }
        }else{
            const char *newDir = commands[0][1].c_str();    /*c_str() function obtains a pointer to a null-terminated character array that represents 
                                                              the content of a string object.*/
            if(chdir(newDir) != 0){
                cout << ERROR_COLOR << "Error al moverse hacia el directorio " << newDir << RESET_COLOR << endl;
            }
        }
    }else{
        char *arguments[commands[0].size() + 1];            //We create a char* array to store the command and its arguments in a format suitable for the 'execvp' system call.
        for(size_t i=0; i<commands[0].size(); ++i){
            arguments[i] = strdup(commands[0][i].c_str());
        }
        arguments[commands[0].size()] = NULL;               //We set the last element of the array to NULL to indicate the end of the list of arguments.
        int pid = fork();                                   //Create a child process using fork(). The pid variable stores the process ID of the child.
        if(pid == 0){    
            int execute_return = execvp(arguments[0], arguments); //EXECUTE THE CHILD!!!
                                                                  //The execvp() executes the command, replacing the child's process' memory space with the new program in 'arguments'.
            if(execute_return < 0){
                cout << ERROR_COLOR << "Error en el comando ingresado." << RESET_COLOR << endl;
                exit(EXIT_FAILURE);
            }
        }else if(pid < 0){                                  //Fork failed.
            cout << ERROR_COLOR << "Algo salió mal durante la creación del proceso hijo." << RESET_COLOR << endl;
        }else{
            wait(NULL);     //Normally kids aren't as fast as their parents, so we must give them time to catch up. I hope I can be a good dad someday! :D
        }
    }
}

int main(){
    vector<vector<string>> commands; //2D vector where each subvector represents a command and its arguments.
    string command;                  //String to store commands inputed by the user.
    long long maxim;                //maximum amount of arguments in a command.  

    //Loop to read and parse the user's input:
    while(true){
        print_prompt();
        if (!getline(cin, command)) break;

        maxim = 0;
        int ret = parser(command, maxim, commands);     //parses the command into 'commands' and updates the maxim with the maximum number of arguments in any subcommand.
        if(ret == 1){       //This means the command is either empty or an 'end' order.
            command.clear();
            continue;
        }
        if(commands.size() <= 1){           //Only one command? No problem! For only $3.99 we present you: pipeless_command()!
            pipeless_command(commands);
        }else{                              //Thanks Daniela and Jorge for explaining to me how pipes work. 
            char *arguments[commands.size()][maxim + 1];
            for(size_t i=0; i < commands.size(); ++i){
                for(size_t j=0; j<commands[i].size(); j++){
                    arguments[i][j] = strdup(commands[i][j].c_str());
                }
                arguments[i][commands[i].size()] = NULL;
            }
            long long all_pipes = commands.size() - 1;
            int Pipes[all_pipes][2];
            for(int k=0; k < all_pipes; ++k){
                pipe(Pipes[k]);             //Do you think pipe is short for Felipe? Phillip maybe? Intriguing.
            }
            int counter = 0;
            if(fork() == 0){
                dup2(Pipes[0][WRITE], WRITE);
                close(Pipes[0][READ]);
                int path = 1;
                while(path < all_pipes){
                    close(Pipes[path][READ]);
                    close(Pipes[path][WRITE]);
                    path++;
                }
                int execute_command = execvp(arguments[0][0], arguments[0]);
                if(execute_command < 0){
                    cout << ERROR_COLOR << "El comando ingresado no es válido." << RESET_COLOR << endl;
                    exit(0);
                }
            }
            if(all_pipes > 1){      //Handling multiple commands.
                while(counter < all_pipes - 1){
                    if(fork() == 0){ 
                        dup2(Pipes[counter][READ], READ);
                        dup2(Pipes[counter + 1][WRITE], WRITE);
                        for(int i=0; i < all_pipes; ++i){
                            if(i==counter)
                                close(Pipes[i][WRITE]);
                            else if(i==counter+1)
                                close(Pipes[i][READ]);
                            else{
                                close(Pipes[i][READ]);
                                close(Pipes[i][WRITE]);
                            }       //Here each child process redirects its standard input to the read/write end of the previous/next pipe and closes all other pipe ends.
                        }
                        int execute_command2 = execvp(arguments[counter+1][0], arguments[counter+1]);
                        if(execute_command2 < 0){
                            cout << ERROR_COLOR << "El comando ingresado no es válido." << RESET_COLOR << endl;
                            exit(0);
                        }
                    }
                    counter++;
                }
            }
            if(fork()==0){
                dup2(Pipes[counter][READ], READ);   //Read from the last pipe.
                close(Pipes[counter][WRITE]);       //Close the write end of the previous pipe.
                int path = 0;

                while(path < all_pipes){            //CLOSE ALL THE PIPES!!!
                    if(path == counter){
                        path++;
                        continue;
                    }
                    close(Pipes[path][READ]);
                    close(Pipes[path][WRITE]);
                    path++;
                }
                int p = execvp(arguments[counter+1][0], arguments[counter+1]);
                if(p < 0){
                    cout << ERROR_COLOR << "El comando ingresado no es valido." << RESET_COLOR << endl;
                    exit(0);
                }
            }
            for(int i=0; i<all_pipes; ++i){
                close(Pipes[i][WRITE]);
                close(Pipes[i][READ]);
            }
            for(size_t l=0; l < commands.size()-1; ++l)
                wait(NULL);
        }
        commands.clear();
        command.clear();
    }   //Clear commands and command for the next iteration.
    return 0;
}