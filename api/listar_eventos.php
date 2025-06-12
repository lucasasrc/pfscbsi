<?php
require_once "conexao.php";

$pdo = getPDOConnection();

$stmt = $pdo->query("SELECT id, tipo, sucesso, CONVERT_TZ(data_hora, '+00:00', '-04:00') AS data_hora FROM eventos_log");
$dados = $stmt->fetchAll(PDO::FETCH_ASSOC);

echo json_encode($dados);
