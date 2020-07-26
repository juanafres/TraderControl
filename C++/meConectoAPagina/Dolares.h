#ifndef DOLARES_H
#define DOLARES_H

#include <string>

class Dolares
{
public:
	Dolares() : dolarOficial(0), dolarMEP(0), dolarCCL(0), tagDolarOficial("dolar_dl\" value=\""), 
				tagDolarMEP("dolar_arg\" value=\""), tagDolarCCL("dolar_ny\" value=\"")
	{
	}

	void obtenerPrecioDolares(const std::string &archivo);

	double DolarOficial() const
	{
		return dolarOficial;
	}

	double DolarMEP() const
	{
		return dolarMEP;
	}

	double DolarCCL() const
	{
		return dolarCCL;
	}

	void imprimir();

private:
	double dolarOficial;
	double dolarMEP;
	double dolarCCL;

	const std::string tagDolarOficial;
	const std::string tagDolarMEP;
	const std::string tagDolarCCL;
};

#endif