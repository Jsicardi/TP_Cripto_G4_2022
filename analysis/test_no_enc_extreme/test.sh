CWD=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
IN_FILE="$CWD/resources/back_to_the_future.txt"
BMP_FILE="$CWD/resources/black.bmp"
OUT_FOLDER="$CWD/results"
cd $CWD/../../
make
STEGOBMP=`pwd`/stegobmp
cd $CWD

if [ -f "$OUT_FOLDER/time_eLSB1.txt" ]; then
    rm $OUT_FOLDER/time_eLSB1.txt
fi

if [ -f "$OUT_FOLDER/time_eLSB4.txt" ]; then
    rm $OUT_FOLDER/time_eLSB4.txt
fi

if [ -f "$OUT_FOLDER/time_eLSBI.txt" ]; then
    rm $OUT_FOLDER/time_eLSBI.txt
fi

if [ -f "$OUT_FOLDER/time_xLSB1.txt" ]; then
    rm $OUT_FOLDER/time_xLSB1.txt
fi

if [ -f "$OUT_FOLDER/time_xLSB4.txt" ]; then
    rm $OUT_FOLDER/time_xLSB4.txt
fi

if [ -f "$OUT_FOLDER/time_xLSBI.txt" ]; then
    rm $OUT_FOLDER/time_xLSBI.txt
fi

for i in $(seq 1 1 1000)
do
    time ($STEGOBMP --embed --in $IN_FILE -p $BMP_FILE --out "$OUT_FOLDER/eLSB1.bmp" --steg LSB1 &> /dev/null) &>> $OUT_FOLDER/time_eLSB1.txt
    time ($STEGOBMP --embed --in $IN_FILE -p $BMP_FILE --out "$OUT_FOLDER/eLSB4.bmp" --steg LSB4 &> /dev/null) &>> $OUT_FOLDER/time_eLSB4.txt
    time ($STEGOBMP --embed --in $IN_FILE -p $BMP_FILE --out "$OUT_FOLDER/eLSBI.bmp" --steg LSBI &> /dev/null) &>> $OUT_FOLDER/time_eLSBI.txt
    time ($STEGOBMP --extract --out "$OUT_FOLDER/xLSB1" -p "$OUT_FOLDER/eLSB1.bmp" --steg LSB1 &> /dev/null) &>> $OUT_FOLDER/time_xLSB1.txt
    time ($STEGOBMP --extract --out "$OUT_FOLDER/xLSB4" -p "$OUT_FOLDER/eLSB4.bmp" --steg LSB4 &> /dev/null) &>> $OUT_FOLDER/time_xLSB4.txt
    time ($STEGOBMP --extract --out "$OUT_FOLDER/xLSBI" -p "$OUT_FOLDER/eLSBI.bmp" --steg LSBI &> /dev/null) &>> $OUT_FOLDER/time_xLSBI.txt
done