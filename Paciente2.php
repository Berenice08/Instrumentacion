
<?php

$conexion=mysqli_connect('localhost','root','','ethernet');

?>


<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Resultados obtenidos de las mediciones</title>
    <link rel="stylesheet" href="assets/css/Tablas.css">	
</head>
<body >
	<?php require 'partials/header.php' ?>
	<br>
       <table class="table-sortable">
		<caption>Resultado toma de mediciones</caption>
		<thead>
			<tr>
				<th>ID</th>
				<th>FECHA</th>
				<th>SP02</th>
				<th>BPM</th>
				<th>FR</th>
			</tr>
        </thead>
       

		<?php 
		$sql="SELECT * from datos2";
		$result=mysqli_query($conexion,$sql);

		while($mostrar=mysqli_fetch_array($result)){
		 ?>

		<tr>
			<td class="c1"><?php echo $mostrar['id'] ?></td>
			<td class="c2"><?php echo $mostrar['event'] ?></td>
			<td class="c3"><?php echo $mostrar['SPO2'] ?></td>
			<td class="c4"><?php echo $mostrar['bpm'] ?></td>
			<td class="c5"><?php echo $mostrar['FR'] ?></td>
		</tr>
	<?php 
	}
	 ?>
	</table>
	<footer class="footer"> 
     <?php require 'partials/footer.php' ?>
  </footer>

<script src="/Arduino/Scripts/main.js"></script>

</body>
</html>