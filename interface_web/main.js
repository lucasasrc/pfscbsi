fetch('https://corsproxy.io/?https://onlinewinxclub.com/centro-web/admin/lucas/api/listar_eventos.php')
    .then(response => response.json())
    .then(data => {
        const resultado = document.getElementById('resultado');
        // Ordena em ordem decrescente pelo ID
        data.sort((a, b) => Number(b.id) - Number(a.id));
        // Cria a tabela
        let tabela = `<table>
            <thead>
                <tr>
                    <th>ID</th>
                    <th>Tipo</th>
                    <th>Sucesso</th>
                    <th>Data/Hora</th>
                </tr>
            </thead>
            <tbody>
        `;
        data.forEach(evento => {
            let sucessoTexto = '-';
            if (evento.tipo === 'acesso') {
                if (evento.sucesso === "1") {
                    sucessoTexto = `<span style="color:green;font-weight:bold;">permitido</span>`;
                } else if (evento.sucesso === "0") {
                    sucessoTexto = `<span style="color:red;font-weight:bold;">negado</span>`;
                }
            } else {
                sucessoTexto = '-';
            }
            tabela += `
                <tr>
                    <td>${evento.id}</td>
                    <td>${evento.tipo}</td>
                    <td>${sucessoTexto}</td>
                    <td>${evento.data_hora}</td>
                </tr>
            `;
        });
        tabela += `
            </tbody>
        </table>`;
        resultado.innerHTML = tabela;
    })
    .catch(error => {
        document.getElementById('resultado').textContent = 'Erro ao carregar dados: ' + error;
    });