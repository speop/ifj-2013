#!/bin/bash
#SASA - Syntakticky analyzator syntaktickeho analyzatoru
#zbytek jindy
#doufam ze nikdo nebude menit(mazat) ten adresar test, out a info, bo je tu natvrdo
#nenastavuju tam u nich zadne prava takze je pak potreba je vytvorit rucne
#pro kazdy soubor se sekvenci  vstupu je v ./out vystup se stejnym jmenem souboru
#pro testy ktere skonci failed je vytvoreny ./info/failed_list
#parametry: -s ... ukaze vypisy z programu, bez nej neukazuje

zac=$(pwd)
test= "test"
ec=0
i=1
y=0
z=0

hovnosracka=$(make)

touch ./info/failed1_list
rm ./info/failed1_list
touch ./info/failed1_list

touch ./info/failed2_list
rm ./info/failed2_list
touch ./info/failed2_list

touch ./info/failed3_list
rm ./info/failed3_list
touch ./info/failed3_list

touch ./info/failed3_list
rm ./info/failed3_list
touch ./info/failed3_list

touch ./info/failed4_list
rm ./info/failed4_list
touch ./info/failed4_list

touch ./info/failed5_list
rm ./info/failed5_list
touch ./info/failed5_list

touch ./info/failed10_list
rm ./info/failed10_list
touch ./info/failed10_list

touch ./info/failed11_list
rm ./info/failed11_list
touch ./info/failed11_list

touch ./info/failed12_list
rm ./info/failed12_list
touch ./info/failed12_list

touch ./info/failed13_list
rm ./info/failed13_list
touch ./info/failed13_list

touch ./info/failed99_list
rm ./info/failed99_list
touch ./info/failed99_list

touch ./info/ok_list
rm ./info/ok_list
touch ./info/ok_list

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



echo "Ok, tak ja pustim SA a kouknu co to udela:"
cd "test"
strom=$(ls)
cd $zac
for a in $strom; do
	echo "#####TEST $i:#####  $a"
	sekv=$(cat ./test/$a)
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
	./main ./test/$a 2> ./out/$a > /dev/null
	ec=$?
	if [ $ec -eq 0  ] ; then
			echo -en "\033[1;32mOK\033[0m"$'\n'
			y=$(($y+1))
			echo $a >> ./info/ok_list
	fi
	if [ $ec -eq 1  ] ; then
			echo -en "\033[1;31mFAILED $ec\033[0m"$'\n'
			y=$(($y+1))
			echo $a >> ./info/failed1_list
	fi
	if [ $ec -eq 2  ] ; then
			echo -en "\033[1;31mFAILED $ec\033[0m"$'\n'
			y=$(($y+1))
			echo $a >> ./info/failed2_list
	fi
	if [ $ec -eq 3  ] ; then
			echo -en "\033[1;31mFAILED $ec\033[0m"$'\n'
			y=$(($y+1))
			echo $a >> ./info/failed3_list
	fi
	if [ $ec -eq 4  ] ; then
			echo -en "\033[1;31mFAILED $ec\033[0m"$'\n'
			y=$(($y+1))
			echo $a >> ./info/failed4_list
	fi
	if [ $ec -eq 5  ] ; then
			echo -en "\033[1;31mFAILED $ec\033[0m"$'\n'
			y=$(($y+1))
			echo $a >> ./info/failed5_list
	fi
	if [ $ec -eq 10  ] ; then
			echo -en "\033[1;31mFAILED $ec\033[0m"$'\n'
			y=$(($y+1))
			echo $a >> ./info/failed10_list
	fi
	if [ $ec -eq 11  ] ; then
			echo -en "\033[1;31mFAILED $ec\033[0m"$'\n'
			y=$(($y+1))
			echo $a >> ./info/failed11_list
	fi
	if [ $ec -eq 12 ] ; then
			echo -en "\033[1;31mFAILED $ec\033[0m"$'\n'
			y=$(($y+1))
			echo $a >> ./info/failed12_list
	fi
	if [ $ec -eq 13  ] ; then
			echo -en "\033[1;31mFAILED $ec\033[0m"$'\n'
			y=$(($y+1))
			echo $a >> ./info/failed13_list
	fi
	if [ $ec -eq 99  ] ; then
			echo -en "\033[1;31mFAILED $ec\033[0m"$'\n'
			y=$(($y+1))
			echo $a >> ./info/failed99_list
	fi
	i=$(($i+1))
done


echo $'\n'"#####Vypis spatnych vystupu:#####"
#ec==0
ot=$(cat ./info/ok_list | grep "ec=1")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
		echo $g >> ./info/ok_diff
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/ok_list | grep "ec=2")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
		echo $g >> ./info/ok_diff
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/ok_list | grep "ec=3")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
		echo $g >> ./info/ok_diff
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/ok_list | grep "ec=4")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
		echo $g >> ./info/ok_diff
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/ok_list | grep "ec=5")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
		echo $g >> ./info/ok_diff
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/ok_list | grep "ec=10")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
		echo $g >> ./info/ok_diff
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/ok_list | grep "ec=11")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
		echo $g >> ./info/ok_diff
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/ok_list | grep "ec=12")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
		echo $g >> ./info/ok_diff
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/ok_list | grep "ec=13")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
		echo $g >> ./info/ok_diff
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/ok_list | grep "ec=99")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
		echo $g >> ./info/ok_diff
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done

#ec==1
ot=$(cat ./info/failed1_list | grep "ec=0")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed1_list | grep "ec=2")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed1_list | grep "ec=3")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed1_list | grep "ec=4")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed1_list | grep "ec=5")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed1_list | grep "ec=10")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed1_list | grep "ec=11")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed1_list | grep "ec=12")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed1_list | grep "ec=13")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed1_list | grep "ec=99")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
#ec==2
ot=$(cat ./info/failed2_list | grep "ec=0")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed2_list | grep "ec=1")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed2_list | grep "ec=3")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed2_list | grep "ec=4")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed2_list | grep "ec=5")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed2_list | grep "ec=10")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed2_list | grep "ec=11")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed2_list | grep "ec=12")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed2_list | grep "ec=13")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed2_list | grep "ec=99")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
#ec==3
ot=$(cat ./info/failed3_list | grep "ec=0")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed3_list | grep "ec=1")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed3_list | grep "ec=2")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed3_list | grep "ec=4")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed3_list | grep "ec=5")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed3_list | grep "ec=10")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed3_list | grep "ec=11")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed3_list | grep "ec=12")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed3_list | grep "ec=13")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed3_list | grep "ec=99")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
#ec==4
ot=$(cat ./info/failed4_list | grep "ec=0")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed4_list | grep "ec=1")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed4_list | grep "ec=2")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed4_list | grep "ec=3")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed4_list | grep "ec=5")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed4_list | grep "ec=10")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed4_list | grep "ec=11")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed4_list | grep "ec=12")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed4_list | grep "ec=13")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed4_list | grep "ec=99")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
#ec==5
ot=$(cat ./info/failed5_list | grep "ec=0")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed5_list | grep "ec=1")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed5_list | grep "ec=2")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed5_list | grep "ec=3")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed5_list | grep "ec=4")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed5_list | grep "ec=10")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed5_list | grep "ec=11")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed5_list | grep "ec=12")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed5_list | grep "ec=13")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed5_list | grep "ec=99")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
#ec==10
ot=$(cat ./info/failed10_list | grep "ec=0")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed10_list | grep "ec=1")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed10_list | grep "ec=2")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed10_list | grep "ec=3")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed10_list | grep "ec=4")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed10_list | grep "ec=5")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed10_list | grep "ec=11")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed10_list | grep "ec=12")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed10_list | grep "ec=13")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed10_list | grep "ec=99")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
#ec==11
ot=$(cat ./info/failed11_list | grep "ec=0")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed11_list | grep "ec=1")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed11_list | grep "ec=2")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed11_list | grep "ec=3")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed11_list | grep "ec=4")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed11_list | grep "ec=5")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed11_list | grep "ec=10")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed11_list | grep "ec=12")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed11_list | grep "ec=13")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed11_list | grep "ec=99")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
#ec==12
ot=$(cat ./info/failed12_list | grep "ec=0")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed12_list | grep "ec=1")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed12_list | grep "ec=2")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed12_list | grep "ec=3")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed12_list | grep "ec=4")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed12_list | grep "ec=5")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed12_list | grep "ec=10")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed12_list | grep "ec=11")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed12_list | grep "ec=13")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed12_list | grep "ec=99")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
#ec==13
ot=$(cat ./info/failed13_list | grep "ec=0")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed13_list | grep "ec=1")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed13_list | grep "ec=2")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed13_list | grep "ec=3")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed13_list | grep "ec=4")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed13_list | grep "ec=5")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed13_list | grep "ec=10")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed13_list | grep "ec=11")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed13_list | grep "ec=12")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed13_list | grep "ec=99")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
#ec==99
ot=$(cat ./info/failed99_list | grep "ec=0")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed99_list | grep "ec=1")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed99_list | grep "ec=2")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed99_list | grep "ec=3")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed99_list | grep "ec=4")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed99_list | grep "ec=5")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed99_list | grep "ec=10")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed99_list | grep "ec=11")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed99_list | grep "ec=12")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed99_list | grep "ec=13")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done
ot=$(cat ./info/failed99_list | grep "ec=99")
	for g in $ot; do
		z=$(($z+1))
		echo -en "\033[1;31m$g\033[0m"$'\n'
			if [ $s -ne 0  ] ; then
				cat ./out/$g
			fi
	done

echo -en "\033[1;31m$z/$y nesouhlasi\033[0m"$'\n'

