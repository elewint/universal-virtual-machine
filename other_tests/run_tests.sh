#! /bin/sh

rm *.dump
# rm *.1
# rm *.0
# rm *.um
rm *.gt
rm *.out
rm *.results

make
./test

echo -e

testFiles=$(ls *.um)

for testFile in $testFiles ; do
    testName=$(echo $testFile | sed -E 's/(.*).um/\1/')
    echo "Now testing $testName..."

    if [ -f $testName.0 ] ; then
        echo "   Input found: yes"
    else
        echo "   (No input found)"
    fi

    if [ -f $testName.1 ] ; then
        echo "   Output found: yes"
        um $testFile > $testName.gt
        ./um $testFile > $testName.out
        diff $testName.gt $testName.out > $testName.results
        if [ -s $testName.results ] ; then
            echo "   ---> Diff FAILED :("
        else
            echo "   ---> Diff success!"
            rm $testName.gt
            rm $testName.out
            rm $testName.results
        fi
        
    else
        echo "   (No output found)"
    fi

    # umdump $testFile > $testName.dump
done