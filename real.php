<?php 
	$db_host = 'localhost';
	$db_user = 'root';
	$db_pass = '';
	$db_name = 'db_wsn';

	$connect = mysqli_connect($db_host, $db_user, $db_pass, $db_name);
	if (!$connect) {
		die ('Failed to connect to your database: '.mysqli_connect_error());
	}
	$sql = "SELECT * FROM tb_abang"; //string that contains a query select all from table mahasiswa
	$query = mysqli_query($connect, $sql); //run query at $sql using database that connected to $connect
	
	while ($row = mysqli_fetch_array($query)) //take data per row from $query as long as it's true 
	{
		?>
		<tr>
            <td><?php echo $row['id']?></td>
            <td><?php echo $row['tm']?></td>
            <td><?php echo $row['bpm']?></td>
		</tr>
		<?php	
	}

?>
	
