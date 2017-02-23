struct payoffStruct;
struct yieldTermStruct;
struct blackVolStruct;
struct blackScholesMertStruct;
struct optionStruct;
struct blackCalcStruct;
struct normalDistStruct;
struct optionInputStruct;
typedef unsigned long size_t;
struct payoffStruct {
int type;
float strike;};
typedef struct payoffStruct payoffStruct;
struct yieldTermStruct {
float timeYearFraction;
float forward;
float compounding;
float frequency;
float intRate;};
typedef struct yieldTermStruct yieldTermStruct;
struct blackVolStruct {
float timeYearFraction;
float following;
float volatility;};
typedef struct blackVolStruct blackVolStruct;
struct blackScholesMertStruct {
float x0;
yieldTermStruct dividendTS;
yieldTermStruct riskFreeTS;
blackVolStruct blackVolTS;};
typedef struct blackScholesMertStruct blackScholesMertStruct;
struct optionStruct {
payoffStruct payoff;
float yearFractionTime;
blackScholesMertStruct pricingEngine;};
typedef struct optionStruct optionStruct;
struct blackCalcStruct {
float strike;
float forward;
float stdDev;
float discount;
float variance;
float d1;
float d2;
float alpha;
float beta;
float DalphaDd1;
float DbetaDd2;
float n_d1;
float cum_d1;
float n_d2;
float cum_d2;
float x;
float DxDs;
float DxDstrike;};
typedef struct blackCalcStruct blackCalcStruct;
struct normalDistStruct {
float average;
float sigma;
float denominator;
float derNormalizationFactor;
float normalizationFactor;};
typedef struct normalDistStruct normalDistStruct;
struct optionInputStruct {
int type;
float strike;
float spot;
float q;
float r;
float t;
float vol;
float value;
float tol;};
typedef struct optionInputStruct optionInputStruct;
