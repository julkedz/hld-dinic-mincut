NAME="ALT"

g++ "$NAME"_test_generator.cpp -o "$NAME"_test_generator
g++ "$NAME"_check_output.cpp -o "$NAME"_check_output
g++ -O3 -static -std=c++17 "$NAME".cpp -o "$NAME"
g++ -O3 -static -std=c++17 "$NAME"_brute.cpp -o "$NAME"_brute

while [ 1 ]
do

./"$NAME"_test_generator > "$NAME"_test.txt

time ./"$NAME" < "$NAME"_test.txt > "$NAME"_output.txt
time ./"$NAME"_brute < "$NAME"_test.txt > "$NAME"_output_brute.txt

head -n 1 "$NAME"_output.txt > "$NAME"_answer.txt
head -n 1 "$NAME"_output_brute.txt > "$NAME"_answer_brute.txt

cat "$NAME"_test.txt "$NAME"_output.txt > "$NAME"_checker_input.txt
./"$NAME"_check_output < "$NAME"_checker_input.txt > "$NAME"_check_outcome.txt
echo "OK" > ok.txt

if ! diff -q "$NAME"_answer.txt "$NAME"_answer_brute.txt > /dev/null
then
    echo "WRONG MINIMUM"
    exit 1
fi

if ! diff -q "$NAME"_check_outcome.txt ok.txt > /dev/null
then
    echo "INVALID OUTPUT"
    exit 1
fi

echo "OK"

done
