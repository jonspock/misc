//#include "plot_fft.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

int GetBlockSubsidy(int nHeight, int nPeakHeight, int nInitReward) {
  // Peak currently happens 1 year out
  int64_t nReward;
  const int nPeakH = nPeakHeight/3;
  int64_t peak = nInitReward + int((2 * nInitReward * nPeakH) / (nPeakH + nPeakHeight));

  if (nHeight <= nPeakH) {
      nReward = (nInitReward) + int((2 * nInitReward * nHeight) / (nPeakHeight + nHeight));
  } else {
      nReward = nPeakH*peak/nHeight;
  }
  return nReward;
}

int main(int argc, const char *argv[]) {
  const int years = 10;
  const int nBlocksPerDay = 30 * 24; // 2 minute
  const int nBlocksPerMonth = nBlocksPerDay * 30;
  const int nBlocksPerYear = nBlocksPerDay * 365.25;
  const int nPeak = 1.5 * nBlocksPerYear;
  const int nInitReward = 500;
  const double billion = 1000000000;
  const int BudgetPerCent = 45;
  const int ScaleFactor = (100 - BudgetPerCent);

  const double cold_fraction = 0.75; // 75% setup for rewards
  std::vector<double> crpc = {15, 12, 9, 7, 5};

  std::vector<double> x;
  std::vector<double> x_inf;
  std::vector<double> inflation;
  std::vector<double> rewards;
  std::vector<double> supply;
  std::ofstream rf("rewards.txt");
  int64_t s = 0;

  double cr = 0;
  double br = 0;

  for (int i = 0; i < nBlocksPerYear * years; i++) {
    int64_t r = GetBlockSubsidy(i, nPeak, nInitReward);

    int64_t year_number = i / nBlocksPerYear;
    if (year_number > (crpc.size() - 1))
      year_number = crpc.size() - 1;
    double cr_per_block_per_coin = crpc[year_number] / nBlocksPerYear;

    s += r;

    // Just add the CR Once per month for simplicity
    if (i % nBlocksPerMonth == 0) {
      // 12 for months, 100 to convert % to fraction
      cr = ((cold_fraction * s) / 12.0) * crpc[year_number] / 100.0;
      if (i > 0)
        br = BudgetPerCent * r * nBlocksPerMonth / ScaleFactor;
      double sum = (cr + br) / nBlocksPerMonth + r;
      s += cr;
      s += br;
      /*
      std::cout << "cr[" << i / nBlocksPerMonth
                << "] = " << (int)cr / nBlocksPerMonth << " vs block = " << r
                << " budget = " << br / nBlocksPerMonth << " sum = " << sum
                << " supply = " << s
                << "\n";
      */
      std::cout << i / nBlocksPerMonth << " " << r  << " " << sum << " " << s << "\n";
    }

    r += (cr / nBlocksPerMonth);
    r += (br / nBlocksPerMonth);

    // std::cout << i << " supply = " << s << " " << added << " r = " << r <<
    // "\n";

    double infl = 100.0 * r * nBlocksPerYear / (double)s;
    if (i % nBlocksPerMonth == 0) {
      supply.push_back(s / billion);
      rewards.push_back(r);
      x.push_back(i / (double)nBlocksPerYear);
      rf << (double)i / nBlocksPerYear << " " << r << " " << s / billion << " "
         << infl << "\n";
    }
    if ((i % nBlocksPerMonth == 0) && (i > nBlocksPerYear)) {
      x_inf.push_back(i / (double)nBlocksPerYear);
      inflation.push_back(infl);
    }
  }
  rf.close();

  std::stringstream ss;
  if (cold_fraction) {
    ss << "(Cold ";
    for (int i = 0; i < crpc.size(); i++) {
      ss << crpc[i];
      if (i < crpc.size() - 1)
        ss << "/";
      else
        ss << "%) ";
    }
  } else {
      if (BudgetPerCent == 0) {
          ss << "Mining only rewards : ";
      } else {
          ss << "Without cold rewards : ";
      }
  }
  //  plot_xy(x, supply, ss.str() + "Total supply per year (billions)");
  //  plot_xy(x, rewards, ss.str() + "Total rewards per year (coins)");
  //  plot_xy(x_inf, inflation, ss.str() + "inflation per year (%)");

  return (0);
}
