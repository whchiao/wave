#import "C:\Program Files (x86)\TradeStation 9.5\Program\tskit.dll"  no_namespace
class WAVE {
public:
	double h, l;
	int len;
	int htime, ltime;
	int hbar, lbar;
	bool up;
	int count;
	std::vector<double> relative_ratio;
};

class WAVE_RUNTIME {
public:
	int HighBarNum, LowBarNum, WaveCount, KBarNumPerUpWave, KBarNumPerDownWave;
	int LowBarNumNew, HighBarNumNew;
	bool InUpWave, InDownWave, FirstWaveAppear;
	int MinWaveKBarNum;
	double MinWavePoints;
	
};

void wave_insert(IEasyLanguageObject * pELObj, int hbar, int lbar, bool up);