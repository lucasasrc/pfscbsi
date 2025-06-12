<?php
function getPDOConnection() {
    $host = '127.0.0.1';
    $dbname = 'lucas';
    $username = 'root';
    $password = 'Datadmin0!';

    try {
        $pdo = new PDO("mysql:host=$host;dbname=$dbname;charset=utf8mb4", $username, $password);
        $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
        return $pdo;
    } catch (PDOException $e) {
        die("Erro de conexão: " . $e->getMessage());
    }
}
