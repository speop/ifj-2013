<?php
// Program 2: Vypocet faktorialu (rekurzivne)
// Hlavni telo programu
//
$z = put_string("Zadejte cislo pro vypocet faktorialu\n");
//$a = get_string();/
//$a = intval($a);
$a = 4;
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
if ($a < 0) // Pokracovani hlavniho tela programu
{
$message = "Faktorial nelze spocitat\n";
}
else
{
$vysl = factorial($a);
$message = "Vysledek je: ";

}
$zzz = put_string($message);
put_string($vysl);
