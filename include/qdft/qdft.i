%module libqdftgraph
%{
	/* Includes the header in the wrapper code */
#include "types.hh"
#include "qdft.hh"
	using namespace qdft;
%}

%include <windows.i>
%include "stl.i"
/* Parse the header file to generate wrappers */
%include "types.hh"
%include "qdft.hh"

%template(dataManagers) qdft::data_managers<>;
