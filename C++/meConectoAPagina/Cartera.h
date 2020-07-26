#ifndef CARTERA_H
#define CARTERA_H

#include <vector>
#include <string>
#include <map>
#include "titulo.h"

class Cartera
{
public:
	Cartera();
	std::map<std::string, std::vector<double> >& MapTicker();
	void CantidadEnCartera(int cant);
	static int CantidadEnCartera();
	void actualizarLimiteCartera(const std::string &, const std::vector<double> &);
private:
	std::vector<Titulo> vecTitulos;
	//std::vector<std::string> vecTicker;
	std::map<std::string, std::vector<double> > mapTicker;
	static int cantidadEnCartera;
};

#endif