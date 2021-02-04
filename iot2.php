<?php
    // iot.php
    // Importamos la configuración
    require("config2.php");
    // Leemos los valores que nos llegan por GET
    $SPO2 = mysqli_real_escape_string($con, $_GET['SPO2']);
    $bpm = mysqli_real_escape_string($con, $_GET['bpm']);
    $FR = mysqli_real_escape_string($con, $_GET['FR']);

    // Esta es la instrucción para insertar los valores
    $query = "INSERT INTO datos(SPO2,bpm,FR) VALUES('".$SPO2."','".$bpm."','".$FR."')";
    
    
    // Ejecutamos la instrucción
    mysqli_query($con, $query);
 
    mysqli_close($con);
?>