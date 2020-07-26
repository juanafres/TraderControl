import mechanize
import datetime
import os.path
import sys

def main():
    today = datetime.date.today()
    fn = 'initfile_' + today.strftime('%d-%m-%Y')
    
    if (os.path.isfile(fn)):
        return
    i = 0
    f = open(fn,'w')
    
    for arg in sys.argv:
        if(i != 0):
            s = 'http://www.ravaonline.com//v2/empresas/precioshistoricos.php?e=' + arg + '&csv=1'
            #s = 'http://www.ravaonline.com/v2/empresas/perfil.php?e=' + arg + '&x=11&y=1'
            print s
            br = mechanize.Browser()
            response=br.open(s)
            f.write(arg + ':\n')
            f.write(response.read())
			
        i = i + 1

    f.close()

if __name__ == '__main__':
    main()