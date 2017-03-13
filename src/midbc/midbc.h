#ifndef _midbc_midbc_h
#define _midbc_midbc_h


#include <CtrlLib/CtrlLib.h>
#include <SqlCtrl/SqlCtrl.h>
#include <plugin/sqlite3/Sqlite3.h>
using namespace Upp;

#define LAYOUTFILE <midbc/midbc.lay>
#include <CtrlCore/lay.h>

#define IMAGEFILE <midbc/midbc.iml>
#include <Draw/iml_header.h>

#define SCHEMADIALECT <plugin/sqlite3/Sqlite3Schema.h>
#define MODEL <midbc/midbc.sch>
#include "Sql/sch_header.h"

#include "dbclex.h"

class midbc : public WithmidbcLayout<TopWindow> {
public:
	typedef midbc CLASSNAME;
	midbc();
	void Quit();
	void FileMenu(Bar& bar);
	void MainMenu(Bar& bar);
	void HelpMenu(Bar& bar);
	void ReportsMenu(Bar& bar);
	void About();
	void Import();
	void Update();
};

#endif
