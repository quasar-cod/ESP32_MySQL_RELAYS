<?php
  require 'database.php';
  if (!empty($_POST)) {
    date_default_timezone_set("Europe/Rome"); // Look here for your timezone : https://www.php.net/manual/en/timezones.php
    $tm = date("H:i:s");
    $dt = date("Y-m-d");
    $board = $_POST['board'];
    $state = $_POST['state'];
    $pdo = Database::connect();
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    if($state=="DOWN"){
      $sql = "UPDATE esp32_table_dht11_leds_update SET LED_01='ON',LED_02='OFF', time = '". $tm . "', date = '" . $dt . "' WHERE board = '".$board."'";
    }
    if($state=="UP"){
      $sql = "UPDATE esp32_table_dht11_leds_update SET LED_01='OFF',LED_02='ON', time = '". $tm . "', date = '" . $dt . "' WHERE board = '".$board."'";
    }
    if($state=="O"){
      $sql = "UPDATE esp32_table_dht11_leds_update SET LED_01='OFF',LED_02='OFF', time = '". $tm . "', date = '" . $dt . "' WHERE board = '".$board."'";
    }
    $q = $pdo->prepare($sql);
    $q->execute(array());

		$sql1 = "INSERT INTO esp32_table_dht11_leds_record (board,LED_01,time,date) values(?, ?, ?, ?)";
		$q = $pdo->prepare($sql1);
		$q->execute(array($board,$state,$tm,$dt));

    $myObj = (object)array();
    $myObj->board =$board;
    $myObj->state = $state;
    $myObj->ls_time = $tm;
    $myObj->ls_date = $dt;
    $myJSON = json_encode($myObj);  
    echo $myJSON;
    //echo $sql;
    Database::disconnect();
  }
?>