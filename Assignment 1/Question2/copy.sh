i=1
start=`date +%s`
while [ $i -le $2 ]
do
	cp ./$1 ./files/$1$i
	(( i++ ))
done
end=`date +%s`
echo `expr $end - $start`
