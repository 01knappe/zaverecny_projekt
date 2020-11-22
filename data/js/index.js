$(document).ready(function () {

    if ($('#inline').length) {
        $('#inline').minicolors({
            inline: $('#inline').attr('data-inline') === 'true',
            theme: 'bootstrap'
        });
    }
});

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
        document.getElementById("time").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/time", true);
    xhttp.send();
  }, 1000);



