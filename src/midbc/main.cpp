#include "midbc.h"
#include <Sql/sch_schema.h>

#define IMAGEFILE <midbc/midbc.iml>
#include <Draw/iml_source.h>

void midbc::Quit(){
	Break();
}
void midbc::About(){
}
void midbc::FileMenu(Bar& bar)
{
	bar.Add("Import..", CtrlImg::open(), THISBACK(Import));
    /*bar.Add("Open..", CtrlImg::open(), THISBACK(Open));
    bar.Add("Save", CtrlImg::save(), THISBACK(Save));
    bar.Add("Save as..", CtrlImg::save_as(), THISBACK(SaveAs));
    bar.Separator();
    bar.Add("Print", CtrlImg::print(), THISBACK(Print));*/
    bar.Separator();
    bar.Add("Quit", THISBACK(Quit));
}
void midbc::ReportsMenu(Bar& bar){
}
void midbc::HelpMenu(Bar& bar){
	bar.Add("About", THISBACK(About));
}
void midbc::MainMenu(Bar& bar)
{
    bar.Add("File", THISBACK(FileMenu));
    bar.Add("Reports", THISBACK(ReportsMenu));
    bar.Add("Help", THISBACK(HelpMenu));
}
void midbc::Import(){
	FileSel fs;
	if (fs.ExecuteOpen()){
		String s=fs.Get();
		DbcLex l;
		l.SetFilename(s);
		l.Parse();
	}
}
void midbc::Update(){
	tree.Clear();
	tree.SetRoot(CtrlImg::Dir(), "root");
	/*
	int parent=0;
	String node;
	Image img= CtrlImg::Dir();
	int q = tree.Add(parent, img,  node);
	*/
}

midbc::midbc()
{
	CtrlLayout(*this, "MI DBC");
	Sizeable().Zoomable();
	//ActiveFocus(stock.edfName);
	menu.Set(THISBACK(MainMenu));
}

GUI_APP_MAIN
{
	Sqlite3Session sqlite3;
	sqlite3.LogErrors(true);
	if(!sqlite3.Open(ConfigFile("cdb.db"))) {
		LOG("Can't create or open database file\n");
		return;
	}

	SQL = sqlite3;

	// Update the schema to match the schema described in "simple.sch"
//#ifdef _DEBUG
	SqlSchema sch(SQLITE3);
	All_Tables(sch);
	if(sch.ScriptChanged(SqlSchema::UPGRADE))
		SqlPerformScript(sch.Upgrade());
	if(sch.ScriptChanged(SqlSchema::ATTRIBUTES)) {
		SqlPerformScript(sch.Attributes());
	}
	if(sch.ScriptChanged(SqlSchema::CONFIG)) {
		SqlPerformScript(sch.ConfigDrop());
		SqlPerformScript(sch.Config());
	}
	sch.SaveNormal();
//	#endif

	midbc().Run();
}
