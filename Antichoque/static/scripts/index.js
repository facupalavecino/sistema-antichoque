function loadIndex() {
    console.log("loadIndex");
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

$( document ).ready(function () {
    var socket = io.connect('http://' + document.domain + ':' + location.port + '/sock');
    socket.on('connection success', function(respuesta) {
        $('#notification').removeClass("alert-danger");
        $('#notification').removeClass("alert-dark");
        $('#notification').addClass("alert-success");
    });

    socket.on('connection error', function(error){
        $('#notification').removeClass("alert-success");
        $('#notification').removeClass("alert-dark");
        $('#notification').addClass("alert-danger");
        $('#notification').text("Sin conexión con el vehículo");
        $('#notification').append("<p> ERROR: "+error.error+"</p>");

    });

    socket.on('fetch success', function(r){
        console.log("SUCCESS");
        $('#marcha').text(r.marcha);
        $('#state').text(r.state);
        $('#front_distance').text(r.front_distance + "cm");
        $('#back_distance').text(r.back_distance + "cm");
    });

    socket.on('fetch error', function(){
        console.log("ERROR");
        $('#marcha').text("Error");
        $('#state').text("Error");
        $('#front_distance').text("Error");
        $('#back_distance').text("Error");
    });

    $('#botonAccess').click(function(){
        console.log("EMITED");
        socket.emit('fetch');
        return null;
    });

    $('#botonConnect').click(function(){
        socket.emit('connection');
        return null;
    });

    $('#botonAvanzar').click(function(){
        socket.emit('control',{data:"Avanzar"});
        return null;
    });

    $('#botonFrenar').click(function(){
        socket.emit('control',{data:"Frenar"});
        return null;
    });

    $('#botonDerecha').click(function(){
        socket.emit('control',{data:"Derecha"});
        return null;
    });

    $('#botonIzquierda').click(function(){
        socket.emit('control',{data:"Izquierda"});
        return null;
    });

    $('#botonRetroceder').click(function(){
        socket.emit('control',{data:"Reversa"});
        return null;
    });

    $('#botonMarcha-').click(function(){
        socket.emit('control',{data:"Marcha-"});
        return null;
    });

    $('#botonMarchaUp').click(function(){
        socket.emit('control',{data:"Marcha+"});
        return null;
    });
});


