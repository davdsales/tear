// David
#ifndef _CLIENTE_H_
#define _CLIENTE_H_

#include <string>

class Cliente {
    private:
        int id;
        std::string nome;
        std::string contato;

    public:
        // construtor com valores padrão
        Cliente(int id = 0, const std::string& nome = "", const std::string& contato = "")
            : id(id), nome(nome), contato(contato) {}

        // getters com constantes
        int getId() const { return id; }
        std::string getNome() const { return nome; }
        std::string getContato() const { return contato; }

        // setters
        void setNome(const std::string& novoNome) { nome = novoNome; }
        void setContato(const std::string& novoContato) { contato = novoContato; }
};

#endif