#include "Dolares.h"
#include "auxiliares.h"
#include <fstream>
#include <vector>
#include <iomanip>
#include <iostream>

//Obtengo los precios de los dolares: oficial, MEP y CCL
void Dolares::obtenerPrecioDolares(const std::string &archivo)
{
	std::fstream fs;
	fs.open (archivo.c_str(), std::fstream::in);
	std::string s;
	int i(0);
	while(getline(fs, s))
	{
		i = s.find(tagDolarOficial);
		if(i>0)
			break;
	}
	i += tagDolarOficial.size();


	size_t j = s.find("\">");
	std::string str(s.substr(i, j - i));

	dolarOficial = Auxiliares::StringToNumber<double>(str);

	i = j;
	i = s.find(tagDolarMEP, i);
	i = i + tagDolarMEP.size();
	j = s.find("\">", i);
	str = s.substr(i, j - i);
	dolarMEP = Auxiliares::StringToNumber<double>(str);

	i = j;
	i = s.find(tagDolarCCL, i);
	i = i + tagDolarCCL.size();
	j = s.find("\">", i);
	str = s.substr(i, j - i);
	dolarCCL = Auxiliares::StringToNumber<double>(str);

	fs.close();
}

void Dolares::imprimir()
{

	std::streamsize p = std::cout.precision();
	std::cout << Auxiliares::obtenerFechaHora() << " |";
	std::cout << " Dolar Oficial: " << std::setprecision(4) << DolarOficial();
	std::cout << " | Dolar MEP: " << DolarMEP();
	std::cout << " | CCL: " << DolarCCL() << std::setprecision(p) << std::endl;
}