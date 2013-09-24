#!/bin/bash
#SASA - Syntakticky analyzator syntaktickeho analyzatoru
#zbytek jindy
#doufam ze nikdo nebude menit(mazat) ten adresar test, out a info, bo je tu natvrdo
#nenastavuju tam u nich zadne prava takze je pak potreba je vytvorit rucne
#pro kazdy soubor se sekvenci  vstupu je v ./out vystup se stejnym jmenem souboru
#pro testy ktere skonci failed je vytvoreny ./info/failed_list
#parametry: -s ... ukaze vypisy z programu, bez nej neukazuje
function go
{
cd $test
strom=$(ls)
cd $zac
for a in $strom; do
	y=$(($y+1))
	echo ""
	sekv=$(cat $test/$a)
	#echo "Sekvence: $sekv"
	#uz neni potreba vypisovat
	if [ $d -ne 0  ] ; then
		IFS=' '
		for b in $sekv; do
			ret=$(cat ./info/tab | grep " $b,")
			echo $ret
		done
		IFS=$'\n'
	fi
	./main $test/$a 2> ./out/$a > /dev/null
	ec=$?
	

	fff=${a##*'ec_'} #orizne to co je pred _
	fff=${fff%%'_'*} #orizne to co je za _
# 	echo $fff
	if [ $fff -eq $ec ] ; then
		echo -en "#####TEST $y: \033[1;32mOK $ec\033[0m $a  #####"
		
	fi
	
	if [ $fff -ne $ec ] ; then
		z=$(($z+1))
		echo -en "#####TEST $y: \033[1;31mFAILED $ec\033[0m $a  #####"
		ot=$(cat ./out/$a)
		echo "$a $ot">> ./info/failed_list
			if [ $s -ne 0  ] ; then
				cat ./out/$a
			fi
	fi
done

echo $'\n'
echo $'\n'"#####Nesouhlasi navratovy kod::#####"
ot=$(cat ./info/failed_list)
echo -en "\033[1;31m$ot\033[0m"$'\n'
echo -en "\033[1;31m------$z/$y nesouhlasi------\033[0m"$'\n'
}

zac=$(pwd)
ec=0
i=1
y=0
z=0
mm=0

hovnosracka=$(make)
mkdir info
rm -r info
mkdir info

mkdir out
rm -r out
mkdir out



touch ./info/failed_list
rm ./info/failed_list
touch ./info/failed_list

function print_help
{ #vypisuje napovedu
	echo "Usage: $0 [-vtrsc] TEST_DIR [REGEX] " $'\n'$'\n' \
	 "Ahoj, ja jsem Sasa, syntakticka analyzatorka syntaktickeho analyzatoru"$'\n' \
	"doufam ze nikdo nebude menit(mazat) ten adresar ./test, ./out a ./info, bo je tu natvrdo"$'\n' \
	 "nenastavuju tam u nich zadne prava takze je pak potreba je vytvorit rucne"$'\n' \
	"pro kazdy soubor se sekvenci  vstupu je v ./out vystup se stejnym jmenem souboru"$'\n' \
	"pro testy ktere skonci failed je vytvoreny ./info/failed_list"$'\n' \
	"v ./info je tab ve ktere je seznam terminalu"$'\n' \
	"parametry: "$'\n' \
	"				-s ... ukaze vypisy z programu u neshodnych vysledku, bez nej neukazuje"$'\n'
}

print_help
s=0 #priznaky pro jednotlive parametry
d=0
while getopts :sd opt; #zpracovani parametru
	do
		case $opt in
			s)	s=1;;
			d) d=1;;
		?) #neznamy parametr
			print_help;;
		esac
	done
IFS=$'\n'
shift $((OPTIND-1)) #odfiltrovani  parametru s -

if [ $# -gt 2 ] ; then #nespravny pocet parametru
 			print_help
fi

cd $zac
test=$1
go

cd $zac
cd ./out
d=$(ls)
ll=0
echo "#####Nesouhlasi cislo radku na kterem je chyba:#####"
for a in $d; do
	mm=$(($mm+1))
	ret=$(cat ./$a) #prvni znak je radek
	radek=${ret%%':'*} #cislo radku ktere hazi program
	fff=${a%%'.'*}
	fff=${fff##*'-'}
	if [ "$radek" != "$fff" ] ; then
		
		if [ "$fff" -ne 0 ] ; then
			ll=$(($ll+1))
			echo -en "\033[1;31m$a o:$fff r:$radek\033[0m"$'\n'
		fi

	fi
done
echo -en "\033[1;31m------$ll/$mm nesouhlasi------\033[0m"$'\n'
