#include <iostream>
#include <exception>

int print_Argc_Argv(int argc, char** argv);

int main(int argc, char** argv){
    print_Argc_Argv(argc, argv);
    return 0;
}

int print_Argc_Argv(int argc, char** argv){
    using namespace std;
   if(argc == 0){
       cout << "no parameters" << endl;
       return 0;
   } 
   for(int i = 0; i < argc; i++){
        try{
            cout << "argc: " << argc << endl;
            cout << *(argv + i) << " ";
        }
        catch(std::exception e){
            std::cout << "exception catched" << std::endl; 
            std::cout << e.what() << std::endl;
            return -1;
        }
   }
   cout << endl;
   return 0;
}