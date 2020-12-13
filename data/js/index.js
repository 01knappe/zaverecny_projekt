$(document).ready(function () {

    if ($('#inline').length) {
        $('#inline').minicolors({
            inline: $('#inline').attr('data-inline') === 'true',
            theme: 'bootstrap'
        });
    }
});

var mode = 0;

function showClock() {
    $.post("/clock");
    mode = 0;    
    console.log(mode);
}

function showTemperature() {
    $.post("/temperature");
    mode = 1;
    console.log(mode);
}

function showHumidity() {
    $.post("/humidity");
    mode = 2;
    console.log(mode);
}

setInterval(function () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
      if (this.readyState == 4 && this.status == 200) {
          if(mode == 0){
            document.getElementById("time").innerHTML = this.responseText;
          }
          else if(mode == 1){
              if(this.responseText==2147483647){
                document.getElementById("time").innerHTML = "-- °C";
              }  
              else{
                document.getElementById("time").innerHTML = this.responseText+" °C";
                }          
        }
        else if(mode == 2){
            if(this.responseText==2147483647){
                document.getElementById("time").innerHTML = "-- %";
            }
            document.getElementById("time").innerHTML = this.responseText+" %";
        }          
      }
    };
    if(mode == 0){
        xhttp.open("GET", "/time", true);
        xhttp.send();
    }
    else if(mode == 1){
        xhttp.open("GET", "/teplota", true);
        xhttp.send();
    }
    else if(mode == 2){
        xhttp.open("GET", "/vlhkost", true);
        xhttp.send();
    }   
}, 1000);

function setColor() {
    var color = $('#inline').minicolors('rgbObject');
    var pcolor = $('#inline').minicolors('rgbString');
    console.log(color);
    $.post("/color", color);
    $(".change").css("background", pcolor);
}

/*var brightness = document.getElementById("rangeBrightness");
      var output = document.getElementById("demo");
      output.innerHTML = brightness.value;
  
      brightness.oninput = function() {
        output.innerHTML = this.value;
      }*/

function setBrightness() {
    var jas = $('#rangeBrightness').val();
    console.log(jas);
    $.post("/brightness", { brightness: jas });
}

setInterval(function () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            if(this.responseText==2147483647){
                document.getElementById("teplota").innerHTML = "-- °C";
              }
              else{
                document.getElementById("teplota").innerHTML = this.responseText+" °C";
                }
    
            }
    };
    xhttp.open("GET", "/teplota", true);
    xhttp.send();
  }, 1000);

setInterval(function () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
      if (this.readyState == 4 && this.status == 200) {
          if(this.responseText==2147483647){
            document.getElementById("vlhkost").innerHTML = "-- %";
          }
          else{
            document.getElementById("vlhkost").innerHTML = this.responseText+" %";
            }

          }
       
    };
    xhttp.open("GET", "/vlhkost", true);
    xhttp.send();
  }, 1000);



