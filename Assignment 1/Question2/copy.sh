i=1
while [ $i -le $2 ]
do
	cp ./$1 ./files/$1$i
	(( i++ ))
done