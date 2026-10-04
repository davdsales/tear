import React, { useEffect, useState } from 'react';

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
    <div
      style={{
        fontFamily: '"DM Sans", sans-serif'
      }}
    >
      <h1
        style={{
          fontSize: '28px',
          fontWeight: 'bold',
          marginBottom: '16px',
          color: '#3d3229',
          fontFamily: '"DM Sans", sans-serif'
        }}
      >
        Fluxo de Pedidos
      </h1>

      {/* busca e filtros */}
      <div
        style={{
          display: 'flex',
          gap: '8px',
          marginBottom: '20px',
          flexWrap: 'wrap'
        }}
      >
        <input
          type="text"
          placeholder="Buscar pedido..."
          value={busca}
          onChange={(e) => setBusca(e.target.value)}
          style={{
            padding: '8px 14px',
            borderRadius: '10px',
            border: '1px solid #E5DED8',
            fontSize: '14px',
            minWidth: '220px',
            outline: 'none',
            fontFamily: '"DM Sans", sans-serif'
          }}
        />

        {['Ativos', 'Concluídos', 'Cancelados', 'Todos'].map((opcao) => (
          <button
            key={opcao}
            type="button"
            onClick={() => setFiltro(opcao)}
            style={{
              padding: '8px 14px',
              borderRadius: '10px',
              border: '1px solid #E5DED8',
              background: filtro === opcao ? '#D96B27' : '#FFFFFF',
              color: filtro === opcao ? '#FFFFFF' : '#524B46',
              cursor: 'pointer',
              fontWeight: filtro === opcao ? 'bold' : 'normal',
              fontFamily: '"DM Sans", sans-serif'
            }}
          >
            {opcao}
          </button>
        ))}
      </div>

      {/* colunas do kanban */}
      {carregando ? (
        <p
          style={{
            color: '#786F6A'
          }}
        >
          Carregando dados do servidor C++...
        </p>
      ) : (
        <div
          style={{
            display: 'flex',
            gap: '20px',
            alignItems: 'flex-start',
            overflowX: 'auto',
            paddingBottom: '10px'
          }}
        >
          {colunas.map((colunaStatus) => {
            const pedidosDaColuna = pedidosFiltrados.filter(
              (pedido) =>
                (pedido.status || 'Em Aberto') === colunaStatus
            );

            return (
              <div
                key={colunaStatus}
                onDragOver={handleDragOver}
                onDrop={(e) => handleDrop(e, colunaStatus)}
                style={{
                  background: '#F7F4EF',
                  borderRadius: '16px',
                  padding: '16px',
                  width: '300px',
                  minHeight: '480px',
                  flexShrink: 0,
                  border: '2px dashed #E5DEC9'
                }}
              >
                {/* cabeçalho da coluna */}
                <div
                  style={{
                    display: 'flex',
                    justifyContent: 'space-between',
                    alignItems: 'center',
                    marginBottom: '16px'
                  }}
                >
                  <h4
                    style={{
                      margin: 0,
                      fontSize: '16px',
                      fontWeight: 'bold',
                      color: '#2B231F'
                    }}
                  >
                    {colunaStatus}
                  </h4>

                  <span
                    style={{
                      background: '#EAE5DF',
                      padding: '2px 8px',
                      borderRadius: '12px',
                      fontSize: '12px',
                      fontWeight: 'bold',
                      color: '#524B46'
                    }}
                  >
                    {pedidosDaColuna.length}
                  </span>
                </div>

                {/* cartões */}
                <div
                  style={{
                    display: 'flex',
                    flexDirection: 'column',
                    gap: '12px',
                    minHeight: '400px'
                  }}
                >
                  {pedidosDaColuna.length === 0 ? (
                    <p
                      style={{
                        margin: 0,
                        fontSize: '13px',
                        color: '#A89F98',
                        textAlign: 'center',
                        paddingTop: '20px'
                      }}
                    >
                      Nenhum pedido nesta coluna.
                    </p>
                  ) : (
                    pedidosDaColuna.map((pedido) => (
                      <div
                        key={pedido.id}
                        draggable={true}
                        onDragStart={(e) =>
                          handleDragStart(e, pedido.id)
                        }
                        style={{
                          background: '#FFF',
                          padding: '16px',
                          borderRadius: '12px',
                          border: '1px solid #EFEAE4',
                          boxShadow: '0 2px 4px rgba(0,0,0,0.04)',
                          cursor: 'grab',
                          userSelect: 'none'
                        }}
                      >
                        <div
                          style={{
                            pointerEvents: 'none'
                          }}
                        >
                          <h3
                            style={{
                              margin: '0 0 8px 0',
                              fontSize: '16px',
                              fontWeight: 'bold',
                              color: '#000000'
                            }}
                          >
                            {pedido.cliente}
                          </h3>

                          {pedido.descricao && (
                            <p
                              style={{
                                margin: '4px 0',
                                fontSize: '13px',
                                color: '#786F6A'
                              }}
                            >
                              <strong>Descrição:</strong>{' '}
                              {pedido.descricao}
                            </p>
                          )}

                          {pedido.contato && (
                            <p
                              style={{
                                margin: '4px 0',
                                fontSize: '13px',
                                color: '#786F6A'
                              }}
                            >
                              <strong>Contato:</strong>{' '}
                              {pedido.contato}
                            </p>
                          )}

                          <div
                            style={{
                              marginTop: '12px',
                              display: 'flex',
                              justifyContent: 'space-between',
                              alignItems: 'center',
                              paddingTop: '8px',
                              borderTop: '1px solid #F8F6F3'
                            }}
                          >
                            <span
                              style={{
                                fontSize: '11px',
                                color: '#A89F98'
                              }}
                            >
                              ID #{pedido.id}
                            </span>

                            <span
                              style={{
                                fontSize: '14px',
                                fontWeight: 'bold',
                                color: '#D96B27'
                              }}
                            >
                              R${' '}
                              {Number(pedido.valor || 0).toFixed(2)}
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