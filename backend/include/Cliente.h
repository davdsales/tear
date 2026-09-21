// David
#ifndef _CLIENTE_H_
#define _CLIENTE_H_
#include <string>
using namespace std;

class Cliente {
    private:
        int id;
        string nome;
        string contato;
    public:
        // construtor
        Cliente(int id = 0,string nome = "", string contato = "")
        : id(id), nome(nome), contato(contato) {}
        // get
        int getId() const { return id; }
        string getNome() const { return nome; }
        string getContato() const { return contato; }
        // set
        void setNome(const string& novoNome) { nome = novoNome; }
        void setContato(const string& novoContato) { contato = novoContato; }
};

#endif