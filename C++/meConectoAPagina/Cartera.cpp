#include "Cartera.h"

int Cartera::cantidadEnCartera;

Cartera::Cartera()
{
	std::vector<double> limites;
	limites.push_back(-0.01);
	limites.push_back(-13.0);

	mapTicker.insert(std::pair<std::string, std::vector<double> >("YPFD", limites));
	mapTicker.insert(std::pair<std::string, std::vector<double> >("ERAR", limites));
	mapTicker.insert(std::pair<std::string, std::vector<double> >("TECO2", limites));
	mapTicker.insert(std::pair<std::string, std::vector<double> >("APBR", limites));
	mapTicker.insert(std::pair<std::string, std::vector<double> >("RO15", limites));
	//mapTicker.insert(std::pair<std::string, std::vector<double> >("BDED", limites));
	mapTicker.insert(std::pair<std::string, std::vector<double> >("TS", limites));
	//mapTicker.insert(std::pair<std::string, std::vector<double> >("AY24", limites));
	mapTicker.insert(std::pair<std::string, std::vector<double> >("AA17", limites));

	CantidadEnCartera(mapTicker.size());
}

void Cartera::CantidadEnCartera(int cant)
{
	Cartera::cantidadEnCartera = cant;
}

int Cartera::CantidadEnCartera()
{
	return cantidadEnCartera;
}

std::map<std::string, std::vector<double> >& Cartera::MapTicker()
{
	return mapTicker;
}

void Cartera::actualizarLimiteCartera(const std::string &ticker, const std::vector<double> &limites)
{
	mapTicker[ticker] = limites;
}