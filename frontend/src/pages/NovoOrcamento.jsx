import { useState } from 'react'
import { Link } from 'react-router-dom'
import '../styles/estoqueCompras.css'
import '../styles/Orcamento.css'

const API = 'http://localhost:8080'

const FORM_VAZIO = {
  cliente: '',
  contato: '',
  materiais: '',
  maoDeObra: '',
  adicionais: '',
  margem: '20',
  desconto: ''
}

function dinheiro(valor) {
  return 'R$ ' + Number(valor).toFixed(2).replace('.', ',')
}

function percentual(valor) {
  return Number(valor).toFixed(1).replace('.', ',') + '%'
}

function numero(texto) {
  const n = Number(String(texto).replace(',', '.'))
  return Number.isFinite(n) ? n : 0
}

// mesmas contas do Orcamento.h, para o cliente ver o preco antes de salvar
function calcular(form) {
  const materiais = numero(form.materiais)
  const maoDeObra = numero(form.maoDeObra)
  const adicionais = numero(form.adicionais)
  const margem = numero(form.margem)
  const desconto = numero(form.desconto)

  const custoTotal = materiais + maoDeObra + adicionais
  const precoBruto = custoTotal * (1 + margem / 100)
  const precoFinal = Math.max(0, precoBruto - desconto)
  const lucro = precoFinal - custoTotal
  const margemReal = precoFinal > 0 ? (lucro / precoFinal) * 100 : 0

  return { custoTotal, precoBruto, precoFinal, lucro, margemReal, desconto }
}

function NovoOrcamento() {
  const [form, setForm] = useState({ ...FORM_VAZIO })
  const [mensagem, setMensagem] = useState(null)
  const [salvando, setSalvando] = useState(false)

  const resumo = calcular(form)

  function mudar(campo, valor) {
    setForm({ ...form, [campo]: valor })
  }

  // devolve o texto do primeiro problema encontrado, ou null se esta tudo certo
  function validar() {
    if (!form.cliente.trim()) return 'Informe o nome do cliente.'
    const campos = ['materiais', 'maoDeObra', 'adicionais', 'margem', 'desconto']
    for (const campo of campos) {
      if (numero(form[campo]) < 0) return 'Os valores não podem ser negativos.'
    }
    if (resumo.custoTotal <= 0) return 'Preencha pelo menos um custo (materiais, mão de obra ou adicionais).'
    return null
  }

  async function salvar(e) {
    e.preventDefault()
    const problema = validar()
    if (problema) {
      setMensagem({ erro: true, texto: problema })
      return
    }

    setSalvando(true)
    try {
      const res = await fetch(API + '/api/pedidos', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          cliente: form.cliente.trim(),
          contato: form.contato.trim(),
          materiais: numero(form.materiais),
          maoDeObra: numero(form.maoDeObra),
          adicionais: numero(form.adicionais),
          // o backend guarda a margem como fracao (0.20 = 20%)
          margem: numero(form.margem) / 100,
          desconto: numero(form.desconto)
        })
      })
      const dados = await res.json().catch(() => ({}))
      if (!res.ok) {
        setMensagem({ erro: true, texto: dados.mensagem || 'Não consegui salvar o orçamento. Confira os campos e tente de novo.' })
        return
      }
      setMensagem({ erro: false, texto: 'Orçamento salvo. Ele já aparece em Pedidos, na coluna Em Aberto.', link: true })
      setForm({ ...FORM_VAZIO })
    } catch (err) {
      console.error('Erro ao salvar orçamento:', err)
      setMensagem({ erro: true, texto: 'Não consegui falar com o servidor C++. Ele está rodando?' })
    } finally {
      setSalvando(false)
    }
  }

  const vendeAbaixoDoCusto = resumo.custoTotal > 0 && resumo.lucro < 0
  const descontoMaiorQuePreco = resumo.desconto > resumo.precoBruto && resumo.precoBruto > 0

  return (
    <div className="ec-pagina">
      <div className="ec-topo">
        <h1>Novo orçamento</h1>
      </div>

      {mensagem && (
        <div className={'ec-aviso' + (mensagem.erro ? '' : ' ok')}>
          {mensagem.texto}{' '}
          {mensagem.link && <Link to="/pedidos">Ver pedidos</Link>}
        </div>
      )}

      <form className="orc-layout" onSubmit={salvar}>
        <div className="orc-coluna">
          <section className="ec-form">
            <h2>Cliente</h2>
            <div className="ec-grade">
              <label className="ec-campo">
                Nome
                <input
                  type="text"
                  value={form.cliente}
                  onChange={e => mudar('cliente', e.target.value)}
                  placeholder="Ex.: Maria Clara"
                />
              </label>
              <label className="ec-campo">
                Contato (opcional)
                <input
                  type="text"
                  value={form.contato}
                  onChange={e => mudar('contato', e.target.value)}
                  placeholder="Telefone ou e-mail"
                />
              </label>
            </div>
          </section>

          <section className="ec-form">
            <h2>Custos</h2>
            <div className="ec-grade">
              <label className="ec-campo">
                Materiais (R$)
                <input
                  type="number" min="0" step="0.01"
                  value={form.materiais}
                  onChange={e => mudar('materiais', e.target.value)}
                  placeholder="0,00"
                />
              </label>
              <label className="ec-campo">
                Mão de obra (R$)
                <input
                  type="number" min="0" step="0.01"
                  value={form.maoDeObra}
                  onChange={e => mudar('maoDeObra', e.target.value)}
                  placeholder="0,00"
                />
              </label>
              <label className="ec-campo">
                Adicionais (R$)
                <input
                  type="number" min="0" step="0.01"
                  value={form.adicionais}
                  onChange={e => mudar('adicionais', e.target.value)}
                  placeholder="Embalagem, frete..."
                />
              </label>
            </div>
          </section>

          <section className="ec-form">
            <h2>Preço</h2>
            <div className="ec-grade">
              <label className="ec-campo">
                Lucro sobre o custo (%)
                <input
                  type="number" min="0" step="1"
                  value={form.margem}
                  onChange={e => mudar('margem', e.target.value)}
                />
              </label>
              <label className="ec-campo">
                Desconto (R$)
                <input
                  type="number" min="0" step="0.01"
                  value={form.desconto}
                  onChange={e => mudar('desconto', e.target.value)}
                  placeholder="0,00"
                />
              </label>
            </div>
            <p className="orc-dica">
              O lucro é somado ao custo total. Com 20%, um custo de R$ 100,00 vira um preço de R$ 120,00.
            </p>
          </section>

          <div className="ec-botoes">
            <button type="submit" className="ec-botao" disabled={salvando}>
              {salvando ? 'Salvando...' : 'Salvar orçamento'}
            </button>
            <button
              type="button"
              className="ec-botao secundario"
              onClick={() => { setForm({ ...FORM_VAZIO }); setMensagem(null) }}
            >
              Limpar
            </button>
          </div>
        </div>

        <aside className="orc-resumo" aria-live="polite">
          <h2>Resumo</h2>

          <dl>
            <div><dt>Custo total</dt><dd>{dinheiro(resumo.custoTotal)}</dd></div>
            <div><dt>Preço com lucro</dt><dd>{dinheiro(resumo.precoBruto)}</dd></div>
            <div><dt>Desconto</dt><dd>− {dinheiro(resumo.desconto)}</dd></div>
          </dl>

          <div className="orc-total">
            <span>Preço final</span>
            <strong>{dinheiro(resumo.precoFinal)}</strong>
          </div>

          <dl>
            <div><dt>Lucro em reais</dt><dd>{dinheiro(resumo.lucro)}</dd></div>
            <div><dt>Margem sobre o preço final</dt><dd>{percentual(resumo.margemReal)}</dd></div>
          </dl>

          {descontoMaiorQuePreco && (
            <p className="orc-alerta">O desconto é maior que o preço. O valor final fica em R$ 0,00.</p>
          )}
          {!descontoMaiorQuePreco && vendeAbaixoDoCusto && (
            <p className="orc-alerta">Com esse desconto, o preço fica abaixo do custo.</p>
          )}
        </aside>
      </form>
    </div>
  )
}

export default NovoOrcamento
