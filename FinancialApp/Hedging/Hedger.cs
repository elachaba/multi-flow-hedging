using FinancialApp.PorfolioManagement;
using FinancialApp.RebalancingOracle;
using ParameterInfo;
using MarketData;
using TimeHandler;
using FinancialApp.Pricing;
using System.ComponentModel;

namespace FinancialApp.Hedging
{
    public class Hedger
    {
        private readonly Pricer _pricer; // TODO
        private readonly FixedRebalancingOracle _oracle;
        private readonly TestParameters _testParam;
        private readonly List<DataFeed> _marketData;
        private readonly MathDateConverter _mathDateConverter;
        private Portfolio? _portfolio;
        private readonly List<OutputData> _outputData;
        private readonly double RiskFreeRate;
        private readonly string[] ShareIds;

        public Hedger(TestParameters testParam, List<DataFeed> marketData)
        {
            _testParam = testParam;
            _marketData = marketData;
            _outputData = new List<OutputData>();
            _pricer = new Pricer(testParam);
            _oracle = new FixedRebalancingOracle(_testParam.RebalancingOracleDescription.Period);
            _mathDateConverter = new MathDateConverter(_testParam.NumberOfDaysInOneYear);
            
            string domesticCurrencyId = _testParam.AssetDescription.DomesticCurrencyId;
            RiskFreeRate = _testParam.AssetDescription.CurrencyRates[domesticCurrencyId];
            
            ShareIds = marketData[0].SpotList.Keys.ToArray();
            initPortfolio(marketData[0]);
        }

        private void initPortfolio(DataFeed initData)
        {
            Double[] spots = initData.SpotList.Values.ToArray();
            var pricingResults = _pricer.Price(initData.Date, spots);
            Dictionary<string, double> composition = ShareIds.Zip(pricingResults.Deltas, (share, delta) => new {share, delta})
                .ToDictionary(couple => couple.share, couple => couple.delta);
            double price = pricingResults.Price;
            _portfolio = new Portfolio(price, composition, _marketData[0].Date);
        }

        private OutputData createOutputData(DateTime Date, PricingResults results, double portfolioValue)
        {
            return new OutputData
            {
                Date = Date,
                Value = portfolioValue,
                Deltas = results.Deltas,
                DeltasStdDev = results.DeltasStdDev,
                Price = results.Price,
                PriceStdDev = results.PriceStdDev,
            };
        }

        private void rebalance(DataFeed dataFeed)
        {
            if (dataFeed == null)
            {
                throw new ArgumentNullException("Rebalancing Failed: data feed null");
            }
            else
            {
                Double[] spots = dataFeed.SpotList.Values.ToArray();
                var pricingResults = _pricer.Price(dataFeed.Date, spots);
                Dictionary<string, double> composition = ShareIds.Zip(pricingResults.Deltas, (share, delta) => new { share, delta })
                    .ToDictionary(couple => couple.share, couple => couple.delta);
                if (_portfolio == null)
                {
                    throw new ArgumentNullException("Rebalancing failed: _portfolio is null");
                } else
                {
                    double time = _mathDateConverter.ConvertToMathDistance(_portfolio.LastUpdateDate, dataFeed.Date);
                    _portfolio.Update(composition, dataFeed, RiskFreeRate, time);
                    double value = _portfolio.GetValue(dataFeed.SpotList, RiskFreeRate, time);
                    var output = createOutputData(dataFeed.Date, pricingResults, value);
                    _outputData.Add(output);
                }

                

            }
        }
        public List<OutputData> Hedge()
        {
            for (int i = 1; i < _marketData.Count; i++)
            {
                if (_oracle.ShouldRebalance())
                    rebalance(_marketData[i]);
            }
            return _outputData;
        }


    }

}
