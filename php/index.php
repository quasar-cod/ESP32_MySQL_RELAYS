<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<style>

    html {
        font-family: Arial, sans-serif;
        display: inline-block;
        text-align: center;
        /* CRITICAL CHANGE: Set a large, fixed base font size for small screens (mobile).
           All 'rem' units will be calculated from this large base. */
        font-size: 26px; 
    }
    
    
    body {
        margin: 0;
        padding: 10px;
        font-size: 16px; 
    }
    
    /* 3. Typography Adjustments (Now proportional to the base size) */
    /* REM values remain the same, but they now calculate off a 26px mobile base. */
    p {font-size: 1rem;} 
    h3 {font-size: 2rem; margin: 0;}
    .reading {font-size: 1.3rem;}
    .packet {color: #bebebe; font-size: 0.5rem;} 

    /* 4. Top Navigation */
    .topnav {
        overflow: hidden; 
        background-color: #480c80ff; 
        color: white; 
        font-size: 3rem; 
    }

    /* 5. Content and Card Layout */
    .content {
        padding: 5px;
    }
    
    .cards {
        max-width: 1200px;
        margin: 0 auto;
        display: grid;
        /* Minmax width reduced to 100% on small screens */
        grid-template-columns: repeat(auto-fit, minmax(100%, 1fr)); 
        grid-gap: 15px;
        padding: 0;
    }


    .card {
        background-color: white; 
        box-shadow: 0 4px 8px rgba(0,0,0,0.1);
        border: 2px solid #0c6980; 
        border-radius: 15px;
        padding: 15px;
        text-align: center; 
        margin-bottom: 5px; 
    }
    
    .card-header {
        background-color: #0c6980; 
        color: white; 
        border-radius: 15px;
        padding: 10px;
    }

    /* 6. Switch and Item Layout */
    .config-item {
        display: flex;
        flex-direction: column; 
        justify-content: space-between;
        align-items: center;
        padding: 5px 0;
    }

    /* 7. Three-State Switch Styling */
    .three-state-switch {
        display: inline-flex;
        border: 3px solid #ccc;
        border-radius: 5px;
        overflow: hidden;
        margin-top: 10px;
    }
    .three-state-switch input[type="radio"] { display: none; }
    .three-state-switch label {
        padding: 10px 15px; 
        cursor: pointer;
        background-color: #f0f0f0;
        transition: background-color 0.3s;
        white-space: nowrap; 
        font-size: 3rem; /* Adjusted for readability on the 26px base */
    }
    .three-state-switch input[type="radio"]:checked + label {
        background-color: #4CAF50;
        color: white;
    }

    /* 8. Media Queries for Desktop Layout (Adjusted flex direction for PC) */
    @media (min-width: 600px) {
        .config-item {
            flex-direction: row; 
            text-align: left;
        }

        .three-state-switch {
            margin: 0;
        font-size: 1rem; /* Adjusted for readability on the 26px base */
        }
        
        .card h3 {
            text-align: left;
        }
    }
</style>
</head>
<body>
  <div id="config-list">
    <p>Loading data...</p>
  </div>
  <br>
  <div class="cards">
    <div class="card-header" style="border-radius: 15px;">
      <button onclick="window.open('recordtable.php', '_blank');" 
      style="padding: 20px 40px; font-size: 1.5rem; font-weight: bold; cursor: pointer; background-color: #3498db; 
      color: white; border: none; border-radius: 8px; transition: background-color 0.3s;">Record Table</button>
      <h3 style="font-size: 0.7rem;"></h3>
    </div>
  </div>
<script>
//------------------------------------------------------------
  document.addEventListener('DOMContentLoaded', fetchConfigData);
//------------------------------------------------------------
  setInterval(myTimer, 5000);  
//------------------------------------------------------------
  function myTimer() {
    Get_All();
  }
//------------------------------------------------------------
  function Get_All() {
    if (window.XMLHttpRequest) {
        // code for IE7+, Firefox, Chrome, Opera, Safari
        xmlhttp = new XMLHttpRequest();
    } else {
        // code for IE6, IE5
        xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
    console.log("getting ALL");
    xmlhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        const now = Date.now();
        // console.log(now);
        const myObj = JSON.parse(this.responseText);
        myObj.forEach((item, index) => {
          const fullDateTimeString = item.date + 'T' + item.time; 
          const itemTimestamp = new Date(fullDateTimeString).getTime();          
          // console.log(item.board + " " + now + " " + itemTimestamp + " " + item.activity); 
          // console.log(now - itemTimestamp);
          if (now - itemTimestamp > "40000" && item.activity != "OFF") {
            // SetThreeState(item.board, "OFF");
            // document.getElementById("pos_up"+item.board).checked = false;
            // document.getElementById("pos_down"+item.board).checked = false;
            // document.getElementById("pos_off"+item.board).checked = true;
            console.log("spegnimento");
          }
          else  
          {
            if (item.activity == "DOWN") {
              document.getElementById("pos_down"+item.board).checked = true;
            } 
            if (item.activity == "UP") {
              document.getElementById("pos_up"+item.board).checked = true;
            } 
            if(item.activity == "OFF" ){
              document.getElementById("pos_off"+item.board).checked = true;
            }
          }
        });
      };
    }
    xmlhttp.open("POST","getall.php",true);
    xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    xmlhttp.send("board=");
  }
//------------------------------------------------------------
  function SetThreeState(boardId, state) {
    console.log("SETTING: " ,boardId,"activity:", state);
    if (window.XMLHttpRequest) {
      // code for IE7+, Firefox, Chrome, Opera, Safari
      xmlhttp = new XMLHttpRequest();
    } else {
        // code for IE6, IE5
        xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
    // xmlhttp.onreadystatechange = function() {
    //   if (this.readyState == 4 && this.status == 200) {
    //      }
    //   }
    xmlhttp.open("POST","updatedata.php",true);
    xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    xmlhttp.send("board="+boardId+"&activity="+state);
  }
//------------------------------------------------------------
  async function fetchConfigData() {
    const listContainer = document.getElementById('config-list');
    listContainer.innerHTML = ''; // Clear the "Loading" message
    try {
      // 1. Make the AJAX request to the PHP endpoint
      const response = await fetch('getconfig.php');
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      // 2. Parse the JSON response
      const dataArray = await response.json(); 
      if (dataArray.length === 0) {
        listContainer.innerHTML = '<p>No configuration records found.</p>';
        return;
      }
      console.log("geting config");
      // 3. Iterate over the array and create list items with switches
      dataArray.forEach((item, index) => {
        // Create a unique ID suffix for inputs/labels in this row
        const rowId = index + 1; 
        // The actual board identifier is needed for the onclick function
        const boardIdentifier = `${item.board}`; // e.g., ESP32_1, ESP32_2, etc.
        // Create the container for the row
        const rowDiv = document.createElement('div');
        rowDiv.classList.add('card');
        // --- Left Side: Data Display ---
        const dataDisplay = document.createElement('h3');
        dataDisplay.textContent = `${item.name}`;
        rowDiv.appendChild(dataDisplay);
        // --- Right Side: Three-State Switch Control ---
        // Create the switch container
        const switchDiv = document.createElement('div');
        switchDiv.classList.add('three-state-switch');
        // Helper function to create radio inputs and labels
        function createRadioControl(value, labelText, isChecked) {
          const input = document.createElement('input');
          input.type = 'radio';
          input.id = `pos_${value.toLowerCase()}${boardIdentifier}`; // unique ID: pos_down1, pos_down2, etc.
          input.name = `position_${rowId}`; // unique NAME for mutual exclusion
          input.value = value;
          input.onclick = () => SetThreeState(boardIdentifier, value); // Pass board ID
          if (isChecked) {
            input.checked = true;
          }
          const label = document.createElement('label');
          label.htmlFor = input.id;
          label.textContent = labelText;
          switchDiv.appendChild(input);
          switchDiv.appendChild(label);
        }
        // Create DOWN control
        createRadioControl('DOWN', 'DOWN', false);
        // Create OFF control (set as default checked)
        createRadioControl('OFF', 'O', true); 
        // Create UP control
        createRadioControl('UP', 'UP', false);
        // Append the switch to the row
        rowDiv.appendChild(switchDiv);
        // Append the entire row to the main container
        listContainer.appendChild(rowDiv);
      });
      Get_All("")
    } catch (error) {
      // Display error message
      listContainer.innerHTML = `<p>Error loading data: ${error.message}</p>`;
      console.error("Fetch error:", error);
    }
  }
</script>
</body>
</html>