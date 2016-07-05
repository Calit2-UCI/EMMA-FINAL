/*
 * NaradaException.h
 *
 *  Created on: 09/01/2012
 *      Author: narada
 */

#ifndef NARADAEXCEPTION_H_
#define NARADAEXCEPTION_H_


namespace VisageSDK
{

class NaradaException : public std::exception
	{
	   std::string message;

		public:

	   NaradaException(std::string msg) : message(msg) {}

	   ~NaradaException() throw(){}

	   const char* what() const throw() { return message.c_str(); }

	   void debug(){std::cerr << "exception caught: " << what() << std::endl;}

	};

}

#endif /* NARADAEXCEPTION_H_ */
