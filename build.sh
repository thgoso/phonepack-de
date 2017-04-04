#!/bin/bash

x=$(locale | grep LANG=de_DE.UTF-8)
if [ -z $x ]; then
  echo -e "\033[1;37m\033[41mphonepack-de ist auf deutschen UTF-8 Sytemen getestet.\033[m"
  echo -e "\033[1;37m\033[41mDeine Spracheinstellung ist jedoch anders !\033[m"
  echo -e "\033[1;37m\033[41mTeste bitte ob es bei dir funktioniert.\033[m"
fi

echo
echo "Kompiliere..."
echo "string.c"
gcc -Wall -Wextra -O3 -c string.c -o string.o
echo "phonetics.c"
gcc -Wall -Wextra -O3 -c phonetics.c -o phonetics.o
echo "phonecode.c"
gcc -Wall -Wextra -O3 -c phonecode.c -o phonecode.o
echo "phonecomp.c"
gcc -Wall -Wextra -O3 -c phonecomp.c -o phonecomp.o
echo "phoneshow.c"
gcc -Wall -Wextra -O3 -c phoneshow.c -o phoneshow.o
echo "phoneshow_read.c"
gcc -Wall -Wextra -O3 -c phoneshow_read.c -o phoneshow_read.o
echo "phoneshow_write.c"
gcc -Wall -Wextra -O3 -c phoneshow_write.c -o phoneshow_write.o
echo "phoneshow_comp.c"
gcc -Wall -Wextra -O3 -c phoneshow_comp.c -o phoneshow_comp.o
echo "Linke..."
gcc -Wall -Wextra string.o phonetics.o phonecode.o -o phonecode-de
gcc -Wall -Wextra string.o phonetics.o phonecomp.o -o phonecomp-de
gcc -Wall -Wextra string.o phonetics.o phoneshow.o phoneshow_read.o phoneshow_write.o phoneshow_comp.o -o phoneshow-de
echo "Fertig"

rm -f string.o
rm -f phonetics.o
rm -f phonecode.o
rm -f phonecomp.o
rm -f phoneshow.o
rm -f phoneshow_read.o
rm -f phoneshow_write.o
rm -f phoneshow_comp.o

exit 0

