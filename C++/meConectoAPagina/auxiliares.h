#ifndef AUXILIARES_H
#define AUXILIARES_H

#include <string>
#include <afxwin.h>
#include <sstream>

class Auxiliares
{
public:
	template <typename T>
	static T StringToNumber ( const std::string &Text )
	{
		std::stringstream ss(Text);
		T result;
		return ss >> result ? result : 0;
	}

	//1er parametro: mascara de formato de fecha, 2do parametro: cantidad de dias anteriores de la fecha, ejemplo para ayer: 1
	static std::string obtenerDia(const std::string &, const int nroDiasAnterior = 0);

	static std::string obtenerFechaHora();

	static BOOL executeCommandLine(CString cmdLine, DWORD & exitCode);
};

#endif