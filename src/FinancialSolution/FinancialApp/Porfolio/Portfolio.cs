﻿using MarketData;

namespace FinancialApp.PorfolioManagement
{
    public class Portfolio
    {
        public Dictionary<string, double> Composition { get; private set; }
        public double Cash { get; private set; }
        public DateTime LastUpdateDate { get; set; }

        public Portfolio(double initCash, DateTime date)
        {
            Composition = new Dictionary<string, double>();
            Cash = initCash;
            LastUpdateDate = date;
        }

       public Portfolio(double initValue, Dictionary<string, double> composition, Dictionary<string, double> spots, DateTime date)
        {
            

            Composition = composition;
            
            double positionsCost = GetRiskyAssetValue(spots);
            Cash = initValue - positionsCost;

            LastUpdateDate = date;
        }

        public double GetValue(Dictionary<string, double> spots, double riskFreeRate, double time)
        {
            double value = GetDiscountedCash(riskFreeRate, time) + GetRiskyAssetValue(spots);
            
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
            LastUpdateDate = dataFeed.Date;
        }

        public double GetDiscountedCash(double riskFreeRate, double time)
        {
            return Cash * Math.Exp(riskFreeRate * time);
        }

        public double GetRiskyAssetValue(Dictionary<string, double> spots)
        {
            double value = 0.0;
            foreach (var asset in Composition.Keys)
            {
                if (spots.ContainsKey(asset))
                {
                    value += spots[asset] * Composition[asset];
                }
                else
                {
                    throw new ArgumentException($"Spot price for asset {asset} missing.");
                }
            }
            return value;
        }

    }
}
