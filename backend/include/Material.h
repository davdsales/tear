// Maria Gabriela
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>

// numero para mensagens: 600 em vez de 600.000000, e 0.5 em vez de 0.500000
inline std::string formatarNumero(double valor) {
    std::ostringstream saida;
    saida << std::fixed << std::setprecision(3) << valor;
    std::string texto = saida.str();
    texto.erase(texto.find_last_not_of('0') + 1);
    if (!texto.empty() && texto.back() == '.') texto.pop_back();
    return texto;
}
// tira o ';' de textos digitados pelo usuario, senao quebra o formato do .txt
inline std::string limparCampo(std::string texto) {
    texto.erase(std::remove(texto.begin(), texto.end(), ';'), texto.end());
    texto.erase(std::remove(texto.begin(), texto.end(), '\n'), texto.end());
    return texto;
}

// classe base abstrata: todo material tem os dados em comum,
// e cada tipo (fio, tecido, aviamento) descreve a si mesmo do seu jeito
class Material {
protected:
    int id;
    std::string nome;
    std::string unidade;     // g, m, un...
    double custoUnitario;    // custo por unidade de estoque (ex.: R$ por grama)
    double estoqueMinimo;

public:
    Material(int id = 0, const std::string& nome = "", const std::string& unidade = "un",
             double custoUnitario = 0.0, double estoqueMinimo = 0.0)
        : id(id), nome(limparCampo(nome)), unidade(limparCampo(unidade)),
          custoUnitario(std::max(0.0, custoUnitario)),
          estoqueMinimo(std::max(0.0, estoqueMinimo)) {}

    virtual ~Material() {}

    // metodos que cada tipo de material implementa
    virtual std::string getTipo() const = 0;
    virtual std::string descricao() const = 0;
    virtual std::string serializarExtras() const = 0;  // campos proprios, separados por ';'

    // getters
    int getId() const { return id; }
    std::string getNome() const { return nome; }
    std::string getUnidade() const { return unidade; }
    double getCustoUnitario() const { return custoUnitario; }
    double getEstoqueMinimo() const { return estoqueMinimo; }

    // setters
    void setId(int novoId) { id = novoId; }
    void setNome(const std::string& n) { nome = limparCampo(n); }
    void setUnidade(const std::string& u) { unidade = limparCampo(u); }
    void setCustoUnitario(double c) { custoUnitario = std::max(0.0, c); }
    void setEstoqueMinimo(double m) { estoqueMinimo = std::max(0.0, m); }

    // linha do arquivo: tipo;id;nome;unidade;custo;minimo;extras...
    std::string serializar() const {
        return getTipo() + ";" + std::to_string(id) + ";" + nome + ";" + unidade + ";" +
               std::to_string(custoUnitario) + ";" + std::to_string(estoqueMinimo) + ";" +
               serializarExtras();
    }
};

// fio de crochê/tricô
class Fio : public Material {
private:
    std::string marca;
    std::string cor;
    double metragem;  // metros por novelo

public:
    Fio(int id = 0, const std::string& nome = "", const std::string& unidade = "g",
        double custoUnitario = 0.0, double estoqueMinimo = 0.0,
        const std::string& marca = "", const std::string& cor = "", double metragem = 0.0)
        : Material(id, nome, unidade, custoUnitario, estoqueMinimo),
          marca(limparCampo(marca)), cor(limparCampo(cor)), metragem(std::max(0.0, metragem)) {}

    std::string getTipo() const override { return "FIO"; }

    std::string descricao() const override {
        return "Fio " + nome + " - " + marca + " (" + cor + ")";
    }

    std::string serializarExtras() const override {
        return marca + ";" + cor + ";" + std::to_string(metragem);
    }

    std::string getMarca() const { return marca; }
    std::string getCor() const { return cor; }
    double getMetragem() const { return metragem; }
    void setMarca(const std::string& m) { marca = limparCampo(m); }
    void setCor(const std::string& c) { cor = limparCampo(c); }
    void setMetragem(double m) { metragem = std::max(0.0, m); }
};

// tecidos e feltros
class Tecido : public Material {
private:
    std::string composicao;
    double largura;  // em cm

public:
    Tecido(int id = 0, const std::string& nome = "", const std::string& unidade = "m",
           double custoUnitario = 0.0, double estoqueMinimo = 0.0,
           const std::string& composicao = "", double largura = 0.0)
        : Material(id, nome, unidade, custoUnitario, estoqueMinimo),
          composicao(limparCampo(composicao)), largura(std::max(0.0, largura)) {}

    std::string getTipo() const override { return "TECIDO"; }

    std::string descricao() const override {
        return "Tecido " + nome + " - " + composicao + " (" + std::to_string((int)largura) + " cm)";
    }

    std::string serializarExtras() const override {
        return composicao + ";" + std::to_string(largura);
    }

    std::string getComposicao() const { return composicao; }
    double getLargura() const { return largura; }
    void setComposicao(const std::string& c) { composicao = limparCampo(c); }
    void setLargura(double l) { largura = std::max(0.0, l); }
};

// olhos, botoes, enchimento, zíperes...
class Aviamento : public Material {
private:
    std::string detalhe;  // ex.: "olho de segurança 10mm"

public:
    Aviamento(int id = 0, const std::string& nome = "", const std::string& unidade = "un",
              double custoUnitario = 0.0, double estoqueMinimo = 0.0,
              const std::string& detalhe = "")
        : Material(id, nome, unidade, custoUnitario, estoqueMinimo),
          detalhe(limparCampo(detalhe)) {}

    std::string getTipo() const override { return "AVIAMENTO"; }

    std::string descricao() const override {
        return "Aviamento " + nome + (detalhe.empty() ? "" : " - " + detalhe);
    }

    std::string serializarExtras() const override { return detalhe; }

    std::string getDetalhe() const { return detalhe; }
    void setDetalhe(const std::string& d) { detalhe = limparCampo(d); }
};

#endif
