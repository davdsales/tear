const CHAVE_LOGADO = 'tear_usuario'
const API = 'http://localhost:8080'

function ler(chave, padrao) {
  try {
    const texto = localStorage.getItem(chave)
    return texto ? JSON.parse(texto) : padrao
  } catch {
    return padrao
  }
}

function guardar(chave, valor) {
  try {
    localStorage.setItem(chave, JSON.stringify(valor))
  } catch {

  }
}

// guarda no navegador quem esta logado (id, nome e e-mail vindos do banco)
export function guardarLogin(usuario) {
  guardar(CHAVE_LOGADO, { id: usuario.id, nome: usuario.nome, email: usuario.email })
}

export function usuarioLogado() {
  const usuario = ler(CHAVE_LOGADO, null)
  return usuario && usuario.id ? usuario : null
}

export function sair() {
  try {
    localStorage.removeItem(CHAVE_LOGADO)
  } catch {

  }
}

// toda requisicao para o C++ leva o id de quem esta logado,
// assim o back devolve so os dados dessa conta
const fetchOriginal = window.fetch.bind(window)

window.fetch = async (url, opcoes = {}) => {
  const usuario = usuarioLogado()
  if (!String(url).startsWith(API) || !usuario) return fetchOriginal(url, opcoes)

  const headers = { ...opcoes.headers, 'X-Usuario-Id': String(usuario.id) }
  const res = await fetchOriginal(url, { ...opcoes, headers })

  // a conta nao existe mais no banco: volta para o login
  if (res.status === 401 && !String(url).includes('/api/login')) {
    sair()
    window.location.href = '/entrar'
  }
  return res
}

export function primeiroNome(nome) {
  return nome ? nome.trim().split(' ')[0] : ''
}

export function iniciais(nome) {
  if (!nome) return '?'
  const partes = nome.trim().split(' ').filter(Boolean)
  const primeira = partes[0][0]
  const ultima = partes.length > 1 ? partes[partes.length - 1][0] : ''
  return (primeira + ultima).toUpperCase()
}