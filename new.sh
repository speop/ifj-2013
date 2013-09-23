#! /bin/bash 
make clean
make
chmod -c 0777 main
echo -e "\nProgram: \n"
./main $1
echo -e "\e[00m \nNavratovy kod: $?"
