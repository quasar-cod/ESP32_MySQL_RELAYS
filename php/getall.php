<?php
  include 'database.php';
  
  if (!empty($_POST)) {
$myArray = array(); // Initialize an empty array instead of a single object
$pdo = Database::connect();

$sql = 'SELECT * FROM (SELECT *, ROW_NUMBER() OVER (PARTITION BY board ORDER BY date DESC, time DESC ) AS rn FROM esp32_activity) AS RankedData WHERE rn = 1';

$q = $pdo->prepare($sql);
$q->execute();

// Use a while loop to iterate through ALL fetched rows
while ($row = $q->fetch()) {
    // 1. Create a new object for each row
    $myObj = (object)array(); 
    
    // 2. Map the data from the row to the object properties
    $myObj->board = $row['board'];
    $myObj->activity = $row['activity'];
    $myObj->date = $row['date'];
    $myObj->time = $row['time'];
    
    // 3. Add the populated object to the array
    array_push($myArray, $myObj); 
}

$myJSON = json_encode($myArray);
echo $myJSON;
Database::disconnect();
  }
?>








