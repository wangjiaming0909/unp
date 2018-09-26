#include <iostream>
#include <exception>

int print_Argc_Argv(int argc, char** argv);

int main(int argc, char** argv){
    print_Argc_Argv(argc, argv);
    return 0;
}

int print_Argc_Argv(int argc, char** argv){
   if(argc == 0){
       std::cout << "no parameters" << std::endl;
       return 0;
   } 
   for(int i = 0; i < argc; i++){
        try{
            std::cout << *(argv + i) << std:: endl;
        }
        catch(std::exception e){
            std::cout << "exception catched" << std::endl; 
            std::cout << e.what() << std::endl;
            return -1;
        }
   }
   return 0;

}