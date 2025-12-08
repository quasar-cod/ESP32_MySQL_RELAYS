<?php
  include 'database.php';
  
  if (!empty($_POST)) {
    $board = $_POST['board'];
    $myObj = (object)array();
    $pdo = Database::connect();
    $sql = 'SELECT a.*,SSID,site,time_on,time_off FROM esp32_activity a 
    inner join esp32_config c on a.board=c.board 
    inner join esp32_schedule s on s.board=c.board 
    WHERE c.board="' . $board . '"  and activity in ("UP","DOWN","OFF") order by date desc, time desc limit 1';

    $q = $pdo->prepare($sql);
    $q->execute();
    $row = $q->fetch();
    $myObj->board = $row['board'];
    $myObj->activity = $row['activity'];
    $myObj->SSID = $row['SSID'];
    $myObj->site = $row['site'];
    $myObj->time_on = $row['time_on'];
    $myObj->time_off = $row['time_off'];
    $myObj->ls_time = $row['time'];
    $myObj->ls_date = $row['date'];
    $myJSON = json_encode($myObj);
    echo $myJSON;
    // echo $sql;
    Database::disconnect();
  }
?>