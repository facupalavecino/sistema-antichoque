function control(action) {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/control", true);
  xhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  xhttp.send("action=" + action); 
}
