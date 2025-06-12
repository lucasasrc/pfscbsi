<?php
require_once "conexao.php";

$pdo = getPDOConnection();

$dados = json_decode(file_get_contents("php://input"), true);
$sucesso = isset($dados['sucesso']) ? intval($dados['sucesso']) : 0;

$stmt = $pdo->prepare("INSERT INTO eventos_log (tipo, sucesso) VALUES ('acesso', ?)");
$stmt->execute([$sucesso]);

echo json_encode([
    "status" => "ok",
    "mensagem" => $sucesso ? "Acesso permitido registrado" : "Tentativa de acesso negado registrada"
]);
