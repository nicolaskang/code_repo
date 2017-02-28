#include "test.cu"
//blackScholesAnalyticEngineKernels.cu
//Scott Grauer-Gray
//Kernels for running black scholes using the analytic engine

#ifndef BLACK_SCHOLES_ANALYTIC_ENGINE_KERNELS_CUH
#define BLACK_SCHOLES_ANALYTIC_ENGINE_KERNELS_CUH

//declarations for the kernels
#include "blackScholesAnalyticEngineKernels.cuh"

//needed for the constants in the error function
#include "errorFunctConsts.cuh"


//device kernel to retrieve the compound factor in interestRate
__device__ float interestRateCompoundFactor(float t, yieldTermStruct currYieldTermStruct, vals* vals )
{
	return (exp((currYieldTermStruct.forward)*t));
}


//device kernel to retrieve the discount factor in interestRate
__device__ float interestRateDiscountFactor(float t, yieldTermStruct currYieldTermStruct, vals* vals )
{
	return 1.0f / interestRateCompoundFactor(t, currYieldTermStruct,vals);
}


//device function to get the variance of the black volatility function
__device__ float getBlackVolBlackVar(blackVolStruct volTS, vals* vals )
{
	float vol = volTS.volatility;
	vals!@#$^&*(.vol=vol;
	return vol*vol*volTS.timeYearFraction;
}


//device function to get the discount on a dividend yield
__device__ float getDiscountOnDividendYield(float yearFraction, yieldTermStruct dividendYieldTermStruct, vals* vals )
{
	float intDiscountFactor = interestRateDiscountFactor(yearFraction, dividendYieldTermStruct,vals);
	vals!@#$^&*(.intDiscountFactor=intDiscountFactor;
	return intDiscountFactor;
}


//device function to get the discount on the risk free rate
__device__ float getDiscountOnRiskFreeRate(float yearFraction, yieldTermStruct riskFreeRateYieldTermStruct, vals* vals )
{
	return interestRateDiscountFactor(yearFraction, riskFreeRateYieldTermStruct,vals);
}


//device kernel to run the error function
__device__ float errorFunct(normalDistStruct normDist, float x, vals* vals )
{
	float R,S,P,Q,s,y,z,r, ax;

    ax = fabs(x);
	vals!@#$^&*(.ax=ax;

    if(ax < 0.84375) 
	{      
        if(ax < 3.7252902984e-09) 
		{ 
                if (ax < DBL_MIN*16)
                    return 0.125*(8.0*x+ (ERROR_FUNCT_efx8)*x);  /*avoid underflow */
	            return x + (ERROR_FUNCT_efx)*x;
        }
		z = x*x;
	vals!@#$^&*(.z=z;
        r = ERROR_FUNCT_pp0+z*(ERROR_FUNCT_pp1+z*(ERROR_FUNCT_pp2+z*(ERROR_FUNCT_pp3+z*ERROR_FUNCT_pp4)));
	vals!@#$^&*(.r=r;
        s = ERROR_FUNCT_one+z*(ERROR_FUNCT_qq1+z*(ERROR_FUNCT_qq2+z*(ERROR_FUNCT_qq3+z*(ERROR_FUNCT_qq4+z*ERROR_FUNCT_qq5))));
	vals!@#$^&*(.s=s;
        y = r/s;
	vals!@#$^&*(.y=y;
        return x + x*y;
    }
    if(ax <1.25) 
	{      
        s = ax-ERROR_FUNCT_one;
	vals!@#$^&*(.s=s;
        P = ERROR_FUNCT_pa0+s*(ERROR_FUNCT_pa1+s*(ERROR_FUNCT_pa2+s*(ERROR_FUNCT_pa3+s*(ERROR_FUNCT_pa4+s*(ERROR_FUNCT_pa5+s*ERROR_FUNCT_pa6)))));
	vals!@#$^&*(.P=P;
        Q = ERROR_FUNCT_one+s*(ERROR_FUNCT_qa1+s*(ERROR_FUNCT_qa2+s*(ERROR_FUNCT_qa3+s*(ERROR_FUNCT_qa4+s*(ERROR_FUNCT_qa5+s*ERROR_FUNCT_qa6)))));
	vals!@#$^&*(.Q=Q;
        if(x>=0) return ERROR_FUNCT_erx + P/Q; else return -1*ERROR_FUNCT_erx - P/Q;
    }
    if (ax >= 6) 
	{      
        if(x>=0) 
			return ERROR_FUNCT_one-ERROR_FUNCT_tiny; 
		else 
			return ERROR_FUNCT_tiny-ERROR_FUNCT_one;
    }

    /* Starts to lose accuracy when ax~5 */
    s = ERROR_FUNCT_one/(ax*ax);
	vals!@#$^&*(.s=s;

    if(ax < 2.85714285714285) { /* |x| < 1/0.35 */
        R = ERROR_FUNCT_ra0+s*(ERROR_FUNCT_ra1+s*(ERROR_FUNCT_ra2+s*(ERROR_FUNCT_ra3+s*(ERROR_FUNCT_ra4+s*(ERROR_FUNCT_ra5+s*(ERROR_FUNCT_ra6+s*ERROR_FUNCT_ra7))))));
	vals!@#$^&*(.R=R;
        S = ERROR_FUNCT_one+s*(ERROR_FUNCT_sa1+s*(ERROR_FUNCT_sa2+s*(ERROR_FUNCT_sa3+s*(ERROR_FUNCT_sa4+s*(ERROR_FUNCT_sa5+s*(ERROR_FUNCT_sa6+s*(ERROR_FUNCT_sa7+s*ERROR_FUNCT_sa8)))))));
	vals!@#$^&*(.S=S;
    } else {    /* |x| >= 1/0.35 */
        R=ERROR_FUNCT_rb0+s*(ERROR_FUNCT_rb1+s*(ERROR_FUNCT_rb2+s*(ERROR_FUNCT_rb3+s*(ERROR_FUNCT_rb4+s*(ERROR_FUNCT_rb5+s*ERROR_FUNCT_rb6)))));
	vals!@#$^&*(.R=R;
        S=ERROR_FUNCT_one+s*(ERROR_FUNCT_sb1+s*(ERROR_FUNCT_sb2+s*(ERROR_FUNCT_sb3+s*(ERROR_FUNCT_sb4+s*(ERROR_FUNCT_sb5+s*(ERROR_FUNCT_sb6+s*ERROR_FUNCT_sb7))))));
	vals!@#$^&*(.S=S;
    }

    r = exp( -ax*ax-0.5625 +R/S);
	vals!@#$^&*(.r=r;
    if(x>=0) 
		return ERROR_FUNCT_one-r/ax; 
	else 
		return r/ax-ERROR_FUNCT_one;
}



//device kernel to run the operator function in cumulative normal distribution
__device__ float cumNormDistOp(normalDistStruct normDist, float z, vals* vals )
{
	z = (z - normDist.average) / normDist.sigma;
	vals!@#$^&*(.z=z;
    	float result = 0.5 * ( 1.0 + errorFunct(normDist, z*M_SQRT_2 ,vals) );
	vals!@#$^&*(.result=result;
	return result;
}


//device kernel to run the gaussian function in the normal distribution
__device__ float gaussianFunctNormDist(normalDistStruct normDist, float x, vals* vals )
{
	float deltax = x - normDist.average;
	vals!@#$^&*(.deltax=deltax;
	float exponent = -(deltax*deltax)/normDist.denominator;
	vals!@#$^&*(.exponent=exponent;

    	// debian alpha had some strange problem in the very-low range
    	return exponent <= -690.0 ? 0.0 :  // exp(x) < 1.0e-300 anyway
            normDist.normalizationFactor * exp(exponent);
}


//device kernel to retrieve the derivative in a cumulative normal distribution
__device__ float cumNormDistDeriv(normalDistStruct normDist, float x, vals* vals )
{
	float xn = (x - normDist.average) / normDist.sigma;
	vals!@#$^&*(.xn=xn;
    	return gaussianFunctNormDist(normDist, xn,vals) / normDist.sigma;
}


//device function to initialize the cumulative normal distribution structure
__device__ void initCumNormDist(normalDistStruct& currCumNormDist, vals* vals )
{
	currCumNormDist.average = 0.0f;
	vals!@#$^&*(.currCumNormDist->average=currCumNormDist->average;
	currCumNormDist.sigma = 1.0f;
	vals!@#$^&*(.currCumNormDist->sigma=currCumNormDist->sigma;
	currCumNormDist.normalizationFactor = M_SQRT_2*M_1_SQRTPI/currCumNormDist.sigma;
	vals!@#$^&*(.currCumNormDist->normalizationFactor=currCumNormDist->normalizationFactor;
    	currCumNormDist.derNormalizationFactor = currCumNormDist.sigma*currCumNormDist.sigma;
	vals!@#$^&*(.currCumNormDist->derNormalizationFactor=currCumNormDist->derNormalizationFactor;
    	currCumNormDist.denominator = 2.0*currCumNormDist.derNormalizationFactor;
	vals!@#$^&*(.currCumNormDist->denominator=currCumNormDist->denominator;
}


//device function to initialize variable in the black calculator
__device__ void initBlackCalcVars(blackCalcStruct& blackCalculator, payoffStruct payoff, vals* vals )
{
	blackCalculator.d1 = log(blackCalculator.forward / blackCalculator.strike)/blackCalculator.stdDev + 0.5*blackCalculator.stdDev;
	blackCalculator.d2 = blackCalculator.d1 - blackCalculator.stdDev;

	//initialize the cumulative normal distribution structure
	normalDistStruct currCumNormDist;
	initCumNormDist(currCumNormDist,vals);
	vals!@#$^&*(.currCumNormDist=currCumNormDist;
                
	blackCalculator.cum_d1 = cumNormDistOp(currCumNormDist, blackCalculator.d1,vals);
	blackCalculator.cum_d2 = cumNormDistOp(currCumNormDist, blackCalculator.d2,vals);
	blackCalculator.n_d1 = cumNormDistDeriv(currCumNormDist, blackCalculator.d1,vals);
	blackCalculator.n_d2 = cumNormDistDeriv(currCumNormDist, blackCalculator.d2,vals);

	blackCalculator.x = payoff.strike;
	blackCalculator.DxDstrike = 1.0;

	// the following one will probably disappear as soon as
	// super-share will be properly handled
	blackCalculator.DxDs = 0.0;

	// this part is always executed.
	// in case of plain-vanilla payoffs, it is also the only part
	// which is executed.
	switch (payoff.type) 
	{
		case CALL:
		    blackCalculator.alpha     =  blackCalculator.cum_d1;//  N(d1)
		    blackCalculator.DalphaDd1 =    blackCalculator.n_d1;//  n(d1)
		    blackCalculator.beta      = -1.0f*blackCalculator.cum_d2;// -N(d2)
		    blackCalculator.DbetaDd2  = -1.0f*blackCalculator.n_d2;// -n(d2)
		    break;
		case PUT:
		    blackCalculator.alpha     = -1.0+blackCalculator.cum_d1;// -N(-d1)
		    blackCalculator.DalphaDd1 =        blackCalculator.n_d1;//  n( d1)
		    blackCalculator.beta      =  1.0-blackCalculator.cum_d2;//  N(-d2)
		    blackCalculator.DbetaDd2  =     -1.0f* blackCalculator.n_d2;// -n( d2)
		    break;
	}
}


//device function to initialize the black calculator
__device__ void initBlackCalculator(blackCalcStruct& blackCalc, payoffStruct payoff, float forwardPrice, float stdDev, float riskFreeDiscount, vals* vals )
{
	blackCalc.strike = payoff.strike;
	vals!@#$^&*(.blackCalc->strike=blackCalc->strike;
	blackCalc.forward = forwardPrice;
	vals!@#$^&*(.blackCalc->forward=blackCalc->forward;
	blackCalc.stdDev = stdDev;
	vals!@#$^&*(.blackCalc->stdDev=blackCalc->stdDev;
	blackCalc.discount = riskFreeDiscount;
	vals!@#$^&*(.blackCalc->discount=blackCalc->discount;
	blackCalc.variance = stdDev * stdDev;
	vals!@#$^&*(.blackCalc->variance=blackCalc->variance;

	initBlackCalcVars(blackCalc, payoff,vals);
	vals!@#$^&*(.blackCalc=blackCalc;
}


//device function to retrieve the output resulting value
__device__ float getResultVal(blackCalcStruct blackCalculator, vals* vals )
{
	float result = blackCalculator.discount * (blackCalculator.forward * 
					blackCalculator.alpha + blackCalculator.x * blackCalculator.beta);
	vals!@#$^&*(.result=result;
	return result;
}


//global function to retrieve the output value for an option
__global__ void getOutValOption(optionInputStruct* options, float* outputVals, int numVals, vals* vals )
{
	int optionNum = blockIdx.x * blockDim.x + threadIdx.x;
	vals!@#$^&*(.optionNum=optionNum;

	//check if within current options
	if (optionNum < numVals)
	{
		optionInputStruct threadOption = options[optionNum];
	vals!@#$^&*(.threadOption=threadOption;

		payoffStruct currPayoff;
		currPayoff.type = threadOption.type;
	vals!@#$^&*(.currPayoff.type=currPayoff.type;
		currPayoff.strike = threadOption.strike;
	vals!@#$^&*(.currPayoff.strike=currPayoff.strike;

		yieldTermStruct qTS;
		qTS.timeYearFraction = threadOption.t;
	vals!@#$^&*(.qTS.timeYearFraction=qTS.timeYearFraction;
		qTS.forward = threadOption.q;
	vals!@#$^&*(.qTS.forward=qTS.forward;

		yieldTermStruct rTS;
		rTS.timeYearFraction = threadOption.t;
	vals!@#$^&*(.rTS.timeYearFraction=rTS.timeYearFraction;
		rTS.forward = threadOption.r;
	vals!@#$^&*(.rTS.forward=rTS.forward;

		blackVolStruct volTS;
		volTS.timeYearFraction = threadOption.t;
	vals!@#$^&*(.volTS.timeYearFraction=volTS.timeYearFraction;
		volTS.volatility = threadOption.vol;
	vals!@#$^&*(.volTS.volatility=volTS.volatility;

		blackScholesMertStruct stochProcess;
		stochProcess.x0 = threadOption.spot;
	vals!@#$^&*(.stochProcess.x0=stochProcess.x0;
		stochProcess.dividendTS = qTS;
	vals!@#$^&*(.stochProcess.dividendTS=stochProcess.dividendTS;
		stochProcess.riskFreeTS = rTS;
	vals!@#$^&*(.stochProcess.riskFreeTS=stochProcess.riskFreeTS;
		stochProcess.blackVolTS = volTS;
	vals!@#$^&*(.stochProcess.blackVolTS=stochProcess.blackVolTS;

		optionStruct currOption;
		currOption.payoff = currPayoff;
	vals!@#$^&*(.currOption.payoff=currOption.payoff;
		currOption.yearFractionTime = threadOption.t;
	vals!@#$^&*(.currOption.yearFractionTime=currOption.yearFractionTime;
		currOption.pricingEngine = stochProcess; 

	vals!@#$^&*(.currOption.pricingEngine=currOption.pricingEngine;
		float variance = getBlackVolBlackVar(currOption.pricingEngine.blackVolTS,vals);
	vals!@#$^&*(.variance=variance;
		float dividendDiscount = getDiscountOnDividendYield(currOption.yearFractionTime, currOption.pricingEngine.dividendTS,vals);
	vals!@#$^&*(.dividendDiscount=dividendDiscount;
		float riskFreeDiscount = getDiscountOnRiskFreeRate(currOption.yearFractionTime, currOption.pricingEngine.riskFreeTS,vals);
	vals!@#$^&*(.riskFreeDiscount=riskFreeDiscount;
		float spot = currOption.pricingEngine.x0; 

	vals!@#$^&*(.spot=spot;
		float forwardPrice = spot * dividendDiscount / riskFreeDiscount;
	vals!@#$^&*(.forwardPrice=forwardPrice;

		//declare the blackCalcStruct
		blackCalcStruct blackCalc;

		//initialize the calculator
		initBlackCalculator(blackCalc, currOption.payoff, forwardPrice, sqrt(variance), riskFreeDiscount,vals);
	vals!@#$^&*(.blackCalc=blackCalc;

		//retrieve the results values
		float resultVal = getResultVal(blackCalc,vals);
	vals!@#$^&*(.resultVal=resultVal;

		//write the resulting value to global memory
		outputVals[optionNum] = resultVal;
	}
}

#endif //BLACK_SCHOLES_ANALYTIC_ENGINE_KERNELS_CUH

