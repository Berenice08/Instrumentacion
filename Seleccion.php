<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="stylesheet" href="assets/css/style.css">
	<title>Document</title>
</head>
<body>
	<?php require 'partials/header.php' ?>

	 <table align="center" >

	  <tr>

	    <td>
	    	<div>
				<p class="centrado">Paciente 1</p>
	    	</div>
			<span class="codigo"> <form action="http://localhost/arduino/Paciente1.php" method="post">
		    <input type=image src="images/Paciente3.jpg" width="250" height="150"> </form> </span>
	    </td>
	    <td><div>
				<p class="centrado">Paciente 2</p>
	    	</div>
			<span class="codigo"> <form action="http://localhost/arduino/Paciente2.php" method="post">
		    <input type=image src="images/Paciente3.jpg" width="250" height="150"> </form> </span>
	    </td>
	    <td><div>
				<p class="centrado">Paciente 3</p>
	    	</div>
			<span class="codigo"> <form action="http://localhost/arduino/Paciente3.php" method="post">
		    <input type=image src="images/Paciente3.jpg" width="250" height="150"> </form> </span>  
	     </td>
	  </tr>
	</table>

 <footer class="footer"> 
     <?php require 'partials/footer.php' ?>
  </footer>
</body>
</html>