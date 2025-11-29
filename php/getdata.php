<?php
  include 'database.php';
  
  if (!empty($_POST)) {
    $board = $_POST['board'];
    $myObj = (object)array();
    $pdo = Database::connect();
    $sql = 'SELECT * FROM esp32_activity WHERE board="' . $board . '" order by date desc, time desc limit 1';
    // foreach ($pdo->query($sql) as $row) {

    $q = $pdo->prepare($sql);
    $q->execute();
    $row = $q->fetch();
    $myObj->board = $row['board'];
    $myObj->activity = $row['activity'];
    $myObj->ls_time = $row['time'];
    $myObj->ls_date = $row['date'];
    $myJSON = json_encode($myObj);
    echo $myJSON;
    // echo $sql;

    Database::disconnect();
  }
?>