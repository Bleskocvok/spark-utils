#!/bin/sh


# set -x

make || exit 1


# ./getrand 0 99 100000000 > dump.txt

# du -h dump.txt

# ./evaluate dump.txt


# mkdir -p dumps

# files=""
# for i in $(seq 20); do
#     f="dumps/dump_$i.txt"
#     ./getrand 0 99 100000000 > $f
#     files="$files $f"
# done

# cat $files | ./evaluate

# rm $files

perform_test()
{
    prefix="$1"
    shift 1
    args="$@"

    echo "test: $args"
    echo "> testing/info_$prefix.txt"
    printf "\n"

    ./getrand $args \
        | pv | ./evaluate \
        2> "testing/hist_$prefix.txt" \
        | tee "testing/info_$prefix.txt"

    printf "\n"
}


mkdir -p testing
perform_test bad --bad 0 99999 40000000000
perform_test good      0 99999 40000000000


# perform_test bad --bad 0 99999 2000000000
# perform_test good      0 99999 2000000000


# perform_test bad --bad 0 99999 20000000
# perform_test good      0 99999 20000000



# ./getrand 0 99 100000000 | ./evaluate
