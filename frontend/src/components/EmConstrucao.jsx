// Kailani
import Sidebar from "./Sidebar";

// placeholder pras telas que ainda não foram implementadas por ninguém do time.
// assim que alguém terminar a página de verdade, é só trocar o <EmConstrucao />
// pela rota correspondente no App.jsx — nada mais precisa mudar.
function EmConstrucao({ titulo }) {
  return (
    <div className="app-layout">
      <Sidebar />
      <div style={{ flex: 1, marginLeft: 20, fontFamily: "var(--fonte-texto)" }}>
        <h3 style={{ fontFamily: "var(--fonte-titulo)", color: "#3D3229" }}>{titulo}</h3>
        <p style={{ color: "#7A6F63" }}>Essa tela ainda está em construção.</p>
      </div>
    </div>
  );
}

export default EmConstrucao;