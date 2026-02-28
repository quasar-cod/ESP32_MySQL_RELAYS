<?php
  include 'database.php';
  if (!empty($_POST)) {
    $board = $_POST['board'];
    $myObj = (object)array();
    $pdo = Database::connect();
    $sql = 'SELECT a.*,c.dutyc,c.time_on,c.time_off FROM esp32_activity a 
    inner join esp32_config c on a.board=c.board 
    WHERE a.board="' . $board . '" and activity in ("UP","DOWN","OFF") order by id desc limit 1';
    $q = $pdo->prepare($sql);
    $q->execute();
    $row = $q->fetch();
    $myObj->board = $row['board'];
    $myObj->activity = $row['activity'];
    $myObj->ts = $row['ts'];
    $myObj->dutyc = $row['dutyc'];
    $myObj->time_on = $row['time_on'];
    $myObj->time_off = $row['time_off'];
    $myJSON = json_encode($myObj);
    echo $myJSON;
    // echo $sql;
    Database::disconnect();
  }
?>
