<?php
// Program 2: Vypocet faktorialu (rekurzivne)
// Hlavni telo programu
//$z = put_string("Zadejte cislo pro vypocet faktorialu");
//$a = get_string();

$a = 10000000;//intval($a);
// Definice funkce pro vypocet hodnoty faktorialu
function factorial($n)
{
	if ($n < 2)
	{
		$result = 1;
	}
	else
	{
		$decremented_n = $n - 1;
		$temp_result = factorial($decremented_n);
		$result = $n * $temp_result;
	}
	return $result;
}

	
		$vysl = factorial($a);
		
$zzz = put_string($vysl); 
