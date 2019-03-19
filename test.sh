clear && g++ src/coder.cpp --std=c++14 -o coder # compile the code
./coder -t selected/$1.txt # directly test your coder on $1'th pic
./coder -e selected/$1.txt test.bin # encode $1.txt to generate test.bin
./coder -d test.bin test.txt # decode the file generated before
python display.py test.txt # display your result using script
# (may encounter dependancy missing, solve it follow the error info)
cp selected/$1.txt origin.txt # copy test element into wkd
cp selected/$1.JPEG origin.JPEG # copy test element into wkd