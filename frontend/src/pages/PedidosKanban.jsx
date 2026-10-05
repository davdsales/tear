import React, { useEffect, useState } from 'react';
import '../styles/PedidosKanban.css';

// componente do quadro kanban com suporte a arrastar e soltar cartões entre colunas
function PedidosKanban() {
  const [pedidos, setPedidos] = useState([]);
  const [carregando, setCarregando] = useState(true);

  // estados de busca e filtro do kanban
  const [busca, setBusca] = useState('');
  const [filtro, setFiltro] = useState('Ativos');

  // todos os status possíveis de um pedido
  const todasColunas = [
    'Em Aberto',
    'Aprovado',
    'Em Produção',
    'Concluído',
    'Cancelado'
  ];

  // define quais colunas serão exibidas de acordo com o filtro selecionado
  const colunas =
    filtro === 'Ativos'
      ? ['Em Aberto', 'Aprovado', 'Em Produção']
      : filtro === 'Concluídos'
      ? ['Concluído']
      : filtro === 'Cancelados'
      ? ['Cancelado']
      : todasColunas;

  // filtra os pedidos pelo nome do cliente, contato ou ID
  const pedidosFiltrados = pedidos.filter((pedido) => {
    const termo = busca.trim().toLowerCase();

    if (!termo) {
      return true;
    }

    return (
      pedido.cliente?.toLowerCase().includes(termo) ||
      pedido.contato?.toLowerCase().includes(termo) ||
      String(pedido.id).includes(termo)
    );
  });

  // busca os pedidos cadastrados no backend C++
  const buscarPedidos = () => {
    setCarregando(true);

    fetch('http://localhost:8080/api/pedidos')
      .then((res) => {
        if (!res.ok) {
          throw new Error('Erro ao buscar pedidos');
        }

        return res.json();
      })
      .then((data) => {
        setPedidos(data);
      })
      .catch((err) => {
        console.error('Erro ao buscar pedidos:', err);
      })
      .finally(() => {
        setCarregando(false);
      });
  };

  // busca os pedidos quando a página é carregada
  useEffect(() => {
    buscarPedidos();
  }, []);

  // guarda o ID do pedido transferido ao iniciar o arrasto
  const handleDragStart = (e, id) => {
    e.dataTransfer.setData('text/plain', String(id));
    e.dataTransfer.effectAllowed = 'move';
  };

  // autoriza a zona de soltura
  const handleDragOver = (e) => {
    e.preventDefault();
    e.dataTransfer.dropEffect = 'move';
  };

  // movimenta o card para outra coluna e atualiza o backend
  const handleDrop = (e, novoStatus) => {
    e.preventDefault();

    const idStr = e.dataTransfer.getData('text/plain');

    if (!idStr) {
      return;
    }

    const id = parseInt(idStr, 10);

    const pedidoAnterior = pedidos.find((pedido) => pedido.id === id);

    if (!pedidoAnterior) {
      return;
    }

    // evita fazer uma requisição desnecessária se o card for solto
    // na própria coluna
    if (pedidoAnterior.status === novoStatus) {
      return;
    }

    // atualização visual imediata
    setPedidos((pedidosAtuais) =>
      pedidosAtuais.map((pedido) =>
        pedido.id === id
          ? { ...pedido, status: novoStatus }
          : pedido
      )
    );

    // persiste a alteração no backend
    fetch(`http://localhost:8080/api/pedidos/${id}`, {
      method: 'PUT',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({
        status: novoStatus
      })
    })
      .then((res) => {
        if (!res.ok) {
          throw new Error('Não foi possível atualizar o status do pedido');
        }

        return res.json();
      })
      .catch((err) => {
        console.error('Erro ao sincronizar status com C++:', err);

        // se o backend falhar, devolve o card para o status anterior
        setPedidos((pedidosAtuais) =>
          pedidosAtuais.map((pedido) =>
            pedido.id === id
              ? { ...pedido, status: pedidoAnterior.status }
              : pedido
          )
        );
      });
  };

  return (
    <div className="pk-pagina">
      <h1 className="pk-titulo">Fluxo de Pedidos</h1>

      {/* busca e filtros */}
      <div className="pk-filtros">
        <input
          type="text"
          className="pk-busca"
          placeholder="Buscar pedido..."
          value={busca}
          onChange={(e) => setBusca(e.target.value)}
        />

        {['Ativos', 'Concluídos', 'Cancelados', 'Todos'].map((opcao) => (
          <button
            key={opcao}
            type="button"
            className={'pk-filtro' + (filtro === opcao ? ' ativo' : '')}
            onClick={() => setFiltro(opcao)}
          >
            {opcao}
          </button>
        ))}
      </div>

      {/* colunas do kanban */}
      {carregando ? (
        <p className="pk-carregando">Carregando dados do servidor C++...</p>
      ) : (
        <div className="pk-quadro">
          {colunas.map((colunaStatus) => {
            const pedidosDaColuna = pedidosFiltrados.filter(
              (pedido) => (pedido.status || 'Em Aberto') === colunaStatus
            );

            return (
              <div
                key={colunaStatus}
                className="pk-coluna"
                onDragOver={handleDragOver}
                onDrop={(e) => handleDrop(e, colunaStatus)}
              >
                {/* cabeçalho da coluna */}
                <div className="pk-coluna-topo">
                  <h4>{colunaStatus}</h4>
                  <span className="pk-contador">{pedidosDaColuna.length}</span>
                </div>

                {/* cartões */}
                <div className="pk-cartoes">
                  {pedidosDaColuna.length === 0 ? (
                    <p className="pk-vazio">Nenhum pedido nesta coluna.</p>
                  ) : (
                    pedidosDaColuna.map((pedido) => (
                      <div
                        key={pedido.id}
                        className="pk-cartao"
                        draggable={true}
                        onDragStart={(e) => handleDragStart(e, pedido.id)}
                      >
                        <div className="pk-cartao-conteudo">
                          <h3>{pedido.cliente}</h3>

                          {pedido.descricao && (
                            <p className="pk-info">
                              <strong>Descrição:</strong> {pedido.descricao}
                            </p>
                          )}

                          {pedido.contato && (
                            <p className="pk-info">
                              <strong>Contato:</strong> {pedido.contato}
                            </p>
                          )}

                          <div className="pk-rodape">
                            <span className="pk-id">ID #{pedido.id}</span>
                            <span className="pk-valor">
                              R$ {Number(pedido.valor || 0).toFixed(2)}
                            </span>
                          </div>
                        </div>
                      </div>
                    ))
                  )}
                </div>
              </div>
            );
          })}
        </div>
      )}
    </div>
  );
}

export default PedidosKanban;