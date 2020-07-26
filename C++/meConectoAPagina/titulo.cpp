#include "titulo.h"
#include "auxiliares.h"
#include "Cartera.h"
#include <iostream>
#include <fstream>
#include <iomanip> 
#include <sstream>
#include <deque>
#include <locale>
#include <algorithm> 
#include <regex>

//std::vector<double> Titulo::vecPrecioAnterior;
bool Titulo::esPrimeraCorrida = true;
static unsigned int contPasadas = 0;

Titulo::Titulo(const std::string tick, std::vector<double> limit) : ticker(tick), precioInicial(0.0),
		variacion(0.0), precio(0.0), vecLimites(limit), limite(0.0),
		numeroTendencia(3), tieneAdr(false), precioAdr(0.0), variacionAdr(0.0), precioTeorico(0.0), TIR(0.0), esBono(false),
		tagPrecioInicial("")
{
	if (ticker == "RO15" || ticker == "BDED" || ticker == "AA17" || ticker == "AY24" || ticker == "AN18" || 
		ticker == "DICA" || ticker == "DICY" || ticker == "GJ17" || ticker == "PARY" || ticker == "PARA" || 
		ticker == "BPMD" || ticker == "DIA0" || ticker == "BP15" || ticker == "BP18" || ticker == "DIY0" || 
		ticker == "BP21" || ticker == "PAA0" || ticker == "BPLD" || ticker == "BP28" || ticker == "PAY0")
	{
		esBono = true;
	}

	if(Titulo::esPrimeraCorrida && contPasadas == 0)
		inicializarMapAdr();

	if(TieneAdr())
		tieneAdr = true;

	//Inicializo el mapa de extremos con minimo = 0.0 y maximo = 0.0
	if(Titulo::esPrimeraCorrida && contPasadas == 0)
	{
		std::pair<double, double> elpar(0.0, 0.0);
		MapExtremos().insert(std::pair<const std::string, std::pair<double, double> >(ticker, elpar));
	}
}

void Titulo::inicializarMapAdr()
{
	std::pair<std::string, unsigned int> elpar("BFR", 3);
	MapAdr().insert(std::pair<const std::string, std::pair<const std::string, const unsigned int> >("FRAN", elpar));

	elpar = std::make_pair("BMA", 10);
	MapAdr().insert(std::pair<const std::string, std::pair<const std::string, const unsigned int> >("BMA", elpar));

	elpar = std::make_pair("EDN", 20);
	MapAdr().insert(std::pair<const std::string, std::pair<const std::string, const unsigned int> >("EDN", elpar));

	elpar = std::make_pair("GGAL", 10);
	MapAdr().insert(std::pair<const std::string, std::pair<const std::string, const unsigned int> >("GGAL", elpar));

	elpar = std::make_pair("PAM", 25);
	MapAdr().insert(std::pair<const std::string, std::pair<const std::string, const unsigned int> >("PAMP", elpar));

	elpar = std::make_pair("PZE", 10);
	MapAdr().insert(std::pair<const std::string, std::pair<const std::string, const unsigned int> >("PESA", elpar));

	elpar = std::make_pair("PBR", 2);
	MapAdr().insert(std::pair<const std::string, std::pair<const std::string, const unsigned int> >("APBR", elpar));

	elpar = std::make_pair("TEO", 5);
	MapAdr().insert(std::pair<const std::string, std::pair<const std::string, const unsigned int> >("TECO2", elpar));

	elpar = std::make_pair("TS", 2);
	MapAdr().insert(std::pair<const std::string, std::pair<const std::string, const unsigned int> >("TS", elpar));

	elpar = std::make_pair("TGS", 5);
	MapAdr().insert(std::pair<const std::string, std::pair<const std::string, const unsigned int> >("TGSU2", elpar));

	elpar = std::make_pair("YPF", 1);
	MapAdr().insert(std::pair<const std::string, std::pair<const std::string, const unsigned int> >("YPFD", elpar));
}

bool Titulo::TieneAdr()
{
	if(esBono)
		return false;

	itMapAdrs it = MapAdr().find(ticker);

	if(it == MapAdr().end())
		return false;

	return true;
}

void Titulo::obtenerPrecioYVariacionAdr()
{
	if(!tieneAdr)
		return;

	DWORD ret_code;

	//obtengo los precios del adr llamando a python desde las pags
	//http://finance.yahoo.com/q?s= + ticker adr. Ejemplo: http://finance.yahoo.com/q?s=ypf 
	//lo grabo a un archivo llamado adr.txt que se va sobreescribiendo

	std::string cmdline("python/buscarAdr ");

	itMapAdrs it = MapAdr().find(ticker);
	std::string tickerAdr(it->second.first);
	std::string tickerAdrLowerCase;
	std::locale loc;

	for(std::string::const_iterator itStr = tickerAdr.begin(); itStr != tickerAdr.end(); ++itStr)
		tickerAdrLowerCase.push_back(std::tolower(*itStr, loc));

	cmdline += tickerAdr;
	Auxiliares::executeCommandLine(cmdline.c_str(), ret_code);

	//Ya baje el archivo. Ahora busco la linea con el precio
	std::string tagPrecio("<span id=\"yfs_l84_");
	tagPrecio += tickerAdrLowerCase;
	tagPrecio += "\">";

	std::fstream fs;
	fs.open ("adr.txt", std::fstream::in);
	std::string s;
	int i(0);
	while(getline(fs, s))
	{
		i = s.find(tagPrecio);
		if(i>0)
			break;
	}
	
	fs.close();

	i += tagPrecio.size();

	size_t j = s.find("</", i);
	std::string str(s.substr(i, j - i));

	precioAdr = Auxiliares::StringToNumber<double>(str);

	//Ahora, en la misma linea s, obtengo el tag que me marca si esta en alza o en baja
	const std::string tagMovimiento("alt=\"");

	std::string movimiento;
	i = s.find(tagMovimiento);

	//si no encuentro movimiento, o sea quedo con movimiento neutro, movimiento queda vacio
	if(i != std::string::npos)
	{
		i += tagMovimiento.size();
		j = s.find("\">", i);
		movimiento = s.substr(i, j - i);
	}

	//Luego obtengo el porcentaje de variacion teniendo en cuenta si el movimiento es al alza o a la baja
	std::string tagVariacion("<span id=\"yfs_p43_");
	tagVariacion += tickerAdrLowerCase;
	tagVariacion += "\">(";

	i = s.find(tagVariacion);
	i += tagVariacion.size();
	j = s.find("%", i);
	str = s.substr(i, j - i);

	//si el movimiento es a la baja le agrego un menos adelante de la variacion
	if(movimiento == "Down")
		str = '-' + str;

	variacionAdr = Auxiliares::StringToNumber<double>(str);
}

//la formula es: Precio Local = ( Precio del ADR / Factor de Conversión ) * (Precio Dólar Cable)
void Titulo::calcularValorTeorico(double dolarCCL)
{
	if(!tieneAdr)
		return;

	unsigned int FactorConversion(MapAdr().find(ticker)->second.second);

	precioTeorico = precioAdr / FactorConversion * dolarCCL;
}

void Titulo::actualizarTitulo()
{
	std::string nombreArchivoInicial("initfile_");
	nombreArchivoInicial += Auxiliares::obtenerDia("%d-%m-%Y");

	maximo = precioInicial = PrecioInicial(nombreArchivoInicial);

	++contPasadas;

	if(Titulo::esPrimeraCorrida)
	{
		Titulo::VecPrecioAnterior().push_back(precioInicial);

		if(contPasadas == Cartera::CantidadEnCartera())
			Titulo::esPrimeraCorrida = false;
	}
}

void Titulo::actualizarPrecio(double var)
{
	variacion = var;
	precio = precioInicial + ((var / 100.0) * precioInicial);
	
	//Actualizo el mínimo
	if(MapExtremos()[ticker].first == 0.0)
		//si la variacion es negativa ese es el precio menor
		if(var < 0.0)
			MapExtremos()[ticker].first = precio;
		else
			//sino pongo el precio inicial
			MapExtremos()[ticker].first = precioInicial;
	else if (MapExtremos()[ticker].first > precio)
		MapExtremos()[ticker].first = precio;

	//Actualizo el máximo
	if(MapExtremos()[ticker].second == 0.0)
		if(var > 0.0)
			//si la variacion es positiva ese es el precio mayor
			MapExtremos()[ticker].second = precio;
		else
			//sino pongo el precio inicial
			MapExtremos()[ticker].second = precioInicial;
	else if (MapExtremos()[ticker].second < precio)
		MapExtremos()[ticker].second = precio;

	if(Titulo::VecPrecioAnterior()[contPasadas - 1] > precio)
		tendencia = baja;
	else if(Titulo::VecPrecioAnterior()[contPasadas - 1] < precio)
		tendencia = alza;
	else
		tendencia = neutra;

	MapTendencias()[contPasadas - 1].push_front(tendencia);
	Titulo::VecPrecioAnterior()[contPasadas - 1] = precio;

	if(MapTendencias()[contPasadas - 1].size() >= numeroTendencia)
		MapTendencias()[contPasadas - 1].resize(numeroTendencia);
}

double Titulo::Variacion(const std::string &ticker, const std::string &nombreArchivoVariacion)
{
	//Busco el ticker y saco el precio
	std::fstream fs;
	fs.open (nombreArchivoVariacion.c_str(), std::fstream::in);
	std::string s;
	while(getline(fs, s))
	{
		std::string aBuscar("class=\"td");
		aBuscar += ticker;
		aBuscar += "\"";
		int i = s.find(aBuscar);
		if(i>0)
			break;
	}
	size_t i = s.find(">+");
	if(i < 0 || i >= s.size())
		i = s.find(">-");
	std::string str(s.substr(++i));
	std::string ret;
	for(std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if(*it != '%')
		{
			ret.push_back(*it);
		}
		else
			break;
	}

	//unTitulo.get()->actualizarTitulo(558.0, StringToNumber<double>(ret));
	/*std::string::const_iterator it = ret.begin();
	if(*it == '+')*/
	fs.close();

	return Auxiliares::StringToNumber<double>(ret);
}

double Titulo::PrecioInicial(const std::string &nombreArchivoInicial)
{
	std::fstream fs;
	fs.open (nombreArchivoInicial.c_str(), std::fstream::in);
	std::string s;
	int i = 0;
	bool encontro1ro(false);
	bool encontro2do(false);
	while(getline(fs, s))
	{
		if(!encontro1ro)
		{
			//Primero busco el ticker
			std::string aBuscar(ticker);
			i = s.find(aBuscar);
		
			if(i>=0)
				encontro1ro = true;
		}
		else
		{
			tagPrecioInicial = Auxiliares::obtenerDia("%Y-%m-%d", 1);

			i = s.find(tagPrecioInicial);
			if(i>=0)
			{
				break;
			}
			
			//si no encontre el tag con la fecha de ayer, busco la de los dias anteriores
			for(int r = 0; r < 3; ++r)
			{
				tagPrecioInicial = Auxiliares::obtenerDia("%Y-%m-%d", r + 2);
				i = s.find(tagPrecioInicial);
				if(i>=0)
				{
					encontro2do = true;
					break;
				}
			}
			if(encontro2do)
				break;
		}
		/*std::string aBuscar("\">");
		aBuscar += ticker;
		aBuscar += "</";
		i = s.find(aBuscar);
		if(i>0)
			break;*/
	}

	fs.close();
	//size_t i = s.find(">+");
	//size_t ii = 0;

	// define a regular expression
	const std::tr1::regex pattern("\"\\d*-\\d*-\\d*\"\\,\"\\d*\\.?\\d*\"\\,\"\\d*\\.?\\d*\"\\,\"\\d*\\.?\\d*\"\\,\"(\\d*\\.?\\d*)\"\\,\"\\d*\"\\,\"0\"");
	//"2015-01-02","1134.50000","1173","1115","1117.00000","4351513","0"
	//"2015-01-02","42.00000","42.6","40.9","41.30000","813066","0"

	std::tr1::cmatch res;
	std::tr1::regex_search(s.c_str(), res, pattern);
	std::string precioAnt(res[1]);

	/*size_t posicionTagPrecioInicial(tagPrecioInicial.size());
	i = s.find(",", posicionTagPrecioInicial);
	size_t tamanioValor(i - posicionTagPrecioInicial);

	std::string ret(s.substr(posicionTagPrecioInicial, tamanioValor));*/

	//for(size_t cont = 0; cont < 2; ++cont)
	//{
	//	i = s.find("\">", ++i);
	//}
	//
	//i += 2;
	//	
	//std::string str(s.substr(i, 7));
	//std::string ret;
	//for(std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	//{
	//	if(*it != '<')
	//	{
	//		if(*it==',')
	//			ret.push_back('.');
	//		else
	//			ret.push_back(*it);
	//	}
	//	else
	//		break;
	//}

	////unTitulo.get()->actualizarTitulo(558.0, StringToNumber<double>(ret));
	///*std::string::const_iterator it = ret.begin();
	//if(*it == '+')*/
	

	return Auxiliares::StringToNumber<double>(precioAnt);
}

void Titulo::imprimir()
{
	//for(std::vector<double>::const_iterator itLimites = VecLimites().begin(); itLimites != VecLimites().end(); ++itLimites)
	//if(pasoLimite(*itLimites))
	//{
	//	std::ostringstream s;
	//	s << "Ticker " << ticker << " llego al limite. Precio actual: " << precio << ". Variacion: " << variacion << ".";
	//	const std::string mensaje = s.str();

	//	AfxMessageBox(_T(mensaje.c_str()));

	//	actualizarLimite(*itLimites);
	//}

	std::streamsize p = std::cout.precision();
	//std::cout << Auxiliares::obtenerFechaHora() << " " << ticker << "\n";
	std::cout << ticker << "\n";
	std::cout << "	Precio actual: " << std::setprecision(6) << precio << "\n";
	std::cout << "	Precio minimo: " << MapExtremos()[ticker].first << " " << "maximo: " << MapExtremos()[ticker].second << "\n";
	std::cout << "	Variacion en el dia: " << std::setprecision(4) << variacion << std::setprecision(p) << "\n";
	std::cout << "	Tendencia actual: " << Tendencia() << "\n";
	if(limite)
		std::cout << "	Limite actual: " << limite << "\n";
	if(esBono)
		std::cout << "	TIR: " << TIR << "\n";
	if(tieneAdr)
	{
		std::string tendenciaAdr;
		std::string tickerAdr(MapAdr()[ticker].first);
		double precioAnteriorAdr(PrecioAnteriorAdr()[tickerAdr]);

		if(precioAnteriorAdr != 0.0)
		{
			if(precioAdr > precioAnteriorAdr)
				tendenciaAdr = "+";
			else if(precioAdr < precioAnteriorAdr)
				tendenciaAdr = "-";
		}

		std::cout << "	Precio Adr: " << std::setprecision(5) << precioAdr << tendenciaAdr << "\t\tVariacion Adr: " << variacionAdr << "\n";
		std::cout << "	Precio Teorico: " << precioTeorico;
		std::cout << "\t\tCCL: " << (precio / precioAdr) * MapAdr()[ticker].second << std::setprecision(p) << "\n";

		PrecioAnteriorAdr()[tickerAdr] = precioAdr;
	}
	if(contPasadas == Cartera::CantidadEnCartera())
	{
		std::cout << "------------------------------------------------------------------------" << std::endl;
		contPasadas = 0;
	}
}

void Titulo::VecLimites(std::vector<double> limit)
{
	vecLimites = limit;
}

std::vector<double>& Titulo::VecLimites()
{
	return vecLimites;
}

bool Titulo::pasoLimite(const double limit)
{
	if(limit > 0.0)
	{
		if(variacion >= limit)
		{
			//limite = limit;
			return true;
		}
	}
	else if(limit < 0.0)
	{
		if(variacion <= limit)
		{
			//limite = limit;
			return true;
		}
	}

	//limite = 0.0;
	return false;
}

std::string Titulo::Tendencia() const
{
	if(tendencia == baja)
		return std::string("baja");
	else if(tendencia == neutra)
		return std::string("neutra");
	else
		return std::string("alza");
}

void Titulo::alertarActualizarLimite()
{
	for(std::vector<double>::iterator itLimites = VecLimites().begin(); itLimites != VecLimites().end(); ++itLimites)
	{
		if(pasoLimite(*itLimites))
		{
			std::ostringstream s;
			s << "Ticker " << ticker << " llego al limite. Precio actual: " << precio << ". Variacion: " << variacion << ".";
			const std::string mensaje = s.str();

			AfxMessageBox(_T(mensaje.c_str()));

			if((*itLimites) < 0.0)
				//(*itLimites) = this->limite = --(*itLimites);
				(*itLimites) = this->limite = (--precio)++;
				//this->limite = --(*itLimites);
			else if(*itLimites > 0.0)
				//(*itLimites) = this->limite = ++(*itLimites);
				(*itLimites) = this->limite = (++precio)--;
				//this->limite = ++(*itLimites);
		}
	}
}


void Titulo::analizarTendencia()
{
	std::deque<Titulo::tTendencia> &deqTend = MapTendencias()[contPasadas - 1];

	if(deqTend.size() < numeroTendencia)
		return;

	tTendencia ultimaTendencia = deqTend[0];

	//PRUEBA tambien en el for en vez de 9 va N//
	//if(ultimaTendencia != neutra)
	if(ultimaTendencia != neutra)
	{
		//for(unsigned int i = 1; i <= N; ++i)
		for(unsigned int i = 1; i < numeroTendencia; ++i)
		{
			tTendencia anteriorTendencia = deqTend[i];
			if(anteriorTendencia != ultimaTendencia)
			{
				goto NoAltaTendencia;
			}
		}
		//solo es alta tendencia cuando tengo todo el deque (los N elementos) con la misma tendencia (no neutra)
		AltaTendencia() = true;
		return;
	}

	NoAltaTendencia:
	AltaTendencia() = false;
	return;
}

void Titulo::alertarAltaTendencia()
{
	if(AltaTendencia() && !YaInformeTendencia())
	{
		std::ostringstream s;
		if(tendencia == alza)
			s << "Ticker " << ticker << " esta con fuerte tendencia al " << Tendencia() << ".";
		else
			s << "Ticker " << ticker << " esta con fuerte tendencia a la " << Tendencia() << ".";

		const std::string mensaje = s.str();

		AfxMessageBox(_T(mensaje.c_str()));

		YaInformeTendencia() = true;
	}
}

void Titulo::obtenerTIR(const std::string &nombreArchivoVariacion)
{
	std::string tagTIRTicker(ticker);
	tagTIRTicker += "\">";
	tagTIRTicker += ticker;

	std::fstream fs;
	fs.open (nombreArchivoVariacion, std::fstream::in);
	std::string s;
	int i(0);
	while(getline(fs, s))
	{
		i = s.find(tagTIRTicker);
		if(i>0)
			break;
	}

	//desde la linea que encontre, es la cuarta
	for(int i = 0; i < 4; ++i)
		getline(fs, s);

	fs.close();

	//dentro de esta nueva linea busco el comienzo del tag con la TIR
	const std::string tagTIR("\">");

	i = s.find(tagTIR);
	//le sumo el tamaño del tag para quedar posicionado justo frente al valor
	i += tagTIR.size();

	//hasta el porcentaje
	size_t j = s.find("%", i);
	std::string str(s.substr(i, j - i));

	//le cambio la coma por un punto
	std::replace(str.begin(), str.end(), ',', '.');

	TIR = Auxiliares::StringToNumber<double>(str);
}