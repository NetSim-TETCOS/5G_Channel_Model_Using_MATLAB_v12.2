

typedef struct stru_pathlossinfo
{
	double x;
	double y;
	double pathloss;
	struct stru_pathlossinfo* next;
}INFO, * ptrINFO;
ptrINFO* pathlossInfo;
ptrINFO* lastInfo;