<?php
require_once "conexao.php";

$pdo = getPDOConnection();

$stmt = $pdo->prepare("INSERT INTO eventos_log (tipo) VALUES ('alarme')");
$stmt->execute();

echo json_encode([
    "status" => "ok",
    "mensagem" => "Alarme registrado com sucesso"
]);
