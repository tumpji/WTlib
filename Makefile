

all:
	-@mkdir build &2>/dev/null
	cd build && cmake .. && make

delete:
	rm -R build

compress:
	zip xtumpa01.zip -r CMakeLists.txt demo lib LICENSE Makefile README.md thirdparty

run:
	echo "Program lze nalezt v build/demo/projbc "
	echo "Knihovnu je mozne nalezt v build/lib/libwt.a"
