function loadIndex() {
    $.get("/access", function(data) {	 
        if(data.error!= "Sin conexion"){
            if(data.error == 'Perdida de conexion'){
                $('#error').removeClass("alert-success");
                $('#error').addClass("alert-danger");
                $('#error').text(data.error);
            }
            else{
                if(data.state == "Avanzando")
                    $('#state').addClass("text-success");
                else if(data.state == "Retrocediendo")
                    $('#state').addClass("text-danger");
                else{
                    $('#state').removeClass("text-danger");
                    $('#state').removeClass("text-success");
                }
            
                $('#state').text("Estado: " + data.state);

                $('#marcha').text("Marcha: " + data.marcha);

                if(data.front_distance > 75){
                    $('#front_distance').removeClass("text-danger");
                    $('#front_distance').removeClass("text-warning");
                    $('#front_distance').addClass("text-success");
                }
                else if(data.front_distance > 50){
                    $('#front_distance').removeClass("text-danger");
                    $('#front_distance').removeClass("text-success");
                    $('#front_distance').addClass("text-warning");
                }
                else{
                    $('#front_distance').removeClass("text-success");
                    $('#front_distance').removeClass("text-warning");
                    $('#front_distance').addClass("text-danger");
                }
                
                if(data.front_distance > 400){
                    $('#front_distance').text("Distancia Adelante: > 400cm");
                }
                else
                    $('#front_distance').text("Distancia Adelante: " + data.front_distance + "cm");

                if(data.back_distance > 75){
                    $('#back_distance').removeClass("text-danger");
                    $('#back_distance').removeClass("text-warning");
                    $('#back_distance').addClass("text-success");
                }
                else if(data.back_distance > 50){
                    $('#back_distance').removeClass("text-danger");
                    $('#back_distance').removeClass("text-success");
                    $('#back_distance').addClass("text-warning");
                }
                else{
                    $('#back_distance').removeClass("text-success");
                    $('#back_distance').removeClass("text-warning");
                    $('#back_distance').addClass("text-danger");
                }
                if(data.back_distance > 400){
                    $('#back_distance').text("Distancia Atras: > 400cm");
                }
                else
                    $('#back_distance').text("Distancia Atras: " + data.back_distance + "cm");
            }
        }
    });
}

// $( document ).ready(function () {
//     setInterval(loadIndex, 2500);
// });




