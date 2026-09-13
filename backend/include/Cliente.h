// David
#ifndef _CLIENTE_H_
#define _CLIENTE_H_
#include <string>
using namespace std;

class Cliente {
    private:
        string nome;
        string contato;
    public:
        //constutor
        Cliente(string nome = "", string contato = "")
        : nome(nome), contato(contato) {}
        // get
        string getNome() const { return nome; }
        string getContato() const { return contato; }
        // set
        void setNome(const string& novoNome) { nome = novoNome; }
        void setContato(const string& novoContato) { contato = novoContato; }
};

#endif