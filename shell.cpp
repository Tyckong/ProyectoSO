#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <csignal>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>		//System specific headers for file process management spoken during class, like the wait() function for example.
#include <thread>
#include <chrono>
#include <semaphore.h>
#include <fstream>

using namespace std;

// Colores ANSI para la terminal
const string RESET_COLOR = "\033[0m";
const string PROMPT_COLOR = "\033[1;32m";   // Verde
const string ERROR_COLOR = "\033[1;31m";    // Rojo
const string INFO_COLOR = "\033[1;34m";     // Azul

const int READ = 0;
const int WRITE = 1;

string reminder_message;
sem_t print_semaphore;

// Estructura para comandos favoritos
vector<string> favorite_commands;
string favs_file_path = ""; // Ruta por defecto del archivo de favoritos

void print_prompt() {
    const size_t size = 1024; 
    char directory[size];
    getcwd(directory, size);
    cout << PROMPT_COLOR << "Myshell Bachelet:" << INFO_COLOR << '~' << directory << RESET_COLOR << "$ " << flush;
}

void reminder_handler(int signum) {
    sem_wait(&print_semaphore); // Esperar para acceder a la sección crítica
    cout << "\n" << INFO_COLOR << "[!] " << reminder_message << RESET_COLOR << endl;
    print_prompt();  // Imprimir el prompt después del recordatorio
    sem_post(&print_semaphore); // Liberar la sección crítica
}

void set_recordatorio(int seconds, const string &message) {
    reminder_message = message;  // Guardar el mensaje del recordatorio
    signal(SIGALRM, reminder_handler);  // Asociar el manejador a la señal SIGALRM
    alarm(seconds);  // Programar la alarma para que se dispare en 'seconds' segundos
}

// Funciones para manejar comandos favoritos

void add_to_favs(const string &command, int cmdCode) {
    if (cmdCode != 0)
        return;
    auto it = find_if(favorite_commands.begin(), favorite_commands.end(), [&command](const string &cmd) { 
        return cmd == command; 
    });
    if (it == favorite_commands.end()) {
        favorite_commands.emplace_back(command);
    }
}

void load_favs() {
    ifstream favs_file(favs_file_path);
    if (favs_file.is_open()) {
        string line;
        while (getline(favs_file, line)) {
            add_to_favs(line, 0); // cmdCode == 0 porque si se esta leyendo del archivo, ya se ejecuto bien
        }
        favs_file.close();
    }
}

void save_favs() {
    ofstream favs_file(favs_file_path);
    if (favs_file.is_open()) {
        for (const auto &command : favorite_commands) {
            favs_file << command << endl;
        }
        favs_file.close();
    }
}

void create_favs_file(const string &path) {
    favs_file_path = path;
    ifstream favs_file(favs_file_path);
    if (!favs_file) {
        favs_file.close();
        ofstream favs_file(path);
        if(!favs_file)
            cout << ERROR_COLOR << "Error creando archivo en " << path << RESET_COLOR << endl;
        else
            cout << INFO_COLOR << "Archivo de favoritos creado en " << path << RESET_COLOR << endl;
    } else {
        cout << INFO_COLOR << "Archivo de favoritos actualizado " << path << RESET_COLOR << endl;
        load_favs();
    }
    ofstream exit_path("actual_path.txt");
    if (exit_path.is_open()) {
        exit_path << favs_file_path;
        exit_path.close();
    }
    favs_file.close();
}

void show_favs_m() {
    int i = 1;
    for (const auto &command : favorite_commands) {
        cout << INFO_COLOR << i << ": " << command << RESET_COLOR << endl;
        i++;
    }
    if(i == 1){
        cout << ERROR_COLOR << "Error. No hay comandos para mostrar." << RESET_COLOR << "\n";
    }
}
void show_favs_c(const string &path) {
    ifstream favs_file(path);
    if(favs_file.is_open()) {
        string command;
        int i = 1;
        while(favs_file) {
            getline(favs_file, command);
            if(command == "")
                continue;
            cout << INFO_COLOR << i << ": " << command << RESET_COLOR << endl;
            i++;
        }
    }
    else {
        cout << ERROR_COLOR << "Error. No se pudo abrir el archivo." << RESET_COLOR << "\n";
    }
}

void delete_favs(const vector<int> &nums) {
    for(auto i: nums) {
        if(i > 0 and i <= favorite_commands.size()){
            favorite_commands.erase(favorite_commands.begin()+i-1);
            cout << INFO_COLOR << "Comando " << i << " eliminado correctamente." << RESET_COLOR << "\n";
        }
        else
            cout << ERROR_COLOR << "Error. Comando " << i << " no encontrado." << RESET_COLOR << "\n";
    }
}

void search_favs(const string &cmd) {
    int i = 1;
    for (const auto &command : favorite_commands) {
        if (command.find(cmd) != string::npos) {
            cout << INFO_COLOR << i << ": " << command << RESET_COLOR << endl;
        }
        i++;
    }
}

void delete_all_favs() {
    favorite_commands.clear();
}

void execute_fav(int num) {
    if(num > 0 and num <= favorite_commands.size()) {
        system((favorite_commands[num-1]).c_str());
    }
    else {
        cout << ERROR_COLOR << "Error. Comando " << num << " no encontrado." << RESET_COLOR << "\n";
    }
}

void handle_favs_command(const vector<string> &args) {
    if(args.size() == 1) {
        cout << ERROR_COLOR << "Error. Ingresa un subcomando junto a favs\n" << RESET_COLOR;
        return;
    }
    if (args[1] == "crear") {
        create_favs_file(args[2]);
        return;
    }
    else if (args[1] == "cargar") {
        load_favs();
        cout << INFO_COLOR << "Comandos extraidos del archivo exitosamente.\n" << RESET_COLOR;
        show_favs_c(favs_file_path);
        return;
    } else if (args[1] == "mostrar") {
        show_favs_m();
    } else if (args[1] == "eliminar") {
        vector<int> nums;
        if(args.size() == 2) {
            cout << ERROR_COLOR << "Formato incorrecto. Usa: favs eliminar <indice1>, <indice2>, ...\n" << RESET_COLOR;
            return;
        }
        stringstream ss(args[2]);
        string num;
        while (getline(ss, num, ',')) {
            nums.push_back(stoi(num));
        }
        sort(nums.begin(), nums.end(), greater<int>() );
        delete_favs(nums);
    } else if (args[1] == "buscar") {
        search_favs(args[2]);
    } else if (args[1] == "borrar") {
        delete_all_favs();
        cout << INFO_COLOR << "Lista de comandos borrados exitosamente.\n" << RESET_COLOR;
    }else if (args[1] == "guardar") {
        save_favs();
        cout << INFO_COLOR << "Comandos sobreescritos en archivo exitosamente.\n" << RESET_COLOR;
    } else if (args[1] == "ejecutar") {
        int num = stoi(args[2]);
        execute_fav(num);
    } else {
        cout << ERROR_COLOR << "Error en el comando ingresado." << RESET_COLOR << endl;
    }
}

//Reading and Writing variables for pipes.

bool parser(string command, long long &maxim, vector<vector<string>> &commands){
    /*The parser function takes a string command representing the command to be parsed (analyzed); 
    ll &maxim, which is a variable that stores the maximum number of tokens found in any of the subcommands; 
    and a 2D vector that will hold the parsed commands and their arguments.*/

    vector<string> temp_commands;   //temporary vector to store the parts of the command as they are parsed.

    if(command == "end"){            //If the inputed command is "end", the program will be terminated.
        save_favs();
        exit(0);
    }else if(command.size() == 0){   //If there's no input then you should love yourself, now.
        return 1;
    }
    else if(command == "wah"){
        cout << INFO_COLOR << "Wah!" << RESET_COLOR << endl;
        return 1;
    }else if(command[command.size()-1] == '|'){         // If the inputed command end in '|' (a pipe that didn't work), exit and not read.
        cout << ERROR_COLOR << "Error en el comando \"pipe\" ingresado." << RESET_COLOR << '\n';
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
        int cmdCode = 0;
        if(commands[0].size() < 2){     //"If no directory is specified"...
            const char *newDir = getenv("HOME");     //We try to change the directory to the user's home directory 
            if(newDir == nullptr){                  //I get PTSD whenever pointers are brought back. 
                cout << ERROR_COLOR << "Error. Por favor especifica un directorio." << RESET_COLOR << endl; /*This error happens if the 'HOME' 
                                                                                                                      environment variable is not set*/
                cmdCode = 1;
            }else{
                if(chdir(newDir) != 0){
                    cout << ERROR_COLOR << "Error al moverse hacia el directorio " << newDir << RESET_COLOR << endl;
                    cmdCode = 1;
                }
            }
        }else{
            const char *newDir = commands[0][1].c_str();    /*c_str() function obtains a pointer to a null-terminated character array that represents 
                                                              the content of a string object.*/
            if(chdir(newDir) != 0){
                cout << ERROR_COLOR << "Error al moverse hacia el directorio " << newDir << RESET_COLOR << endl;
                cmdCode = 1;
            }
        }
        string command;
        for(auto s: commands[0])
            command += s + " ";
        add_to_favs(command, WEXITSTATUS(cmdCode));
    }else if (commands[0][0] == "set" && commands[0][1] == "recordatorio") {
        int cmdCode = 0;
        if (commands[0].size() >= 4) {
            int seconds = stoi(commands[0][2]);
            string message;
            for(int i = 3; i < commands[0].size(); ++i) {
                message += commands[0][i] + " ";
            }
            thread t(set_recordatorio, seconds, message);
            t.detach();
        } else {
            cout << ERROR_COLOR << "Formato incorrecto. Usa: set recordatorio <tiempo> <mensaje>" << RESET_COLOR << endl;
            cmdCode = 1;
        }
        string command;
        for(auto s: commands[0])
            command += s + " ";
        add_to_favs(command, WEXITSTATUS(cmdCode));
    }else if (commands[0][0] == "favs") {
        handle_favs_command(commands[0]);
    }else{
        int cmdCode = 0;
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
            wait(&cmdCode);     //Normally kids aren't as fast as their parents, so we must give them time to catch up. I hope I can be a good dad someday! :D
        }
        string command;
        for(auto s: commands[0])
            command += s + " ";
        add_to_favs(command, WEXITSTATUS(cmdCode));  // Agregar el comando a favoritos si no es de gestión de favoritos
    }
}

int main(){
    vector<vector<string>> commands; //2D vector where each subvector represents a command and its arguments.
    string command;                  //String to store commands inputed by the user.
    long long maxim;                //maximum amount of arguments in a command.
    int prevErrCode = -1;
    ifstream actual_path("actual_path.txt");
    if (actual_path.is_open()) {
        string line;
        getline(actual_path, line);
        favs_file_path = line;
        actual_path.close();
    }

    sem_init(&print_semaphore, 0, 1);

    //Loop to read and parse the user's input:
    while(true){
        bool actErrCode = true;
        maxim = -1;
        print_prompt();
        if (!getline(cin, command)) break;

        maxim = 0;
        int ret = parser(command, maxim, commands);     //Parses the command into 'commands' and updates the maxim with the maximum number of arguments in any subcommand.
        if(ret == 1){       //This means the command is either empty, an 'end' order or end with '|'.
            command.clear();
            continue;
        }
        if(commands.size() <= 1){           //Only one command? No problem! For only $3.99 we present you: pipeless_command()!
            pipeless_command(commands);
        }else{                              //Thanks Daniela and Jorge for explaining to me how pipes work. 
            char *arguments[commands.size()][maxim + 1];
            string pipe_command = "";
            for(size_t i=0; i < commands.size(); ++i){
                for(size_t j=0; j<commands[i].size(); j++){
                    arguments[i][j] = strdup(commands[i][j].c_str());
                    if(j!=0) pipe_command = pipe_command + " " + arguments[i][j];
                    else pipe_command = pipe_command + arguments[i][j];
                }
                arguments[i][commands[i].size()] = NULL;
                if(i < commands.size()-1) pipe_command = pipe_command + " | ";
            }
            // 
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
                    actErrCode = false;
                    exit(1);
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
                            actErrCode = false;
                            exit(1);
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
                    actErrCode = false;
                    exit(1);
                }
            }

            for(int i=0; i<all_pipes; ++i){
                close(Pipes[i][WRITE]);
                close(Pipes[i][READ]);
            }
            int child_status;
            int allCorrect = 0;
            for(size_t l=0; l < commands.size()-1; ++l) {
                wait(&child_status);
                if (WEXITSTATUS(child_status) != 0)
                    allCorrect = 1;
            }
            
            add_to_favs(pipe_command, allCorrect);
        }
        commands.clear();
        command.clear();
    }   //Clear commands and command for the next iteration.
    save_favs();  // Guardar comandos favoritos antes de cerrar la shell
    sem_destroy(&print_semaphore);

    return 0;
}