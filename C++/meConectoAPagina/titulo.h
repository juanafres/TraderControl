#ifndef TITULO_H
#define TITULO_H

#include <string>
#include <vector>
#include <map>
#include <deque>

class Titulo
{
public:

	typedef std::map<const std::string, std::pair<const std::string, const unsigned int> > mapAdrs;
	typedef std::map<const std::string, std::pair<const std::string, const unsigned int> >::const_iterator itMapAdrs;
	Titulo(const std::string, std::vector<double>);

	enum tTendencia { baja = -1, neutra, alza};

	void actualizarPrecio(double);
	void actualizarTitulo();
	double Variacion(const std::string &ticker, const std::string &nombreArchivoVariacion);
	double PrecioInicial(const std::string &nombreArchivoInicial);
	void VecLimites(std::vector<double> limit);
	std::vector<double> & VecLimites();
	bool pasoLimite(const double);
	void imprimir();
	std::string Tendencia() const;
	void alertarActualizarLimite();
	const std::string & Ticker() const
	{
		return ticker;
	}
	double Limite() const
	{
		return limite;
	}

	void analizarTendencia();

	void alertarAltaTendencia();

	void inicializarMapAdr();

	void obtenerPrecioYVariacionAdr();

	void calcularValorTeorico(double dolarCCL);

	void obtenerTIR(const std::string &nombreArchivoVariacion);

	bool EsBono()
	{
		return esBono;
	}

	void calcularMediaMovil(int dias);

private:
	const std::string ticker;
	double precioInicial;
	double variacion;
	double precio;
	//double minimo;
	double maximo;
	std::vector<double> vecLimites;
	//static std::vector<double> vecPrecioAnterior;
	static bool esPrimeraCorrida;

	double limite;
	
	tTendencia tendencia;

	std::vector<double> & VecPrecioAnterior()
	{
		static std::vector<double> vecPrecioAnterior;
		return vecPrecioAnterior;
	}

	const unsigned int numeroTendencia;

	std::map<int, std::deque<tTendencia> > & MapTendencias()
	{
		static std::map<int, std::deque<tTendencia> > mapTendencias;
 		return mapTendencias;
	}

	bool & AltaTendencia()
	{
		static bool altaTendencia = false;
		return altaTendencia;
	}

	bool & YaInformeTendencia()
	{
		static bool yaInformeTendencia = false;
		return yaInformeTendencia;
	}

	//map conteniendo el ticker argentino, el ticker del adr y el factor de conversion
	mapAdrs & MapAdr()
	{
		static std::map<const std::string, std::pair<const std::string, const unsigned int> > mapAdr;
		return mapAdr;
	}

	bool tieneAdr;

	bool TieneAdr();

	double precioAdr;

	double variacionAdr;

	double precioTeorico;

	bool esBono;

	double TIR;

	//mapa con key = ticker y el primer valor del par = mínimo y el segundo = máximo
	std::map<const std::string, std::pair<double, double> > & MapExtremos()
	{
		static std::map<const std::string, std::pair<double, double> > mapExtremos;
		return mapExtremos;
	}

	std::map<const std::string, double> & PrecioAnteriorAdr()
	{
		static std::map<const std::string, double> precioAnteriorAdr;

		return precioAnteriorAdr;
	}

	std::string tagPrecioInicial;
};

#endif