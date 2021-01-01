// RRAPI_Aggregator.cpp : Defines the functions for the static library.
//
#include "framework.h"
#include "RRAPI_Aggregator.h"

ns1__authInfo _auth;
RRWsdlBindingProxy _rrapi;

RRAPI_Aggregator::RRAPI_Aggregator(const string username, string const password, const string apiKey) {
	_auth.appKey = apiKey;
	_auth.password = password;
	_auth.username = username;
}

RRAPI_Aggregator::~RRAPI_Aggregator() {
	_rrapi.destroy();
}

SimpleType RRAPI_Aggregator::_getTRSType(const int id) {
	SimpleType ret;
	ns1__getTrsTypeResponse resp;
	int s1 = _rrapi.getTrsType(id, &_auth, resp);
	if (s1 != SOAP_OK) { throw exception(_rrapi.soap_fault_string()); }
	if (!resp.return_) {
		string errMsg = "Trunked System Type with ID '" + to_string(id) + "' not found";
		throw exception(errMsg.c_str());
	}
	int iCount = resp.return_->__size;
	if (iCount > 0) {
		ret.ID = resp.return_->__ptr[0]->sType;
		ret.Descr = resp.return_->__ptr[0]->sTypeDescr;
	}
	return ret;
}

SimpleType RRAPI_Aggregator::_getTRSVoice(const int id) {
	SimpleType ret;
	ns1__getTrsVoiceResponse resp;
	int s1 = _rrapi.getTrsVoice(id, &_auth, resp);
	if (s1 != SOAP_OK) { throw exception(_rrapi.soap_fault_string()); }
	if (!resp.return_) {
		string errMsg = "Trunked System Voice with ID '" + to_string(id) + "' not found";
		throw exception(errMsg.c_str());
	}
	int iCount = resp.return_->__size;
	if (iCount > 0) {
		ret.ID = resp.return_->__ptr[0]->sVoice;
		ret.Descr = resp.return_->__ptr[0]->sVoiceDescr;
	}
	return ret;
}

SimpleType RRAPI_Aggregator::_getTRSFlavor(const int id) {
	SimpleType ret;
	ns1__getTrsFlavorResponse resp;
	int s1 = _rrapi.getTrsFlavor(id, &_auth, resp);
	if (s1 != SOAP_OK) { throw exception(_rrapi.soap_fault_string()); }
	if (!resp.return_) {
		string errMsg = "Trunked System Flavor with ID '" + to_string(id) + "' not found";
		throw exception(errMsg.c_str());
	}
	int iCount = resp.return_->__size;
	if (iCount > 0) {
		ret.ID = resp.return_->__ptr[0]->sFlavor;
		ret.Descr = resp.return_->__ptr[0]->sFlavorDescr;
	}
	return ret;
}

TRS_SystemHeader RRAPI_Aggregator::_getTRS_System_Header(const string sysid) {
	TRS_SystemHeader ret;

	ns1__getTrsBySysidResponse resp;

	int s1 = _rrapi.getTrsBySysid(sysid, &_auth, resp);
	if (s1 != SOAP_OK) { throw exception(_rrapi.soap_fault_string()); }
	if (!resp.return_) {
		string errMsg = "Trunked System with ID '" + sysid + "' not found";
		throw exception(errMsg.c_str());
	}

	int iCount = resp.return_->__size;
	if (iCount > 0) {
		ret.City = resp.return_->__ptr[0]->sCity;
		ret.Name = resp.return_->__ptr[0]->sName;
		ret.SystemID = resp.return_->__ptr[0]->sid;
		ret.SystemID_Hex = sysid;
		ret.SystemType = _getTRSType(resp.return_->__ptr[0]->sType);
		ret.Voice = _getTRSVoice(resp.return_->__ptr[0]->sVoice);
		ret.Flavor = _getTRSFlavor(resp.return_->__ptr[0]->sFlavor);
	}

	return ret;
};

vector<string> RRAPI_Aggregator::_extractLicenseCallsigns(TrsSiteLicenses* const  licenses) {
	vector<string> ret;
	if (licenses) {
		int iCount = licenses->__size;
		if (iCount > 0) {
			for (int iIdx = 0; iIdx < iCount; iIdx++) {
				ret.push_back(licenses->__ptr[iIdx]->license);
			}
		}
	}
	return ret;
}

vector<TRS_Tower> RRAPI_Aggregator::_getTRS_Callsign_Towers(const vector<string> callSigns) {
	//call fccCallsign for each call sign
	vector<TRS_Tower> ret;
	for (auto const& c : callSigns) {
		ns1__fccGetCallsignResponse resp;
		int s1 = _rrapi.fccGetCallsign(c, &_auth, resp);
		if (s1 != SOAP_OK) { throw exception(_rrapi.soap_fault_string()); }
		if (resp.return_) {
			//Iterate locations
			int iCount = resp.return_->locations->__size;
			for (int iLocIdx = 0; iLocIdx < iCount; iLocIdx++) {
				auto loc = resp.return_->locations->__ptr[iLocIdx];
				//Get TOWER/MAST locations only
				if (loc->type == "TOWER" || loc->type == "MAST") {
					TRS_Tower t;
					t.Address = loc->address;
					t.Elevation = stod(loc->antennaHeight);
					t.City = loc->city;
					t.County = loc->county;
					t.Latitude = stod(loc->lat);
					t.Longitude = stod(loc->lon);
					t.State = loc->state;

					ret.push_back(t);
				}
			}
		}
	}

	return ret;
}

vector<TRS_Site> RRAPI_Aggregator::_getTRS_SystemSites(const int systemId) {
	vector<TRS_Site> ret;
	ns1__getTrsSitesResponse resp;

	int s1 = _rrapi.getTrsSites(systemId, &_auth, resp);
	if (s1 != SOAP_OK) { throw exception(_rrapi.soap_fault_string()); }

	if (resp.return_) {
		int iSiteCount = resp.return_->__size;
		if (iSiteCount > 0) {
			for (int iSiteIdx = 0; iSiteIdx < iSiteCount; iSiteIdx++) {
				TRS_Site s;

				//Header
				s.Header.SystemID = systemId;
				s.Header.Latitude = stod(resp.return_->__ptr[iSiteIdx]->lat);
				s.Header.Longitude = stod(resp.return_->__ptr[iSiteIdx]->lon);
				s.Header.SiteID = resp.return_->__ptr[iSiteIdx]->siteId;
				s.Header.SiteDescr = resp.return_->__ptr[iSiteIdx]->siteDescr;
				s.Header.CountyID = resp.return_->__ptr[iSiteIdx]->siteCtid;
				s.Header.CountyDescr = resp.return_->__ptr[iSiteIdx]->siteCt;
				s.Header.Modulation = resp.return_->__ptr[iSiteIdx]->siteModulation;
				s.Header.Range = stod(resp.return_->__ptr[iSiteIdx]->range);

				//Towers
				vector<string> callSigns = _extractLicenseCallsigns(resp.return_->__ptr[iSiteIdx]->siteLicenses);
				s.Towers = _getTRS_Callsign_Towers(callSigns);

				//Frequencies
				auto freqs = resp.return_->__ptr[iSiteIdx]->siteFreqs;
				int iFreqCount = freqs->__size;
				if (iFreqCount > 0) {
					for (int iFreqIdx = 0; iFreqIdx < iFreqCount; iFreqIdx++) {
						TRS_Site_Freq f;
						f.Freq = stod(freqs->__ptr[iFreqIdx]->freq);
						f.LCN = freqs->__ptr[iFreqIdx]->lcn;
						f.Use = freqs->__ptr[iFreqIdx]->use;
						s.Frequencies.push_back(f);
					}
				}

				ret.push_back(s);
			}
		}
	}
	return ret;
}

vector<TRS_Talkgroup_Category> RRAPI_Aggregator::_getTRS_Talkgroup_Categories(const int systemId) {
	vector<TRS_Talkgroup_Category> ret;
	ns1__getTrsTalkgroupCatsResponse resp;
	int s1 = _rrapi.getTrsTalkgroupCats(systemId, &_auth, resp);
	if (s1 != SOAP_OK) { throw exception(_rrapi.soap_fault_string()); }

	if (resp.return_) {
		int iCatCount = resp.return_->__size;
		if (iCatCount > 0) {
			auto cats = resp.return_->__ptr;
			for (int iCatIdx = 0; iCatIdx < iCatCount; iCatIdx++) {
				TRS_Talkgroup_Category cat;
				cat.CategoryID = cats[iCatIdx]->tgCid;
				cat.CategoryName = cats[iCatIdx]->tgCname;
				cat.Sort = cats[iCatIdx]->tgSort;
				cat.SortBy = cats[iCatIdx]->tgSortBy;
				cat.Latitude = stod(cats[iCatIdx]->lat);
				cat.Longitude = stod(cats[iCatIdx]->lon);
				cat.Range = stod(cats[iCatIdx]->range);

				ret.push_back(cat);
			}
		}
	}

	return ret;
}

vector<TRS_Talkgroup> RRAPI_Aggregator::_getTRS_Talkgroups(const int systemId, const int catId, const int tagId, const int tgDecId) {
	vector<TRS_Talkgroup> ret;
	ns1__getTrsTalkgroupsResponse resp;
	int s1 = _rrapi.getTrsTalkgroups(systemId, catId, tagId, tgDecId, &_auth, resp);
	if (s1 != SOAP_OK) { throw exception(_rrapi.soap_fault_string()); }

	if (resp.return_) {
		int iTGCount = resp.return_->__size;
		if (iTGCount > 0) {
			for (int iTGIdx = 0; iTGIdx < iTGCount; iTGIdx++) {
				TRS_Talkgroup tg;
				tg.AlphaTag = resp.return_->__ptr[iTGIdx]->tgAlpha;
				tg.Descr = resp.return_->__ptr[iTGIdx]->tgDescr;
				tg.Mode = resp.return_->__ptr[iTGIdx]->tgMode;
				auto tags = resp.return_->__ptr[iTGIdx]->tags_;
				int iTagCount = tags->__size;
				if (iTagCount > 0) {
					for (int iTagIdx = 0; iTagIdx < iTagCount; iTagIdx++) {
						TRS_Tag tag;
						tag.TagDescr = tags->__ptr[iTagIdx]->tagDescr;
						tag.TagID = tags->__ptr[iTagIdx]->tagId;
						tg.Tags.push_back(tag);
					}
				}
				ret.push_back(tg);
			}
		}
	}
	return ret;
}

TRS_System RRAPI_Aggregator::GetTrunkedSystemById(const string systemHexId)
{
	TRS_System ret;
	TRS_SystemHeader header = _getTRS_System_Header(systemHexId);	//Must be called first
	ret.Header = header;
	ret.TalkgroupCategories = _getTRS_Talkgroup_Categories(ret.Header.SystemID);
	ret.Talkgroups = _getTRS_Talkgroups(ret.Header.SystemID);
	ret.Sites = _getTRS_SystemSites(ret.Header.SystemID);

	return ret;
}