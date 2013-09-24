testdirs="`find . | grep "source$" | sed "s/\/source$//g"`"
ecode=0

for dir in $testdirs; do
  echo ""
  echo "\033[1;34m========================== testuji $dir ==========================\033[0m"
  
  ./ifj12 "$dir/source" < "$dir/stdin" > stdout 2> stderr
  valgrind ./ifj12 "$dir/source" < "$dir/stdin" > /dev/null 2> val
  echo $? > code
  
  #kontrola navratoveho kodu
  diff code "$dir/code" >/dev/null
  if [ $? -eq 0 ];then
    echo "\033[1;32m[   return code   ] OK\033[0m"
  else
    ecode=1
    echo "\033[34m[   return code   ] Differ!\033[0m"
    echo -n "\033[34mExpected: \033[0m"; cat "$dir/code"
    echo -n "\033[34mReturned: \033[0m"; cat code
  fi
  
  #kontrola stdout
  diff stdout "$dir/stdout" >diff
  if [ $? -eq 0 ];then
    echo "\033[1;32m[     stdout      ] OK\033[0m"
  else
    ecode=1
    echo "\033[34m[     stdout      ] Differ!\033[0m"
    cat diff
  fi

  #kontrola stderr
  size1=`du -b "$dir/stderr" | sed "s/\t.*//g"`
  size2=`du -b "stderr" | sed "s/\t.*//g"`
  #pokud jsou oba soubory prazdne, tak rovnou zobrazime OK, jinak zobrazime oba
  #vystupy pro srovnani
  if [ "$size1" = "0" ] && [ "$size2" = "0" ]; then
    echo "\033[1;32m[      stderr     ] OK\033[0m"
  else
    echo -n "\033[1;33m[ stderr expected ] \033[0m"
    cat "$dir/stderr"
    echo -n "\033[1;33m[ stderr returned ] \033[0m"
    cat "stderr"
  fi
  
  valgrind="`cat val | grep "All heap blocks were freed -- no leaks are possible"`"
  if [ "$valgrind" = "" ];then
    ecode=1
    echo "\033[34m[    valgrind     ] Memory leak!\033[0m"
  else
    echo "\033[1;32m[    valgrind     ] OK\033[0m"
  fi
  
done

rm -f tests/code tests/stderr tests/stdout tests/val

exit $ecode
