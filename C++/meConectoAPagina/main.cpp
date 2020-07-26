#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include "auxiliares.h"
#include "Cartera.h"
#include "Dolares.h"

int main()
{
	DWORD ret_code;

	//obtengo los precios iniciales llamando a python desde las pags
	//http://www.merval.sba.com.ar/vistas/cotizaciones/acciones.aspx y 
	//http://www.merval.sba.com.ar/Vistas/Cotizaciones/TitulosPublicos.aspx y 
	//lo grabo a un archivo con fecha del dia. Si ya existe el archivo no lo busco devuelta.
	//Auxiliares::executeCommandLine("python/initWeb.exe", ret_code);
	Cartera laCartera;
	
	std::string args;

	for(std::map<std::string, std::vector<double> >::iterator it = laCartera.MapTicker().begin(); it != laCartera.MapTicker().end(); ++it)
	{
		args += " ";
		args += it->first;
	}

	std::string cmd("python/ObtenerPreciosIniciales.exe");
	cmd += args;

	Auxiliares::executeCommandLine(cmd.c_str(), ret_code);

	std::tr1::shared_ptr<Dolares> dolares(new Dolares);

	while(true)
	{
		for(std::map<std::string, std::vector<double> >::const_iterator itTicker = laCartera.MapTicker().begin(); itTicker != laCartera.MapTicker().end(); ++itTicker)
		{
			/*for(std::vector<double>::const_iterator itLimites = (*itTicker).second.begin(); itLimites != (*itTicker).second.end(); ++itLimites)
			{
*/
			try
			{
				std::tr1::shared_ptr<Titulo> unTitulo(new Titulo((*itTicker).first, (*itTicker).second));

				//obtengo y seteo el precio de cierre anterior
				unTitulo->actualizarTitulo();
	
				//executeCommandLine("c:/python27/dist/getWeb.exe", ret_code);
				if(itTicker == laCartera.MapTicker().begin())
					Auxiliares::executeCommandLine("python/getWeb.exe", ret_code);

				//obtengo el precio actual llamando a python desde http://www.ravaonline.com/v2/empresas/mapa.php
				unTitulo->actualizarPrecio(unTitulo->Variacion((*itTicker).first, "myfile"));

				//si la variacion actual del precio supera el limite para el ticker, aviso y le añado un porciento al limite
				unTitulo->alertarActualizarLimite();

				//actualizo el vector de limites de la cartera para el ticker
				laCartera.actualizarLimiteCartera(unTitulo.get()->Ticker(), unTitulo.get()->VecLimites());

				unTitulo->analizarTendencia();

				unTitulo->alertarAltaTendencia();

				if(itTicker == laCartera.MapTicker().begin())
				{
					dolares->obtenerPrecioDolares("myfile");
					dolares->imprimir();
				}

				if(unTitulo->EsBono())
					unTitulo->obtenerTIR("myfile");

				unTitulo->obtenerPrecioYVariacionAdr();

				unTitulo->calcularValorTeorico(dolares->DolarCCL());

				unTitulo->imprimir();
			}
			catch(...)
			{
				std::cout << "Me trago la excepcion" << std::endl;
			}
		}

		Sleep(60000);
	}

	return 0;
}