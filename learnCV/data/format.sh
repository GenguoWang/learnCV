files=`ls *.png *.PNG`
j=0
for f in $files
do
    echo "mv $f card$j.png"
    mv $f "card$j.png"
    j=$[j+1]
done
