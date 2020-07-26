import mechanize
br = mechanize.Browser()
response=br.open("http://www.ravaonline.com/v2/empresas/mapa.php")
f = open('myfile','w')
response=br.open("http://www.ravaonline.com/v2/empresas/bonos.php")
f = open('myfile','w')
f.write(response.read())
f.close() 
