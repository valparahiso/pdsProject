#include <iostream>
#include "user.h"
#include "parameters_exception.h"

int main(int argc, char* argv[])
{
    try
    {
        if(argc < 2 || argc > 3) { // user.exe "nome directory" "comando"

            throw parameters_exception();
        }

        //Creiamo un istanza della classe user che gestirà l'autenticazione e la connessione con il server
        /*
         * PARAMS
         * Directory: la cartella sui cui si desidera eseguire il programma
         * Command: il comanda da eseguire, può essere default/check_validity/restore,
         *          se il campo è vuoto si assume default come comando scelto
         */
        user u = (argc == 2) ? user(std::string(argv[1]), "default") : user(std::string(argv[1]), std::string(argv[2]));

        //Se i parametri inseriti sono corretti procediamo alla connesione con il server
        u.connection();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

