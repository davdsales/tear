import React, { useState, useEffect } from 'react'
import '../styles/estoqueCompras.css'

// endereço do servidor em C++
const API = 'http://localhost:8080'

// linha em branco usada quando a pessoa adiciona um item novo
const ITEM_VAZIO = { idMaterial: '', quantidade: '', precoUnitario: '' }

// mostra o número como dinheiro tipo R$ 12,50
function dinheiro(valor) {
  return 'R$ ' + Number(valor).toFixed(2).replace('.', ',')
}

// tela de compras que aparece como aba dentro do estoque
function Compras({ embutida = false }) {
  // guardam as informações da tela
  const [compras, setCompras] = useState([])
  const [materiais, setMateriais] = useState([])
  const [mensagem, setMensagem] = useState(null)
  const [mostrarNova, setMostrarNova] = useState(false)
  const [fornecedor, setFornecedor] = useState('')
  const [itens, setItens] = useState([{ ...ITEM_VAZIO }])
  const [confirmar, setConfirmar] = useState(false)

  // busca os dados assim que a tela abre
  useEffect(() => { carregar() }, [])

  // pede ao servidor a lista de compras e a de materiais ao mesmo tempo
  async function carregar() {
    try {
      const [resCompras, resMat] = await Promise.all([
        fetch(API + '/api/compras'),
        fetch(API + '/api/materiais')
      ])
      setCompras(await resCompras.json())
      setMateriais(await resMat.json())
    } catch (err) {
    console.error("Erro na requisição carregar():", err)
    setMensagem({ erro: true, texto: 'Não consegui falar com o servidor C++. Ele está rodando?' })
    }
  }

  // função usada para salvar confirmar e excluir
  // manda o pedido para o servidor e mostra a mensagem de certo ou de erro
  async function enviar(url, metodo, corpo, textoOk) {
    try {
      const res = await fetch(API + url, {
        method: metodo,
        headers: { 'Content-Type': 'application/json' },
        body: corpo ? JSON.stringify(corpo) : undefined
      })
      const dados = await res.json()
      if (!res.ok) {
        setMensagem({ erro: true, texto: dados.mensagem })
        return false
      }
      setMensagem({ erro: false, texto: textoOk })
      carregar()  // atualiza a lista depois que deu certo
      return true
    } catch (err) {
      console.error("Erro na requisição enviar():", err)
      setMensagem({ erro: true, texto: 'Não consegui falar com o servidor C++. Ele está rodando?' })
      return false
    }
  }

  // muda só um campo de um item da compra
  function mudarItem(posicao, campo, valor) {
    setItens(itens.map((item, i) => (i === posicao ? { ...item, [campo]: valor } : item)))
  }

  // tira um item da compra
  function removerItem(posicao) {
    setItens(itens.filter((_, i) => i !== posicao))
  }

  // descobre a unidade do material escolhido tipo g ou m
  function unidadeDe(idMaterial) {
    const m = materiais.find(x => String(x.id) === String(idMaterial))
    return m ? m.unidade : ''
  }

  // soma quantidade vezes preço de todos os itens
  const totalPrevisto = itens.reduce((soma, i) => soma + (Number(i.quantidade) || 0) * (Number(i.precoUnitario) || 0), 0)

  // salva a compra nova e se der certo limpa o formulário
  async function salvarNova(e) {
    e.preventDefault()
    const corpo = { fornecedor, itens, confirmar }
    if (await enviar('/api/compras', 'POST', corpo, confirmar ? 'Compra confirmada e estoque atualizado.' : 'Compra salva como pendente.')) {
      setFornecedor('')
      setItens([{ ...ITEM_VAZIO }])
      setConfirmar(false)
      setMostrarNova(false)
    }
  }

  // pergunta antes de apagar a compra
  function excluir(c) {
    if (window.confirm('Excluir a compra #' + c.id + '?')) {
      enviar('/api/compras/' + c.id, 'DELETE', null, 'Compra excluída.')
    }
  }

  // o que aparece na tela
  return (
    <div className="ec-pagina">
      {/* topo com o botão que abre e fecha o formulário */}
      <div className={'ec-topo' + (embutida ? ' ec-topo-direita' : '')}>
        {!embutida && <h1>Compras</h1>}
        <button className="ec-botao" onClick={() => setMostrarNova(!mostrarNova)}>
          {mostrarNova ? 'Fechar' : '+ Nova compra'}
        </button>
      </div>

      {/* aviso de sucesso ou de erro */}
      {mensagem && <div className={'ec-aviso' + (mensagem.erro ? '' : ' ok')}>{mensagem.texto}</div>}

      {/* formulário de compra nova que só aparece quando está aberto */}
      {mostrarNova && (
        <form className="ec-form" onSubmit={salvarNova}>
          <h2>Nova compra</h2>
          <div className="ec-grade">
            <label className="ec-campo">Fornecedor
              <input value={fornecedor} onChange={e => setFornecedor(e.target.value)} />
            </label>
          </div>

          {/* uma linha para cada item da compra */}
          {itens.map((item, i) => (
            <div className="ec-grade" key={i}>
              <label className="ec-campo">Material
                <select value={item.idMaterial} onChange={e => mudarItem(i, 'idMaterial', e.target.value)} required>
                  <option value="">Escolha...</option>
                  {materiais.map(m => <option key={m.id} value={m.id}>{m.descricao}</option>)}
                </select>
              </label>
              <label className="ec-campo">Quantidade {unidadeDe(item.idMaterial) && '(' + unidadeDe(item.idMaterial) + ')'}
                <input type="number" step="any" min="0" value={item.quantidade} onChange={e => mudarItem(i, 'quantidade', e.target.value)} required />
              </label>
              <label className="ec-campo">Preço por {unidadeDe(item.idMaterial) || 'unidade'} (R$)
                <input type="number" step="0.001" min="0" value={item.precoUnitario} onChange={e => mudarItem(i, 'precoUnitario', e.target.value)} required />
              </label>
              {/* o botão remover só aparece se tiver mais de um item */}
              {itens.length > 1 && (
                <button type="button" className="ec-botao secundario perigo pequeno" onClick={() => removerItem(i)}>Remover</button>
              )}
            </div>
          ))}

          {/* coloca mais uma linha de item */}
          <div className="ec-botoes" style={{ marginBottom: 14 }}>
            <button type="button" className="ec-botao secundario pequeno" onClick={() => setItens([...itens, { ...ITEM_VAZIO }])}>
              + Adicionar item
            </button>
          </div>

          {/* se marcar a compra já entra no estoque */}
          <label style={{ display: 'block', marginBottom: 14, fontSize: 14 }}>
            <input type="checkbox" checked={confirmar} onChange={e => setConfirmar(e.target.checked)} />{' '}
            Já recebi: confirmar e dar entrada no estoque
          </label>

          <p><strong>Total: {dinheiro(totalPrevisto)}</strong></p>

          <div className="ec-botoes">
            <button className="ec-botao" type="submit">Salvar compra</button>
            <button className="ec-botao secundario" type="button" onClick={() => setMostrarNova(false)}>Cancelar</button>
          </div>
        </form>
      )}

      {/* tabela com todas as compras */}
      <table className="ec-tabela">
        <thead>
          <tr>
            <th>#</th><th>Data</th><th>Fornecedor</th><th>Itens</th><th>Total</th><th>Situação</th><th></th>
          </tr>
        </thead>
        <tbody>
          {/* mensagem quando ainda não tem nenhuma compra */}
          {compras.length === 0 && (
            <tr><td colSpan="7" className="ec-vazio">Nenhuma compra registrada ainda.</td></tr>
          )}
          {/* uma linha para cada compra */}
          {compras.map(c => (
            <tr key={c.id}>
              <td>{c.id}</td>
              <td>{c.data}</td>
              <td>{c.fornecedor || '—'}</td>
              <td>
                {c.itens.map((item, i) => (
                  <div key={i}>{item.quantidade} × {item.material}</div>
                ))}
              </td>
              <td>{dinheiro(c.total)}</td>
              <td>
                <span className={'ec-etiqueta ' + (c.confirmada ? 'ok' : '')}>
                  {c.confirmada ? 'Confirmada' : 'Pendente'}
                </span>
              </td>
              {/* confirmar e excluir só aparecem nas compras pendentes */}
              <td className="acoes">
                {!c.confirmada && (
                  <>
                    <button className="ec-botao pequeno"
                      onClick={() => enviar('/api/compras/' + c.id + '/confirmar', 'POST', null, 'Compra confirmada e estoque atualizado.')}>
                      Confirmar
                    </button>
                    <button className="ec-botao secundario perigo pequeno" onClick={() => excluir(c)}>Excluir</button>
                  </>
                )}
              </td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  )
}

export default Compras