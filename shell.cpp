#include <bits/stdc++.h>	//Include all standard c++ headers.
#include <sys/stat.h>
#include <sys/wait.h>		//System specific headers for file process management spoken during class, like the wait() function for example.

using namespace std;

const int READ = 0;
const int WRITE = 1;

//Reading and Writing variables for pipes.

bool parser(string command, long long &maxim, vector<vector<string>> &commands){
    /*The parser function takes a string command representing the command to be parsed (analyzed); 
    ll &maxim, which is a variable that stores the maximum number of tokens found in any of the subcommands; 
    and a 2D vector that will hold the parsed commands and their arguments.*/

    vector<string> temp_commands;   //temporary vector to store the parts of the command as they are parsed.

    if(command == "end")            //If the inputed command is "end", the program will be terminated.
        exit(0);
    else if(command.size() == 0){   //If there's no input then you should love yourself, now.
        cout << "Mishell Bachelet $ ";
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
            const char *newDir = getenv("HOME :D");     //We try to change the directory to the user's home directory 
            if(newDir == nullptr){                  //I get PTSD whenever pointers are brought back. 
                cout << "Error. Por favor especifica un directorio o sufrirás la ira de las máquinas. >:)" << endl; /*This error happens if the 'HOME :D' 
                                                                                                                      environment variable is not set*/
            }else{
                if(chdir(newDir) != 0){
                    cout << "Error al moverse hacia el directorio " << newDir << endl;
                }
            }
        }else{
            const char *newDir = commands[0][1].c_str();    /*c_str() function obtains a pointer to a null-terminated character array that represents 
                                                              the content of a string object.*/
            if(chdir(newDir) != 0){
                cout << "Error al moverse hacia el directorio " << newDir << endl;
            }
        }else{

        }
    }
}
