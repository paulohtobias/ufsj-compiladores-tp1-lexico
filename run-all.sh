SRCS=$(find src/ -type f -name "*.c")
HEADERS=$(find include/ -type f -name "*.h")

run()
{
	echo ./pcc $2 $3 $4 $1
	./pcc $2 $3 $4 $1
}

run main.c $1 $2 $3
for SRC in $SRCS; do
	run $SRC $1 $2 $3
done

for HEADER in $HEADERS; do
	run $HEADER $1 $2 $3
done
