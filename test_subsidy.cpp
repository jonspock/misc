#include <cmath>
#include <iostream>
#include <fstream>
#include "plot_fft.h"

int GetBlockSubsidy(int nHeight, int nPeakHeight, int nMaxReward) {
  // Peak currently happens 1 year out
  int64_t nReward;

  if (nHeight <= nPeakHeight) {
    nReward = (nMaxReward/2) + int((nMaxReward*nHeight)/(nPeakHeight+nHeight));
  } else {
    nReward =  int((nMaxReward * nPeakHeight)/nHeight);
  }
  return nReward;
}

int main(int argc, const char *argv[]) {
  const int years = 5;
  const int nBlocksPerDay = 30 * 24; // 2 minute
  const int nBlocksPerMonth = nBlocksPerDay * 30;
  const int nBlocksPerYear = nBlocksPerDay * 365.25;
  const int nPeak = 1.5*nBlocksPerYear;
  const int nMaxReward = 2000;
  const double billion = 1000000000;

  const double cold_fraction = 0.75; // 75% setup for rewards
  //const double crpc[] = {15,13,11,9,7};
  const double crpc[] = {10,9,8,7,6};
  
  std::vector<double> x;
  std::vector<double> x_inf;
  std::vector<double> inflation;
  std::vector<double> rewards;
  std::vector<double> supply;
  std::ofstream rf("rewards.txt");
  int64_t s=0;

  double cr = 0;
  
  for (int i=0;i<nBlocksPerYear*years;i++) {
    int64_t r = GetBlockSubsidy(i, nPeak, nMaxReward);

    int64_t year_number = i/nBlocksPerYear;
    double cr_per_block_per_coin = crpc[year_number]/nBlocksPerYear;

    s += r;

    // Just add the CR Once per month for simplicity
    if (i % nBlocksPerMonth == 0) {
      // 12 for months, 100 to convert % to fraction
      cr = ((cold_fraction*s)/12.0)*crpc[year_number]/100.0;
      s += cr;
    }

    r += (cr/nBlocksPerMonth);
    
    //std::cout << i << " supply = " << s << " " << added << " r = " << r << "\n";
    
    double infl = 100.0*r*nBlocksPerYear/(double)s;
    if (i % nBlocksPerMonth == 0) {
      supply.push_back(s/billion);
      rewards.push_back(r);
      x.push_back(i/(double)nBlocksPerYear);
      rf << (double)i/nBlocksPerYear << " " << r << " " << s/billion << " " << infl  << "\n";
    }
    if ((i % nBlocksPerMonth == 0) && (i>nBlocksPerYear)) {
      x_inf.push_back(i/(double)nBlocksPerYear);
      inflation.push_back(infl);
    }
  }
  rf.close();

  plot_xy(x, supply, "supply per year (billions)");
  plot_xy(x, rewards, "rewards per year (coins)");
  plot_xy(x_inf, inflation, "inflation per year (%)");
  
  return (0);
}
