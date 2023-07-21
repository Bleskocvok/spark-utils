#!/bin/sh


# set -x

make || exit 1

perform_test()
{
    prefix="$1"
    shift 1
    min="$1"
    max="$2"
    count="$3"
    shift 3
    args="$@"

    echo "test: $min $max $count $args"
    echo "> testing/info_$prefix.txt"
    printf "\n"

    ./getrand $args $min $max $count \
        | pv | ./evaluate \
        2> /dev/null \
        | tee "testing/info_$prefix.txt"

    printf "\n"
}


printf "\n"

mkdir -p testing
# perform_test bad  0 99999 40000000000 --bad
# perform_test good 0 99999 40000000000


# perform_test bad  0 99999 2000000000 --bad
# perform_test good 0 99999 2000000000


# perform_test bad  0 99999 20000000 --bad
# perform_test good 0 99999 20000000


# perform_test bad  0 671088640 40000000000 --bad
# perform_test good 0 671088640 40000000000


perform_test bad  0 671088640 200000000 --bad
perform_test good 0 671088640 200000000



# ./getrand 0 99 100000000 | ./evaluate
