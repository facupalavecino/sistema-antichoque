function loadIndex() {
    $.get("/access", function(data) {	 
        if(data.error!= "Sin conexion"){
            if(data.error == 'Perdida de conexion'){
                $('#error').removeClass("alert-success");
                $('#error').addClass("alert-danger");
                $('#error').text(data.error);
            }
            else{
                $('#error').text('Conexion exitosa.')
                if(data.state == "Avanzando")
                    $('#state').addClass("text-success");
                else if(data.state == "Retrocediendo")
                    $('#state').addClass("text-danger");
                else{
                    $('#state').removeClass("text-danger");
                    $('#state').removeClass("text-success");
                }
                
                if(typeof data.state == 'undefined'){
                    $('#btn-marchad').attr('disabled',true);
                    $('#btn-freno').attr('disabled',true);
                    $('#btn-marchaa').attr('disabled',true);
                    $('#btn-avanzar').attr('disabled',true);
                    $('#btn-izq').attr('disabled',true);
                    $('#btn-der').attr('disabled',true);
                    $('#btn-retro').attr('disabled',true);
                    $('#btn-marchad').attr('accesskey','');
                    $('#btn-freno').attr('accesskey','');
                    $('#btn-marchaa').attr('accesskey','');
                    $('#btn-avanzar').attr('accesskey','');
                    $('#btn-izq').attr('accesskey','');
                    $('#btn-der').attr('accesskey','');
                    $('#btn-retro').attr('accesskey','');
                    $('#error').text('Reconectando...')
                }
                else{
                    $('#btn-marchad').attr('disabled',false);
                    $('#btn-freno').attr('disabled',false);
                    $('#btn-marchaa').attr('disabled',false);
                    $('#btn-avanzar').attr('disabled',false);
                    $('#btn-izq').attr('disabled',false);
                    $('#btn-der').attr('disabled',false);
                    $('#btn-retro').attr('disabled',false);
                    $('#btn-marchad').attr('accesskey','q');
                    $('#btn-freno').attr('accesskey','f')
                    $('#btn-marchaa').attr('accesskey','e');
                    $('#btn-avanzar').attr('accesskey','w');
                    $('#btn-izq').attr('accesskey','a');
                    $('#btn-der').attr('accesskey','d');
                    $('#btn-retro').attr('accesskey','s');
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

                if(data.left_distance > 75){
                    $('#left_distance').removeClass("text-danger");
                    $('#left_distance').removeClass("text-warning");
                    $('#left_distance').addClass("text-success");
                }
                else if(data.left_distance > 50){
                    $('#left_distance').removeClass("text-danger");
                    $('#left_distance').removeClass("text-success");
                    $('#left_distance').addClass("text-warning");
                }
                else{
                    $('#left_distance').removeClass("text-success");
                    $('#left_distance').removeClass("text-warning");
                    $('#left_distance').addClass("text-danger");
                }
                
                if(data.left_distance > 400){
                    $('#left_distance').text("Distancia Izquierda: > 400cm");
                }
                else
                    $('#left_distance').text("Distancia Izquierda: " + data.left_distance + "cm");

                if(data.right_distance > 75){
                    $('#right_distance').removeClass("text-danger");
                    $('#right_distance').removeClass("text-warning");
                    $('#right_distance').addClass("text-success");
                }
                else if(data.right_distance > 50){
                    $('#right_distance').removeClass("text-danger");
                    $('#right_distance').removeClass("text-success");
                    $('#right_distance').addClass("text-warning");
                }
                else{
                    $('#right_distance').removeClass("text-success");
                    $('#right_distance').removeClass("text-warning");
                    $('#right_distance').addClass("text-danger");
                }
                
                if(data.right_distance > 400){
                    $('#right_distance').text("Distancia Derecha: > 400cm");
                }
                else
                    $('#right_distance').text("Distancia Derecha: " + data.right_distance + "cm");
            }
        }
    });
}

$( document ).ready(function () {
    setInterval(loadIndex, 2500);
});




