

#ifndef CLIENTPDS_PARAMETERS_EXCEPTION_H
#define CLIENTPDS_PARAMETERS_EXCEPTION_H

#include <iostream>
#include <exception>

class parameters_exception: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Comandi accettati: \n1) Nessun comando/Comando 'default': check continuo sulla directory specificata e salvataggio sul server della versione del client\n2) 'check_validity': controlla che la directory specificata sia valida (aggiornata con il server) oppure invalida (non aggiornata).\n3) 'restore': nel caso di una directory invalida salva sul client l'ultima versione presente sul server.\n";
    }
};

#endif //CLIENTPDS_PARAMETERS_EXCEPTION_H
