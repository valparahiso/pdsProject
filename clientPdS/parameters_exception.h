//
// Created by simo on 19/10/20.
//

#ifndef CLIENTPDS_PARAMETERS_EXCEPTION_H
#define CLIENTPDS_PARAMETERS_EXCEPTION_H

#include <iostream>
#include <exception>

class parameters_exception: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Comandi accettati: \n1) Nessun comando: check continuo sulla directory specificata e salvataggio sul server della versione del user\n2) 'check_validity': specifica se la directory specificata sia valida (aggiornata con il server) oppure invalida (non aggiornata).\n3) 'restore': nel caso di una directory invalida salva sul user l'ultima versione presente sul server.\n";
    }
};

#endif //CLIENTPDS_PARAMETERS_EXCEPTION_H
