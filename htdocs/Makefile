#########################################################
#### makefile to process Morgana's homepage
#########################################################

all:
	@echo "Specify either clean, upload, update or checkout."

CVS_RSH=ssh
CVSROOT=pandur@cvs.morgana.sourceforge.net:/cvsroot/morgana
export

.PHONY: all clean upload update co

clean:
	rm -rf `find . -name '*.bak'` `find . -name '*~'`

update:
	cvs -z3 update

checkout: co

co:
	cd ..; cvs co htdocs

upload: update
	cvs -z3 commit -m ""
	ssh -A -t -l pandur shell1.sourceforge.net \
	 /home/users/pandur/homepage.sh
