#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>

// deixa o número bonito na mensagem tipo 600 em vez de 600.000000
inline std::string formatarNumero(double valor) {
    std::ostringstream saida;
    saida << std::fixed << std::setprecision(3) << valor;
    std::string texto = saida.str();
    texto.erase(texto.find_last_not_of('0') + 1);  // tira os zeros que sobram no final
    if (!texto.empty() && texto.back() == '.') texto.pop_back();  // tira o ponto se ficar sozinho
    return texto;
}

// limpa o texto que o usuário digita tirando ponto e vírgula e quebra de linha
inline std::string limparCampo(std::string texto) {
    texto.erase(std::remove(texto.begin(), texto.end(), ';'), texto.end());
    texto.erase(std::remove(texto.begin(), texto.end(), '\n'), texto.end());
    return texto;
}

// molde de todo material do estoque
// fio tecido e aviamento seguem esse molde e cada um se descreve do seu jeito
class Material {
protected:
    // dados que todo material tem e que os filhos também podem usar
    int id;
    std::string nome;
    std::string unidade;     // jeito de medir como grama metro ou unidade
    double custoUnitario;    // quanto custa cada unidade
    double estoqueMinimo;    // abaixo disso aparece o aviso de estoque baixo

public:
    // cria o material e não deixa custo nem mínimo ficarem negativos
    Material(int id = 0, const std::string& nome = "", const std::string& unidade = "un",
             double custoUnitario = 0.0, double estoqueMinimo = 0.0)
        : id(id), nome(limparCampo(nome)), unidade(limparCampo(unidade)),
          custoUnitario(std::max(0.0, custoUnitario)),
          estoqueMinimo(std::max(0.0, estoqueMinimo)) {}

    // garante que o material seja apagado do jeito certo
    virtual ~Material() {}

    // cada tipo de material é obrigado a escrever esses métodos
    virtual std::string getTipo() const = 0;
    virtual std::string descricao() const = 0;

    // pegam os dados
    int getId() const { return id; }
    std::string getNome() const { return nome; }
    std::string getUnidade() const { return unidade; }
    double getCustoUnitario() const { return custoUnitario; }
    double getEstoqueMinimo() const { return estoqueMinimo; }

    // mudam os dados sem deixar texto sujo nem valor negativo
    void setId(int novoId) { id = novoId; }
    void setNome(const std::string& n) { nome = limparCampo(n); }
    void setUnidade(const std::string& u) { unidade = limparCampo(u); }
    void setCustoUnitario(double c) { custoUnitario = std::max(0.0, c); }
    void setEstoqueMinimo(double m) { estoqueMinimo = std::max(0.0, m); }
};

// fio de crochê e tricô
class Fio : public Material {
private:
    // dados que só o fio tem
    std::string marca;
    std::string cor;
    double metragem;  // quantos metros tem um novelo

public:
    // manda os dados comuns para o molde e guarda os dados do fio
    Fio(int id = 0, const std::string& nome = "", const std::string& unidade = "g",
        double custoUnitario = 0.0, double estoqueMinimo = 0.0,
        const std::string& marca = "", const std::string& cor = "", double metragem = 0.0)
        : Material(id, nome, unidade, custoUnitario, estoqueMinimo),
          marca(limparCampo(marca)), cor(limparCampo(cor)), metragem(std::max(0.0, metragem)) {}

    std::string getTipo() const override { return "FIO"; }

    // como o fio aparece na tela
    std::string descricao() const override {
        return "Fio " + nome + " - " + marca + " (" + cor + ")";
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
    // dados que só o tecido tem
    std::string composicao;  // do que o tecido é feito
    double largura;          // largura em centímetros

public:
    // manda os dados comuns para o molde e guarda os dados do tecido
    Tecido(int id = 0, const std::string& nome = "", const std::string& unidade = "m",
           double custoUnitario = 0.0, double estoqueMinimo = 0.0,
           const std::string& composicao = "", double largura = 0.0)
        : Material(id, nome, unidade, custoUnitario, estoqueMinimo),
          composicao(limparCampo(composicao)), largura(std::max(0.0, largura)) {}

    std::string getTipo() const override { return "TECIDO"; }

    // como o tecido aparece na tela
    std::string descricao() const override {
        return "Tecido " + nome + " - " + composicao + " (" + std::to_string((int)largura) + " cm)";
    }

    std::string getComposicao() const { return composicao; }
    double getLargura() const { return largura; }
    void setComposicao(const std::string& c) { composicao = limparCampo(c); }
    void setLargura(double l) { largura = std::max(0.0, l); }
};

// olhos botões enchimento e zíperes
class Aviamento : public Material {
private:
    std::string detalhe;  // um detalhe a mais tipo olho de segurança 10mm

public:
    // manda os dados comuns para o molde e guarda o detalhe
    Aviamento(int id = 0, const std::string& nome = "", const std::string& unidade = "un",
              double custoUnitario = 0.0, double estoqueMinimo = 0.0,
              const std::string& detalhe = "")
        : Material(id, nome, unidade, custoUnitario, estoqueMinimo),
          detalhe(limparCampo(detalhe)) {}

    std::string getTipo() const override { return "AVIAMENTO"; }

    // como o aviamento aparece na tela e se não tiver detalhe mostra só o nome
    std::string descricao() const override {
        return "Aviamento " + nome + (detalhe.empty() ? "" : " - " + detalhe);
    }

    std::string getDetalhe() const { return detalhe; }
    void setDetalhe(const std::string& d) { detalhe = limparCampo(d); }
};

#endif