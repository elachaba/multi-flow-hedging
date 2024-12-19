using MarketData;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FinancialApp.PorfolioManagement
{
    public class Portfolio
    {
        public Dictionary<string, double> Composition { get; private set; }
        public double Cash { get; private set; }
        public DateTime LastInvestmentDate { get; set; }

        public Portfolio(double initCash, DateTime date)
        {
            Composition = new Dictionary<string, double>();
            Cash = initCash;
            LastInvestmentDate = date;
        }

       public Portfolio(double cash, Dictionary<string, double> composition, DateTime date)
        {
            Composition = composition;
            Cash = cash;
            LastInvestmentDate = date;
        }

        public double GetValue(Dictionary<string, double> spots, double riskFreeRate, double time)
        {
            double value = GetDiscountedCash(riskFreeRate, time);
            foreach (var asset in Composition.Keys)
            {
                if (spots.ContainsKey(asset))
                {
                    value = spots[asset] * Composition[asset];
                } else
                {
                    throw new ArgumentException($"Spot price for asset {asset} missing.");
                }
            }
            
            return value;
        }

        public void Update(Dictionary<string, double> newComposition, DataFeed dataFeed, double riskFreeRate, double time)
        {

            double currValue = GetValue(dataFeed.SpotList, riskFreeRate, time);
            double riskyAssetsValue = 0.0;

            foreach (var asset in newComposition.Keys)
            {
                if (dataFeed.SpotList.TryGetValue(asset, out double spotValue))
                {
                    Composition[asset] = newComposition[asset];
                    riskyAssetsValue += spotValue * Composition[asset];
                } else
                {
                    throw new ArgumentException($"Spot price for asset {asset} missing.");
                }
            }

            Cash = currValue - riskyAssetsValue;
            LastInvestmentDate = dataFeed.Date;
        }

        public double GetDiscountedCash(double riskFreeRate, double time)
        {
            return Cash * Math.Exp(riskFreeRate * time);
        }

    }
}
