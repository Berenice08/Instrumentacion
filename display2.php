<?php
$url=$_SERVER['REQUEST_URI'];
header("Refresh: 5; URL=$url"); // Refresh the webpage every 5 seconds
?>
<html>
<head>
<title>Arduino Ethernet Database</title>
<style type="text/css">
.table_titles {
padding-right: 20px;
padding-left: 20px;
color: #000;
}
.table_titles {
color: #FFF;
background-color: #0000FF;
}
table {
border: 2px solid #333;
}
body { font-family: "Trebuchet MS", Courier; }
</style>
</head>
<body>
<h1>Datos Paciente (Proyecto Instru)</h1>
<table border="0" cellspacing="0" cellpadding="4">
<tr>
<td class="table_titles">ID</td>
<td class="table_titles">Fecha y Hora </td>
<td class="table_titles">SPO2</td>
<td class="table_titles">bpm</td>
<td class="table_titles">FR</td>
</tr>
<?php
include('config2.php');
$result = mysqli_query($con,'SELECT * FROM datos ORDER BY id DESC');
// Process every record
$oddrow = true;
while($row = mysqli_fetch_array($result))
{
if ($oddrow)
{
$css_class=' class="table_cells_odd"';
}
else
{
$css_class=' class="table_cells_even"';
}
$oddrow = !$oddrow; 
echo "<tr>";
echo "<td '.$css_class.'>" . $row['id'] . "</td>";
echo "<td '.$css_class.'>" . $row['event'] . "</td>";
echo "<td '.$css_class.'>" . $row['SPO2'] . "</td>";
echo "<td '.$css_class.'>" . $row['bpm'] . "</td>";
echo "<td '.$css_class.'>" . $row['FR'] . "</td>";
echo "</tr>"; 
}
 
// Close the connection
mysqli_close($con);
?>
</table>
</body>
</html>