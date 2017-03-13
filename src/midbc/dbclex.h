#ifndef _midbc_dbclex_h_
#define _midbc_dbclex_h_


#define LEXFILE <midbc/dbclex.mlx>
#include "milex_header.h"
#include "milex.h"


class DbcLex: public MiLex{
	public:
	DbcLex();
	void SetFilename(const String& s);
	void GetLine();
	void Parse();
	FileIn f;
	String ActualLine;
};

#endif
