import mechanize
import os.path
import sys, os, string

def main():
	f = open('adr.txt','w')
	print "lo abri"
	br = mechanize.Browser()
	print "lo mechanize"

	webpage = "http://finance.yahoo.com/q?s=" + str(sys.argv[1])
	print webpage
	response=br.open(webpage)
	print "lo openee"
	f.write(response.read())
	print "escribi archivo"
	f.close()
	print "cerre archivo"
	
if __name__ == '__main__':
    main()
