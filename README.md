# Tear

Sistema de gestão para artesãs: estoque de materiais, compras, orçamentos, pedidos e financeiro num lugar só.

Projeto da disciplina de Estruturas de Dados Orientadas a Objetos (EDOO), CIn/UFPE, 2026.2.

- **Back-end:** C++17, com servidor HTTP próprio e banco de dados SQLite
- **Front-end:** React com Vite
- **Página do projeto:** https://davdsales.github.io/tear/

---

## O que o sistema faz

| Tela | O que tem |
|---|---|
| Cadastro e login | Cada pessoa cria a própria conta. Toda conta começa vazia e só vê os próprios dados. |
| Início | Receita do mês, valor a receber, pedidos em andamento, aviso de estoque baixo, gráfico de receitas e despesas e cadastro de transações. |
| Estoque | Fios, tecidos e aviamentos, com entradas, consumos e ajustes. Tem uma aba de Compras: ao confirmar uma compra, os itens entram no estoque. |
| Orçamento | Soma materiais, mão de obra e adicionais, aplica a margem de lucro e o desconto e mostra o preço final. Ao salvar, vira um pedido. |
| Pedidos | Quadro kanban: cada pedido é um cartão arrastado entre as etapas, com busca e filtros. |

---

## Como rodar

O projeto tem **duas partes que rodam ao mesmo tempo**, cada uma no seu terminal:

1. o **servidor em C++** (back-end), na porta `8080`;
2. as **telas em React** (front-end), na porta `5173`.

### 1. Instale o que precisa (só na primeira vez)

| Programa | Para quê | Onde baixar |
|---|---|---|
| **Git** | Baixar o projeto. No Windows, já vem com o **Git Bash**, o terminal usado nos comandos abaixo. | https://git-scm.com/downloads |
| **Node.js** (versão LTS) | Rodar as telas. Já vem com o `npm`. | https://nodejs.org |
| **GCC do WinLibs** (só Windows) | Compilar o C++. | https://winlibs.com |
| VS Code (opcional) | Editar o código. | https://code.visualstudio.com |

**Não é preciso instalar banco de dados.** O SQLite já está dentro do projeto, na pasta `backend/sqlite`, e é compilado junto com o resto.

#### Instalando o GCC no Windows (WinLibs)

1. Em https://winlibs.com, baixe a versão **UCRT runtime**, **POSIX threads**, **Win64**, **sem LLVM**, em `.zip`.
2. Descompacte o zip direto no `C:\`. Tem que ficar uma pasta `C:\mingw64`, com `C:\mingw64\bin\g++.exe` dentro.
3. Confira no Git Bash:

```bash
/c/mingw64/bin/g++.exe --version
```

Tem que aparecer a versão do GCC (14 ou mais nova).

> **Por que o caminho completo `/c/mingw64/bin/g++.exe`?** Alguns computadores já têm um MinGW antigo (GCC 6.3), que não compila o projeto. Escrevendo o caminho completo, você garante que está usando o compilador certo.

#### Conferindo o Node.js

```bash
node --version
npm --version
```

Os dois têm que mostrar um número de versão. O Node precisa ser 18 ou mais novo.

### 2. Baixe o projeto

```bash
git clone https://github.com/davdsales/tear.git
cd tear
```

### 3. Rode o back-end (Terminal 1)

Abra o Git Bash na pasta `tear`:

```bash
cd backend
```

**Só na primeira vez**, compile o SQLite. Demora uns 30 segundos e não mostra nada enquanto roda:

```bash
/c/mingw64/bin/gcc.exe -c sqlite/sqlite3.c -o sqlite3.o
```

Compile o servidor:

```bash
/c/mingw64/bin/g++.exe -std=c++17 -D_WIN32_WINNT=0x0A00 -Iinclude main.cpp sqlite3.o -o sistema.exe -lws2_32 -static
```

Rode:

```bash
./sistema.exe
```

Tem que aparecer:

```text
Servidor Backend em C++ rodando em http://localhost:8080
```

**Deixe esse terminal aberto.** Se fechar, as telas param de funcionar.

Na primeira vez, o servidor cria sozinho o banco em `backend/database/tear.db`, com todas as tabelas.

#### Linux e macOS

Use o `gcc`/`g++` do sistema, sem o caminho do WinLibs:

```bash
gcc -c sqlite/sqlite3.c -o sqlite3.o
g++ -std=c++17 -Iinclude main.cpp sqlite3.o -o sistema -pthread -ldl
./sistema
```

### 4. Rode o front-end (Terminal 2)

Abra **outro** Git Bash na pasta `tear`:

```bash
cd frontend
npm install
npm run dev
```

O `npm install` só precisa rodar na primeira vez, ou quando alguém adicionar um pacote novo.

### 5. Abra no navegador

Acesse **http://localhost:5173**, crie uma conta e use o sistema.

### Para parar

Aperte `Ctrl + C` em cada um dos dois terminais.

### Da segunda vez em diante

Se você não mudou nada no C++, basta:

```bash
# Terminal 1
cd backend
./sistema.exe

# Terminal 2
cd frontend
npm run dev
```

Se mudou algum arquivo `.cpp` ou `.h`, **compile de novo** antes de rodar. O `sqlite3.o` não precisa ser gerado de novo.

---

## Problemas comuns

| O que aparece | O que fazer |
|---|---|
| `sqlite3.o: No such file or directory` | Falta compilar o SQLite. Rode o comando do `gcc` do passo 3. |
| `Permission denied` ao compilar | O `sistema.exe` antigo ainda está aberto. Feche com `taskkill //IM sistema.exe //F` e compile de novo. |
| `Nao consegui abrir a porta 8080` | Outro servidor já está rodando. Feche o outro terminal ou use o `taskkill` acima. |
| `std::thread` ou `mutex` não encontrado | O compilador usado é o MinGW antigo. Use o caminho completo `/c/mingw64/bin/g++.exe`. |
| Tela mostra "Não consegui falar com o servidor C++" | O back-end não está rodando. Volte ao passo 3. |
| Volta sempre para a tela de Entrar | A conta não existe mais no banco (por exemplo, se o `tear.db` foi apagado). Crie uma conta nova. |
| `npm` não é reconhecido | Instale o Node.js e abra um terminal novo. |
| `npm install` deu erro | Apague a pasta `frontend/node_modules` e rode `npm install` de novo. |
| A porta do front não é 5173 | Outro `npm run dev` está aberto. Use o endereço que aparecer no terminal. |

**Quer começar com o banco vazio?** Feche o servidor, apague `backend/database/tear.db` e rode `./sistema.exe` de novo. O banco é recriado do zero.

---

## Mapa dos arquivos

```text
tear/
├── README.md                     este arquivo
├── .gitignore                    o que não vai para o Git (executáveis, banco, node_modules)
├── .github/
│   ├── PULL_REQUEST_TEMPLATE.md  modelo de descrição dos PRs
│   └── workflows/
│       └── static.yml            publica a pasta docs/ no GitHub Pages
├── docs/                         página do projeto no GitHub Pages
│   ├── index.html
│   └── img/                      prints das telas
│
├── backend/                      servidor em C++
│   ├── main.cpp                  liga o servidor, registra as rotas e as rotas de pedidos
│   ├── CMakeLists.txt            alternativa para compilar com CMake
│   ├── database/
│   │   ├── schema.sql            documentação das tabelas do banco
│   │   └── tear.db               o banco (criado ao rodar; não vai para o Git)
│   ├── sqlite/
│   │   ├── sqlite3.c             biblioteca do SQLite
│   │   └── sqlite3.h
│   └── include/
│       │
│       │  ── banco de dados e sessão ──
│       ├── BancoDados.h          classes BancoDados (conexão) e Consulta (comandos SQL)
│       ├── ContextoUsuario.h     separa os dados de cada conta e cria as tabelas
│       │
│       │  ── estoque e compras ──
│       ├── Material.h            Material e as subclasses Fio, Tecido e Aviamento
│       ├── MovimentacaoEstoque.h entrada, consumo ou ajuste de estoque
│       ├── Estoque.h             lista de materiais, saldo e estoque mínimo
│       ├── ListaCompras.h        compras e itens; confirmar dá entrada no estoque
│       │
│       │  ── orçamentos e pedidos ──
│       ├── Cliente.h             nome e contato do cliente
│       ├── Orcamento.h           custos, margem, desconto e preço final
│       ├── GerenciadorOrcamentos.h  lista de orçamentos
│       ├── Pedido.h              pedido e seu status no kanban
│       │
│       │  ── financeiro ──
│       ├── Transacao.h           classe abstrata transacao
│       ├── Receita.h             receita (herda de transacao), com origem
│       ├── Despesa.h             despesa (herda de transacao), com categoria
│       ├── GerenciamentoFinanceiro.h  CRUD de transações e cálculos do mês
│       │
│       │  ── rotas da API ──
│       ├── RotasUsuarios.h       cadastro e login
│       ├── RotasEstoque.h        materiais, movimentações e compras
│       ├── RotasFinanceiro.h     transações, resumo e dashboard do Início
│       │
│       │  ── bibliotecas externas ──
│       ├── httplib.h             cpp-httplib: servidor HTTP
│       └── nlohmann/json.hpp     nlohmann/json: leitura e escrita de JSON
│
└── frontend/                     telas em React
    ├── index.html
    ├── package.json              dependências e scripts (dev, build, preview)
    ├── package-lock.json
    ├── vite.config.js
    └── src/
        ├── main.jsx              ponto de entrada; manda para o login quem não entrou
        ├── App.jsx               rotas das telas
        ├── usuario.js            guarda quem está logado e envia o id em toda requisição
        ├── index.css             estilos gerais
        ├── components/
        │   ├── Sidebar.jsx       barra lateral
        │   └── Sidebar.css
        ├── pages/
        │   ├── Cadastro.jsx
        │   ├── Entrar.jsx
        │   ├── Inicio.jsx        resumo e financeiro
        │   ├── Estoque.jsx       materiais, com a aba de compras
        │   ├── Compras.jsx       aba de compras (aparece dentro do Estoque)
        │   ├── NovoOrcamento.jsx
        │   └── PedidosKanban.jsx
        ├── styles/               um CSS por tela
        │   ├── Cadastro.css
        │   ├── Entrar.css
        │   ├── Inicio.css
        │   ├── estoqueCompras.css
        │   ├── Orcamento.css
        │   └── PedidosKanban.css
        └── assets/               logo e ícones dos cards do Início
```

---

## Telas (rotas do front-end)

| Endereço | Tela |
|---|---|
| `/` | Cadastro |
| `/entrar` | Login |
| `/inicio` | Início (resumo e financeiro) |
| `/estoque` | Estoque, com a aba de Compras |
| `/orcamento` | Novo orçamento |
| `/pedidos` | Kanban de pedidos |

---

## API do back-end

O servidor roda em `http://localhost:8080`. Com exceção de cadastro e login, toda rota precisa do cabeçalho `X-Usuario-Id` com o id de quem está logado. O front já envia isso sozinho (`usuario.js`).

| Método | Rota | O que faz |
|---|---|---|
| POST | `/api/usuarios` | Cadastro |
| POST | `/api/login` | Login |
| GET | `/api/materiais` | Lista materiais |
| POST | `/api/materiais` | Cria material (`FIO`, `TECIDO` ou `AVIAMENTO`) |
| PUT | `/api/materiais/:id` | Edita material |
| DELETE | `/api/materiais/:id` | Exclui material sem movimentações |
| GET | `/api/estoque` | Resumo do estoque |
| GET | `/api/estoque/movimentacoes` | Lista movimentações (filtro opcional `?idMaterial=1`) |
| POST | `/api/estoque/movimentacoes` | Registra `ENTRADA`, `CONSUMO` ou `AJUSTE` |
| GET | `/api/compras` | Lista compras |
| POST | `/api/compras` | Cria compra |
| POST | `/api/compras/:id/confirmar` | Confirma a compra e dá entrada no estoque |
| DELETE | `/api/compras/:id` | Exclui compra pendente |
| GET | `/api/pedidos` | Lista pedidos |
| POST | `/api/pedidos` | Cria orçamento e pedido |
| PUT | `/api/pedidos/:id` | Muda o status do pedido |
| GET | `/api/financeiro/transacoes` | Lista receitas e despesas |
| POST | `/api/financeiro/transacoes` | Cria receita ou despesa |
| PUT | `/api/financeiro/transacoes/:id` | Edita transação |
| DELETE | `/api/financeiro/transacoes/:id` | Exclui transação |
| GET | `/api/financeiro/resumo` | Totais do mês, gráfico de 6 meses e origens |
| GET | `/api/dashboard` | Os cards do Início |

---

## Banco de dados

SQLite, num arquivo só: `backend/database/tear.db`. As tabelas estão documentadas em `backend/database/schema.sql`.

| Tabela | Guarda |
|---|---|
| `usuarios` | Contas de acesso |
| `materiais` | Fios, tecidos e aviamentos |
| `movimentacoes` | Entradas, consumos e ajustes de estoque |
| `compras` e `itens_compra` | Compras e os itens de cada uma |
| `orcamentos` | Custos e preço de cada orçamento |
| `pedidos` | Pedidos e o status no kanban |
| `transacoes` | Receitas e despesas |

Todas as tabelas, menos `usuarios`, têm a coluna `usuario_id`. É assim que cada conta só vê os próprios dados.

---

## Orientação a objetos no projeto

| Conceito | Onde aparece |
|---|---|
| Herança | `Material` → `Fio`, `Tecido`, `Aviamento`; `transacao` → `Receita`, `Despesa` |
| Classe abstrata | `transacao`, com métodos virtuais puros |
| Polimorfismo | `descricao()` e `getTipo()`: cada subclasse responde do seu jeito |
| Encapsulamento | Atributos privados, acessados por getters e setters |
| Construtor e destrutor (RAII) | `BancoDados` abre o banco ao ser criado e fecha ao ser destruído |
| Sobrecarga | `Consulta::ligar` para texto, inteiro e número decimal |
| Ponteiros e referências | `vector<Material*>`, objetos passados por referência |

---

## Trabalho em grupo com Git

Nunca trabalhe direto na `develop` ou na `main`. Para cada tarefa, crie uma branch:

```bash
git checkout develop
git pull
git checkout -b feature/nome-da-tarefa
```

Ao terminar:

```bash
git add -A
git commit -m "feat: o que foi feito"
git push -u origin feature/nome-da-tarefa
```

Depois, abra um Pull Request para a `develop`. A `main` só recebe a versão final, vinda da `develop`.

---

## Equipe

| Pessoa | Parte |
|---|---|
| Heloisa | Início e financeiro |
| Maria Gabriela | Estoque e compras |
| Kailani | Orçamentos |
| David | Pedidos (kanban) |

---

Projeto acadêmico desenvolvido para a disciplina de EDOO, CIn/UFPE.
