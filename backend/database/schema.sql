-- login e cadastro (RotasUsuarios.h)
CREATE TABLE IF NOT EXISTS usuarios (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nome TEXT NOT NULL,
    email TEXT NOT NULL UNIQUE,
    senha TEXT NOT NULL
);

-- estoque (Estoque.h). Fio, Tecido e Aviamento ficam na mesma tabela
CREATE TABLE IF NOT EXISTS materiais (
    usuario_id INTEGER NOT NULL REFERENCES usuarios(id),
    id INTEGER NOT NULL,
    tipo TEXT NOT NULL CHECK (tipo IN ('FIO', 'TECIDO', 'AVIAMENTO')),
    nome TEXT NOT NULL,
    unidade TEXT NOT NULL,
    custo_unitario REAL NOT NULL,
    estoque_minimo REAL NOT NULL,
    marca TEXT, cor TEXT, metragem REAL,   -- fio
    composicao TEXT, largura REAL,         -- tecido
    detalhe TEXT,                          -- aviamento
    PRIMARY KEY (usuario_id, id)
);

-- o saldo de cada material e a soma das movimentacoes
CREATE TABLE IF NOT EXISTS movimentacoes (
    usuario_id INTEGER NOT NULL REFERENCES usuarios(id),
    id INTEGER NOT NULL,
    id_material INTEGER NOT NULL,
    tipo INTEGER NOT NULL, -- 0 entrada, 1 consumo, 2 ajuste
    quantidade REAL NOT NULL,
    data TEXT NOT NULL,
    observacao TEXT,
    PRIMARY KEY (usuario_id, id)
);

-- compras (ListaCompras.h)
CREATE TABLE IF NOT EXISTS compras (
    usuario_id INTEGER NOT NULL REFERENCES usuarios(id),
    id INTEGER NOT NULL,
    fornecedor TEXT,
    data TEXT NOT NULL,
    confirmada INTEGER NOT NULL DEFAULT 0,
    PRIMARY KEY (usuario_id, id)
);

CREATE TABLE IF NOT EXISTS itens_compra (
    usuario_id INTEGER NOT NULL REFERENCES usuarios(id),
    id_compra INTEGER NOT NULL,
    id_material INTEGER NOT NULL,
    quantidade REAL NOT NULL,
    preco_unitario REAL NOT NULL
);

-- orcamentos (GerenciadorOrcamentos.h)
CREATE TABLE IF NOT EXISTS orcamentos (
    usuario_id INTEGER NOT NULL REFERENCES usuarios(id),
    id INTEGER NOT NULL,
    cliente TEXT NOT NULL,
    contato TEXT,
    custo_materiais REAL NOT NULL,
    custo_mao_de_obra REAL NOT NULL,
    custo_adicionais REAL NOT NULL,
    margem_lucro REAL NOT NULL,
    desconto REAL NOT NULL,
    PRIMARY KEY (usuario_id, id)
);

-- pedidos (ContextoUsuario.h): cada pedido nasce de um orcamento
CREATE TABLE IF NOT EXISTS pedidos (
    usuario_id INTEGER NOT NULL REFERENCES usuarios(id),
    id INTEGER NOT NULL,
    id_orcamento INTEGER NOT NULL,
    status INTEGER NOT NULL,-- 0 em aberto, 1 aprovado, 2 em producao, 3 concluido, 4 cancelado
    data_criacao TEXT NOT NULL,
    PRIMARY KEY (usuario_id, id)
);

-- financeiro (GerenciamentoFinanceiro.h). Receita e Despesa herdam de transacao;
-- receita usa a coluna origem e despesa usa a coluna categoria
CREATE TABLE IF NOT EXISTS transacoes (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    usuario_id INTEGER NOT NULL REFERENCES usuarios(id),
    tipo TEXT NOT NULL CHECK (tipo IN ('Receita', 'Despesa')),
    descricao TEXT NOT NULL,
    valor REAL NOT NULL CHECK (valor > 0),
    data TEXT NOT NULL,
    origem TEXT,
    categoria TEXT
);