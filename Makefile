default:
	./s3g.out
	cp style/* out/style/
rebuild:
	g++ -std=c++17 -Wall main.cpp -o s3g.out
