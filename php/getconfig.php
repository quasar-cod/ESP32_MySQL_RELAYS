<?php
// Set the content type header to ensure the browser/client knows it's receiving JSON
header('Content-Type: application/json');
include 'database.php';
$myArray = array();
$pdo = Database::connect();
$sql = 'SELECT * FROM esp32_config';
$q = $pdo->prepare($sql);
$q->execute();
while ($row = $q->fetch()) {
    $myObj = (object)array();
    // Mapping DB columns to desired object keys
    $myObj->board = $row['board'];
    $myObj->type = $row['type']; 
    $myObj->name = $row['name']; 
    $myObj->location = $row['location']; 
    array_push($myArray, $myObj); 
}
$myJSON = json_encode($myArray);
echo $myJSON;
Database::disconnect();
?>