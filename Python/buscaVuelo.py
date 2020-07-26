from selenium import webdriver
from selenium.webdriver.common.keys import Keys
import time
import datetime
import sqlite3

browser = webdriver.Firefox()
#browser = webdriver.PhantomJS('C:\Python27\phantomjs-1.9.8-windows\phantomjs.exe')

db = sqlite3.connect('C:\Python27\Vuelos.sqlite')
db.execute("PRAGMA foreign_keys = ON")
cursorExt = db.cursor()

cursorExt.execute('''select distinct trayecto from vuelos''')
i = 1
j = 1

today = datetime.date.today()
today = today.strftime('%m/%d/%Y')
print today

for row in cursorExt:
    cursor = db.cursor()
    consulta = '''SELECT v.fecha, t.desde, t.hasta, v.precio, v.fecha_busqueda, v.vuelo_id FROM vuelos v INNER JOIN trayectos t ON v.trayecto = t.trayecto_id where trayecto = ''' + str(row[0]) + ''''''
    cursor.execute(consulta)
    
    for row in cursor:
        browser.get('http://matrix.itasoftware.com/search.htm')
        assert 'Matrix' in browser.title

        if j == 1:
            xpath = "//*[@id='advancedfrom" + str(i) + "']"
        else:
            xpath = "//*[@id='advanced_from" + str(j) + "']"

        print xpath
        #elem = browser.find_element_by_xpath("//*[@id='advancedfrom1']")
        elem = browser.find_element_by_xpath(xpath)
        elem.send_keys(str(row[1]))
        
        if j == 1:
            xpath = "//*[@id='advancedto" + str(i) + "']"
        else:
            xpath = "//*[@id='advanced_to" + str(j) + "']"
        print xpath
        print "desde " + str(row[1])
            
        #elem = browser.find_element_by_xpath('//*[@id="advancedto1"]')
        elem = browser.find_element_by_xpath(xpath)
        elem.send_keys(str(row[2]))
        print "hasta " + str(row[2])

        elem = browser.find_element_by_xpath("//*[@id='ita_layout_TabContainer_0_tablist_ita_form_SliceForm_1']/span[1]")
        elem.click()

        xpath = "//*[@id='ita_form_date_DateTextBox_" + str(i) + "']"
        print xpath
        print "ingreso fecha " + str(row[0])
        #elem = browser.find_element_by_xpath("//*[@id='ita_form_date_DateTextBox_1']")
        elem = browser.find_element_by_xpath(xpath)
        elem.clear()
        elem.send_keys(row[0])
        elem = browser.find_element_by_xpath('//*[@id="advanced_searchSubmitButton_label"]')
        elem.click()
        
        print "espero 20 segundos"
        time.sleep(20)

        elem = browser.find_element_by_xpath('//*[@id="ita_form_button_LinkButton_0_label"]/span')
        
        precio = elem.text.encode('ascii', 'ignore').decode('ascii')
        
        print "voy a updatear con precio " + str(precio) + " fecha_busqueda " + str(today) + " para el vuelo_id " + str(row[5])
        cursorUpdate = db.cursor()
        cursorUpdate.execute('''UPDATE vuelos SET precio = ?, fecha_busqueda = ? WHERE vuelo_id = ? ''', (precio, today, row[5]))
        
        cursorHistorico = db.cursor()
        cursorHistorico.execute('''SELECT vuelo, fecha, precio FROM historico WHERE vuelo = ? AND fecha = ?''', (row[5], today))
        fila = cursorHistorico.fetchone()
        
        if (fila == None or fila[2] != float(precio)):  
            print "inserto precio en historico"    
            cursorUpdate.execute('''INSERT INTO historico(vuelo, fecha, precio) VALUES (?, ?, ?)''', (row[5], today, precio))
        else:
            print "ya habia valor en historico para hoy"
        #f = open('c:/python27/vuelos.txt',"a")
        #s = row[0] + " " + elem.text.encode('ascii', 'ignore').decode('ascii') + '\n'
        #print s
        #f.write(s)
        #f.close()
        #print "escribi archivo"
        
        print "espero 10 segundos"
        time.sleep(20)
        
        i = 0
        j = 2
        
        xpath = "//*[@id=\"ita_common_HoverWidget_" + str(i) + "\"]/div/div[2]"
        print xpath
        
        elem = browser.find_element_by_xpath(xpath)
        elem.click()
        print "vuelvo a la pagina de busqueda"
        
        print "espero 10 segundos"
        time.sleep(10)

db.commit()
browser.quit()
print "proceso terminado OK"

#SELECT t.desde, t.hasta, v.fecha, v.precio FROM vuelos v INNER JOIN trayectos t ON v.trayecto = t.trayecto_id order by v.precio asc