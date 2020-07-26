import mechanize
import datetime
import os.path

def main():
	today = datetime.date.today()
	s = 'initfile_' + today.strftime('%d-%m-%Y')

	if (os.path.isfile(s)):
		return

	f = open(s,'w')
	print "lo abri"
	br = mechanize.Browser()
	print "lo mechanize"
	response=br.open("http://www.merval.sba.com.ar/vistas/cotizaciones/acciones.aspx")
	print "lo openee"
	f.write(response.read())
	print "escribi archivo"
	response=br.open("http://www.merval.sba.com.ar/Vistas/Cotizaciones/TitulosPublicos.aspx")
	print "lo openee"
	f.write(response.read())
	print "escribi archivo"
	f.close()
	print "cerre archivo"
	
if __name__ == '__main__':
    main()
