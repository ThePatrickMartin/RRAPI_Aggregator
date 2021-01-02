#pragma once
#define DllExport   __declspec( dllexport )

#include <rrapi_RRWsdlBindingProxy.h>
#include <vector>;
#include <map>;

using namespace std;

/// <summary>
/// Type having only ID and Description
/// </summary>
class DllExport SimpleType {
public:
	int ID = 0;
	string Descr;
};

class DllExport TRS_Tag {
public:
	int TagID = 0;
	string TagDescr;
};

class DllExport TRS_Site_Freq {
public:
	int LCN = 0;
	double Freq = 0.0;
	string Use;
};

class DllExport TRS_Tower {
public:
	int TowerID;
	double Latitude = 0.0;
	double Longitude = 0.0;
	double Elevation = 0.0;
	string Address;
	string City;
	string County;
	string State;
};



class DllExport TRS_Site_Header {
public:
	int SiteID = 0;
	int SystemID = 0;
	int CountyID = 0;
	string SiteDescr;
	string CountyDescr;
	string Modulation;
	double Latitude = 0.0;
	double Longitude = 0.0;
	double Range = 0.0;
};

class DllExport TRS_Talkgroup_Category {
public:
	int CategoryID = 0;
	int SystemId = 0;
	string CategoryName;
	double Latitude = 0.0;
	double Longitude = 0.0;
	double Range = 0.0;
	int Sort = 0;
	int SortBy = 1;
};


class DllExport TRS_Talkgroup {
public:
	TRS_Talkgroup_Category Category;
	int TGID = 0;
	int TGID_Dec = 0;
	string Descr;
	string AlphaTag;
	string Mode;
	vector<TRS_Tag> Tags;
};



class DllExport TRS_SystemHeader {
public:
	string SystemID_Hex;
	string WACN;
	int SystemID = 0;
	string Name;
	SimpleType SystemType;
	SimpleType Flavor;
	SimpleType Voice;
	string City;
protected:

};

class DllExport TRS_Site {
public:
	TRS_Site_Header Header;
	vector<TRS_Site_Freq> Frequencies;
	map<int, TRS_Tower> Towers;
};

class DllExport TRS_System {
public:
	TRS_SystemHeader Header;
	map<int, TRS_Site> Sites;
	map<int, TRS_Talkgroup> Talkgroups;
	map<int, TRS_Talkgroup_Category> TalkgroupCategories;
};

class DllExport RRAPI_Aggregator {
public:
	RRAPI_Aggregator(const string username, const string password, const string apiKey);
	~RRAPI_Aggregator();

	TRS_System GetTrunkedSystemById(const string systemHexId);
private:
	SimpleType _getTRSType(const int id);
	SimpleType _getTRSVoice(const int id);
	SimpleType _getTRSFlavor(const int id);
	TRS_SystemHeader _getTRS_System_Header(const string sysid);
	vector<string> _extractLicenseCallsigns(TrsSiteLicenses* const licenses);
	map<int, TRS_Tower> _getTRS_Callsign_Towers(const vector<string>  callSigns);
	map<int, TRS_Site> _getTRS_SystemSites(const int systemId);
	map<int, TRS_Talkgroup_Category> _getTRS_Talkgroup_Categories(const int systemId);
	map<int, TRS_Talkgroup> _getTRS_Talkgroups(const int systemId, const int catId = 0, const int tagId = 0, const int tgDecId = 0);
};