#include "midbc.h"
#include "milex_code.h"
#include "dbclex.h"

static int hex(const char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}

DbcLex::DbcLex(){
}

void DbcLex::SetFilename(const String&  s){
	f.Open(s);
	filename=s;
	line=1;
}

void DbcLex::GetLine(){
	if (f.IsEof()){
	}else{
		ActualLine=f.GetAll(8192);
		SetPtr(~ActualLine, 1, line);
	}
}

void DbcLex::Parse(){
	GetLine();
	Token_en token=T_UNKNOWN;
	Begin(T_All); //initial state
	while (!(f.IsEof()||T_EOF==token||stop>2) )
	{
		Token_en token= MiLex::Parse();
		if (T_EOL==token) GetLine();
	}
}
