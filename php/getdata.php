<?php
  include 'database.php';
  
  if (!empty($_POST)) {
    $board = $_POST['board'];
    $myObj = (object)array();
    $pdo = Database::connect();
    $sql = 'SELECT * FROM esp32_table_dht11_leds_update WHERE board="' . $board . '"';
    // foreach ($pdo->query($sql) as $row) {

      $q = $pdo->prepare($sql);
      $q->execute();
      $row = $q->fetch();

      $date = date_create($row['date']);
      $dateFormat = date_format($date,"d-m-Y");
      $myObj->board = $row['board'];
      $myObj->LED_01 = $row['LED_01'];
      $myObj->LED_02 = $row['LED_02'];
      $myObj->ls_time = $row['time'];
      $myObj->ls_date = $dateFormat;
      $myJSON = json_encode($myObj);
      echo $myJSON;
    // }


    Database::disconnect();
  }
?>