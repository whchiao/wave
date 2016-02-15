
#include "stdafx.h"
#include "Wave.h"
#include <vector>
#include <map>
#include <string>
#import "C:\Program Files (x86)\TradeStation 9.5\Program\tskit.dll"  no_namespace
//#import "C:\Program Files (x86)\TS Support\MultiCharts\PLKit.dll"  no_namespace

int currentbar;
std::map<std::string, WAVE_RUNTIME> WR;
std::vector<WAVE> WaveData;

__declspec(dllexport) double __stdcall GetLowBar(IEasyLanguageObject * pELObj, LPSTR symname, int barinterval, int baseratio) 
{ 
	std::string key = symname + std::to_string(baseratio);
	return WR[key].LowBarNum;
}
__declspec(dllexport) double __stdcall GetHighBar(IEasyLanguageObject * pELObj, LPSTR symname, int barinterval, int baseratio) 
{ 
	std::string key = symname + std::to_string(baseratio);
	return WR[key].HighBarNum;
}
__declspec(dllexport) double __stdcall GetLowBarPrice(IEasyLanguageObject * pELObj, LPSTR symname, int barinterval, int baseratio) 
{ 
	std::string key = symname + std::to_string(baseratio);
	return pELObj->Low[pELObj->CurrentBar[data1] - WR[key].LowBarNum];
}
__declspec(dllexport) double __stdcall GetHighBarPrice(IEasyLanguageObject * pELObj, LPSTR symname, int barinterval, int baseratio) 
{ 
	std::string key = symname + std::to_string(baseratio);
	return pELObj->High[pELObj->CurrentBar[data1] - WR[key].HighBarNum];
}
__declspec(dllexport) double __stdcall GetLowBarTime(IEasyLanguageObject * pELObj, LPSTR symname, int barinterval, int baseratio) 
{ 
	std::string key = symname + std::to_string(baseratio);
	return pELObj->DateTime[pELObj->CurrentBar[data1] - WR[key].LowBarNum];
}
__declspec(dllexport) double __stdcall GetHighBarTime(IEasyLanguageObject * pELObj, LPSTR symname, int barinterval, int baseratio) 
{ 
	std::string key = symname + std::to_string(baseratio);
	return pELObj->DateTime[pELObj->CurrentBar[data1] - WR[key].HighBarNum];
}

__declspec(dllexport) double __stdcall Wave(IEasyLanguageObject * pELObj, LPSTR symname, int barinterval, int baseratio)
{
	
	int currentbar = pELObj->CurrentBar[data1];
	std::string key = symname + std::to_string(baseratio);
	
	if (currentbar == 1) {
		WR[key].FirstWaveAppear = false;
		WR[key].WaveCount = 0;
		WR[key].InUpWave = false;
		WR[key].InDownWave = false;
		WR[key].KBarNumPerUpWave = 1;
		WR[key].KBarNumPerDownWave = 1;
		WR[key].LowBarNum = 1;
		WR[key].HighBarNum = 1;
		WR[key].LowBarNumNew = 1;
		WR[key].HighBarNumNew = 1;
		WR[key].MinWaveKBarNum = 2;
		WR[key].MinWavePoints = pELObj->Close[0] * baseratio / 1000;
	}
	else {
		if (!WR[key].FirstWaveAppear) {

			if (pELObj->High[0] > pELObj->High[currentbar - WR[key].HighBarNum]) WR[key].HighBarNum = currentbar;
			if (pELObj->Low[0] < pELObj->Low[currentbar - WR[key].LowBarNum]) WR[key].LowBarNum = currentbar;
			WR[key].HighBarNumNew = WR[key].HighBarNum;
			WR[key].LowBarNumNew = WR[key].HighBarNum;
			if ((pELObj->High[currentbar - WR[key].HighBarNum] - pELObj->Low[currentbar - WR[key].LowBarNum]) >= WR[key].MinWavePoints) {
				if ((WR[key].HighBarNum == currentbar) && (WR[key].HighBarNum != WR[key].LowBarNum)) {
					WR[key].InUpWave = true;
					WR[key].InDownWave = false;
					WR[key].FirstWaveAppear = true;
					WR[key].WaveCount = WR[key].WaveCount + 1;
					WR[key].MinWavePoints = pELObj->Close[0] * baseratio / 1000;
				}

				if ((WR[key].LowBarNum == currentbar) && (WR[key].HighBarNum != WR[key].LowBarNum)) {
					WR[key].InUpWave = false;
					WR[key].InDownWave = true;
					WR[key].FirstWaveAppear = true;
					WR[key].WaveCount = WR[key].WaveCount + 1;
					WR[key].MinWavePoints = pELObj->Close[0] * baseratio / 1000;
				}
			}

		}

		if (WR[key].InUpWave) {
			if (pELObj->High[0] > pELObj->High[currentbar - WR[key].HighBarNumNew]) {
				//HighBarNum = currentbar;
				WR[key].HighBarNumNew = currentbar;
				WR[key].LowBarNumNew = currentbar;
			}
			if (pELObj->Low[0] < pELObj->Low[currentbar - WR[key].LowBarNumNew]) {
				WR[key].LowBarNumNew = currentbar;
				WR[key].KBarNumPerDownWave = currentbar - WR[key].HighBarNumNew + 1;
				if (((pELObj->High[currentbar - WR[key].HighBarNumNew] - pELObj->Low[0]) >= WR[key].MinWavePoints) && (WR[key].KBarNumPerDownWave >= WR[key].MinWaveKBarNum)) {
					WR[key].InUpWave = false;
					WR[key].InDownWave = true;
					
					WR[key].HighBarNum = WR[key].HighBarNumNew;
					WR[key].MinWavePoints = pELObj->Close[0] * baseratio / 1000;
					wave_insert(pELObj,HighBarNum, LowBarNum, true, WR[key].WaveCount);
					WR[key].WaveCount = WR[key].WaveCount + 1;
				}
			}
		}
		else if (WR[key].InDownWave) {
			if (pELObj->Low[0] < pELObj->Low[currentbar - WR[key].LowBarNumNew]) {
				WR[key].LowBarNumNew = currentbar;
				WR[key].HighBarNumNew = currentbar;
			}
			if (pELObj->High[0] > pELObj->High[currentbar - WR[key].HighBarNumNew]) {
				WR[key].HighBarNumNew = currentbar;
				WR[key].KBarNumPerUpWave = currentbar - WR[key].LowBarNum + 1;
				if (((pELObj->High[0] - pELObj->Low[currentbar - WR[key].LowBarNumNew]) >= WR[key].MinWavePoints) && (WR[key].KBarNumPerUpWave >= WR[key].MinWaveKBarNum)) {
					WR[key].InUpWave = true;
					WR[key].InDownWave = false;
					
					WR[key].LowBarNum = WR[key].LowBarNumNew;
					WR[key].MinWavePoints = pELObj->Close[0] * baseratio / 1000;
					wave_insert(pELObj,HighBarNum, LowBarNum, false, WR[key].WaveCount);
					WR[key].WaveCount = WR[key].WaveCount + 1;

				}
			}


		}
	}

	return WR[key].WaveCount;
}


void wave_insert(IEasyLanguageObject * pELObj, int hbar, int lbar, bool up, int count) {
	WAVE data;
	data.hbar = hbar;
	data.lbar = lbar;
	if (up) data.len = (hbar - lbar);
	else data.len = lbar - hbar;
	data.up = up;
	data.h = pELObj->High[pELObj->CurrentBar[data1] - hbar];
	data.l = pELObj->Low[pELObj->CurrentBar[data1] - lbar];
	data.htime=pELObj->DateTime[pELObj->CurrentBar[data1] - hbar];
	data.ltime = pELObj->DateTime[pELObj->CurrentBar[data1] - lbar];
	data.count = count;
	//WaveData.push_back(data);
	WaveData.insert(WaveData.begin(), data);
}

