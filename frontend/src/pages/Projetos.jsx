// Kailani
import '../styles/Projetos.css'

const projetosMock = [
  {
    id: 1,
    nome: "Xale para Dona Marta",
    cliente: "Marta Oliveira",
    status: "EM_ANDAMENTO",
    dataPrazo: "15/10/2026",
    progresso: 40,
  },
  {
    id: 2,
    nome: "Tapete Espinha de Peixe",
    cliente: "João Ferreira",
    status: "PLANEJAMENTO",
    dataPrazo: "02/11/2026",
    progresso: 0,
  },
  {
    id: 3,
    nome: "Toalha de Mesa Trama Dupla",
    cliente: "Casa Verde Decorações",
    status: "ATRASADO",
    dataPrazo: "20/09/2026",
    progresso: 75,
  },
  {
    id: 4,
    nome: "Manta em Lã Crua",
    cliente: "Beatriz Nunes",
    status: "CONCLUIDO",
    dataPrazo: "05/09/2026",
    progresso: 100,
  },
];

const statusInfo = {
  PLANEJAMENTO: { texto: "Planejamento", cor: "#7A6F63" },
  EM_ANDAMENTO: { texto: "Em Andamento", cor: "#C1502E" },
  PAUSADO: { texto: "Pausado", cor: "#B8860B" },
  CONCLUIDO: { texto: "Concluído", cor: "#3D8361" },
  ATRASADO: { texto: "Atrasado", cor: "#B33A3A" },
};

function CardProjeto({ projeto }) {
  const info = statusInfo[projeto.status] ?? statusInfo.PLANEJAMENTO;

  return (
    <div className="card-projeto">
      <div className="card-projeto-topo">
        <h4>{projeto.nome}</h4>
        <span className="badge-status" style={{ backgroundColor: info.cor }}>
          {info.texto}
        </span>
      </div>

      <a className="card-projeto-cliente">{projeto.cliente}</a>

      <div className="barra-progresso-fundo">
        <div
          className="barra-progresso-preenchida"
          style={{ width: `${projeto.progresso}%` }}
        />
      </div>

      <div className="card-projeto-rodape">
        <a>{projeto.progresso}% concluído</a>
        <a>Prazo: {projeto.dataPrazo}</a>
      </div>
    </div>
  );
}

function Projetos() {
  return (
    <div className="projetos-conteudo">
      <h3>Projetos</h3>
      <a>Acompanhe o andamento de cada peça em produção.</a>

      <div className="lista-projetos">
        {projetosMock.map((projeto) => (
          <CardProjeto key={projeto.id} projeto={projeto} />
        ))}
      </div>
    </div>
  );
}

export default Projetos;