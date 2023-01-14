# requires pandoc
man:
	pandoc s3g.1.md -s -t man -o s3g.1

# requires sudo-er
install:
	cp s3g.1 /usr/local/man/man1
	sudo mandb
