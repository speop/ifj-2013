<?php
$q=put_string("zadej x: ");
$x=get_string();
$x=doubleval($x);

$zacatek=0.0;
$stred=$x/2.0;
$konec=$x;
$vysledek = 8;

if ($x*$x === $x){
  $vysledek=$x;}
else{
  $i=0.0;
  while ($i<1000.0){
    if ($stred*$stred === $x){
      $vysledek=$stred;}
    else{
      if ($stred*$stred>$x){
        $konec=$stred;
        $stred=($zacatek+$konec)/2.0;
      }
      else{
        $zacatek=$stred;
        $stred=($zacatek+$konec)/2.0;
        }      
    }
    $i=$i+1.0;
  }
}

$qq=put_string("druha odmocnina z ",$x," je ",$vysledek,"\n");
