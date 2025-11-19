<!DOCTYPE HTML>
<html>
  <head>
    <title>ESP32 WITH MYSQL DATABASE</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <link rel="icon" href="data:,">
    <style>
      /* Container for the 3-state switch */
      .three-state-switch {
          display: inline-flex; /* Arrange items horizontally */
          border: 2px solid #3498db; /* Blue border */
          border-radius: 6px; /* Rounded corners for the whole control */
          overflow: hidden; /* Important for clean edges */
          margin-top: 10px; /* Space below the heading */
      }
      /* Hide the actual radio buttons */
      .three-state-switch input[type="radio"] {
          display: none;
      }
      /* Style for the labels (the visible buttons) */
      .three-state-switch label {
          padding: 8px 15px;
          font-size: 14px;
          cursor: pointer;
          background-color: #f1f1f1; /* Light gray background for unselected states */
          color: #333;
          transition: background-color 0.3s, color 0.3s;
          user-select: none; /* Prevent text selection */
          text-align: center;
      }
      /* Style for the separator between buttons */
      .three-state-switch label:not(:last-child) {
          border-right: 2px solid #3498db; /* Separator color matches the border */
      }
      /* Hover effect on unselected buttons */
      .three-state-switch label:hover:not(.three-state-switch input[type="radio"]:checked + label) {
          background-color: #b2c0faff;
      }
      /* Style for the selected (checked) state */
      .three-state-switch input[type="radio"]:checked + label {
          background-color: #3498db; /* Blue background for active state */
          color: white; /* White text for active state */
          font-weight: bold;
      }
      html {font-family: Arial; display: inline-block; text-align: center;}
      p {font-size: 1.2rem;}
      h3 {font-size: 0.8rem;}
      body {margin: 0;}
      .topnav {overflow: hidden; background-color: #480c80ff; color: white; font-size: 1.2rem;}
      .content {padding: 1px; }
      .card {background-color: white; box-shadow: 0px 0px 0px 0px rgba(140,140,140,.5); border: 2px solid #0c6980; border-radius: 15px;}
      .card-header {background-color: #0c6980; color: white; border-bottom-right-radius: 0px; border-bottom-left-radius: 0px; border-top-right-radius: 0px; border-top-left-radius: 0px;}
      .cards {max-width: 300px; margin: 0 auto; display: grid; grid-gap: 1rem; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));}
      .reading {font-size: 1.3rem;}
      .packet {color: #bebebe;}
    </style>
  </head>

  <body>
    <div class="topnav">
      <p>ESP32 WITH MYSQL</p>
    </div>
    <br>
    <div class="content">
      <div class="cards">
        <!--*************************************************** -->
        <div class="card">
          <div class="card-header">
            <h3 style="font-size: 1rem;">seminterrato</h3>
          </div>            
          <h3 style="font-size: 0.7rem;"><span id="ESP32_01_LTRD"></span></h3>
          <div class="three-state-switch">
            <input type="radio" id="pos_down" name="position" value="DOWN" onclick="SetThreeState('ESP32_01','DOWN')">
            <label for="pos_down">DOWN</label>
            <input type="radio" id="pos_off" name="position" value="O" onclick="SetThreeState('ESP32_01','O')" checked>
            <label for="pos_off">O</label>
            <input type="radio" id="pos_up" name="position" value="UP" onclick="SetThreeState('ESP32_01','UP')">
            <label for="pos_up">UP</label>
          </div>
        </div>
        <!--*************************************************** -->
        <div class="card">
          <div class="card-header">
            <h3 style="font-size: 1rem;">notte</h3>
          </div>            
          <h3 style="font-size: 0.7rem;"><span id="ESP32_02_LTRD"></span></h3>
          <div class="three-state-switch">
            <input type="radio" id="pos_down2" name="position" value="DOWN" onclick="SetThreeState('ESP32_02','DOWN')">
            <label for="pos_down2">DOWN</label>
            <input type="radio" id="pos_off2" name="position" value="O" onclick="SetThreeState('ESP32_02','O')" checked>
            <label for="pos_off2">O</label>
            <input type="radio" id="pos_up2" name="position" value="UP" onclick="SetThreeState('ESP32_02','UP')">
            <label for="pos_up2">UP</label>
          </div>
        </div>
        <!--*************************************************** -->
      </div>
    </div>
    <br>
    <div class="content">
      <div class="cards">
        <div class="card-header" style="border-radius: 15px;">
          <button onclick="window.open('recordtable.php', '_blank');">Open Record Table</button>
          <h3 style="font-size: 0.7rem;"></h3>
        </div>
      </div>
    </div>
<!-- ___________________________________________________________________________________________________________________________________ -->
    <script>
    Get_Data("ESP32_01");
    Get_Data("ESP32_02");
    setInterval(myTimer, 5000);     
//------------------------------------------------------------
      function SetThreeState(board,state) {
        console.log("setting board: " ,board,"New State:", state);
				if (window.XMLHttpRequest) {
          // code for IE7+, Firefox, Chrome, Opera, Safari
          xmlhttp = new XMLHttpRequest();
        } else {
          // code for IE6, IE5
          xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
        }
        xmlhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          }
        }
        xmlhttp.open("POST","updatedata1.php",true);
        xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        xmlhttp.send("board="+board+"&state="+state);
}//------------------------------------------------------------
      function myTimer() {
        Get_Data("ESP32_01");
//        Get_Data("ESP32_02");
      }
//------------------------------------------------------------
      function Get_Data(board) {
				if (window.XMLHttpRequest) {
          // code for IE7+, Firefox, Chrome, Opera, Safari
          xmlhttp = new XMLHttpRequest();
        } else {
          // code for IE6, IE5
          xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
        }
        xmlhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            const myObj = JSON.parse(this.responseText);
            // if (myObj.board == "ESP32_01") {
              document.getElementById("ESP32_01_LTRD").innerHTML = "Last Date: " + myObj.ls_date + " Time: " + myObj.ls_time ;
              if (myObj.LED_01 == "ON") {
                document.getElementById("pos_down").checked = true;
              } 
              if (myObj.LED_02 == "ON") {
                document.getElementById("pos_up").checked = true;
              }  
              if(myObj.LED_01 == "OFF" && myObj.LED_02 == "OFF"){
                document.getElementById("pos_off").checked = true;
              }
            // }
          }
        };
        console.log("getting board: " ,board);
        xmlhttp.open("POST","getdata.php",true);
        xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        xmlhttp.send("board="+board);
			}
    </script>
  </body>
</html>