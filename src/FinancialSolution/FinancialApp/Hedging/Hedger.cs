using FinancialApp.PorfolioManagement;
using FinancialApp.RebalancingOracle;
using ParameterInfo;
using MarketData;
using TimeHandler;
using FinancialApp.PricingClient;
using System.ComponentModel;

namespace FinancialApp.Hedging
{
    public class Hedger
    {
        private readonly IPricerClient _pricer;
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
            _pricer = new PricerClient();
            _oracle = new FixedRebalancingOracle(_testParam.RebalancingOracleDescription.Period);
            _mathDateConverter = new MathDateConverter(_testParam.NumberOfDaysInOneYear);
            
            string domesticCurrencyId = _testParam.AssetDescription.DomesticCurrencyId;
            RiskFreeRate = _testParam.AssetDescription.CurrencyRates[domesticCurrencyId];
            
            ShareIds = marketData[0].SpotList.Keys.ToArray();
            initPortfolio(marketData[0]);
        }

        private void initPortfolio(DataFeed initData)
        {
            try
            {
                var pastData = new[] { initData }; // Only initial data point
                double initialTime = _mathDateConverter.ConvertToMathDistance(_testParam.PayoffDescription.CreationDate, initData.Date);
                bool isMonitoringDate = _testParam.PayoffDescription.PaymentDates.Contains(initData.Date);

                _pricer.ComputePriceAndDeltas(pastData, isMonitoringDate, initialTime);
                var pricingResults = _pricer.PricingResult;

                Dictionary<string, double> composition = ShareIds
                    .Zip(pricingResults.Deltas, (share, delta) => new { share, delta })
                    .ToDictionary(couple => couple.share, couple => couple.delta);

                _portfolio = new Portfolio(pricingResults.Price, composition, initData.Date);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error during portfolio initialization: {ex.Message}");
                throw;
            }
        }

        private OutputData createOutputData(DateTime Date, PricingResult results, double portfolioValue)
        {
            return new OutputData
            {
                Date = Date,
                Value = portfolioValue,
                Deltas = results.Deltas.ToArray(),
                DeltasStdDev = results.DeltasStdDev.ToArray(),
                Price = results.Price,
                PriceStdDev = results.PriceStdDev,
            };
        }

        private void rebalance(DataFeed dataFeed)
        {
            try
            {
                if (dataFeed == null)
                {
                    throw new ArgumentNullException("Rebalancing Failed: data feed null");
                }

                if (_portfolio == null)
                {
                    throw new ArgumentNullException("Rebalancing failed: _portfolio is null");
                }

                var pastData = _marketData
                    .TakeWhile(d => d.Date <= dataFeed.Date)
                    .Where(d => _testParam.PayoffDescription.PaymentDates.Contains(d.Date))
                    .ToArray();

                // Time for pricer is from option start date
                double pricerTime = _mathDateConverter.ConvertToMathDistance(_testParam.PayoffDescription.CreationDate, dataFeed.Date);

                // Time for portfolio update is from last update
                double portfolioTime = _mathDateConverter.ConvertToMathDistance(_portfolio.LastUpdateDate, dataFeed.Date);

                bool isMonitoringDate = _testParam.PayoffDescription.PaymentDates.Contains(dataFeed.Date);

                if (!isMonitoringDate)
                {
                    pastData = pastData.Concat(new[] { dataFeed }).ToArray();
                }

                _pricer.ComputePriceAndDeltas(pastData, isMonitoringDate, pricerTime);
                var pricingResults = _pricer.PricingResult;

                Dictionary<string, double> composition = ShareIds
                    .Zip(pricingResults.Deltas, (share, delta) => new { share, delta })
                    .ToDictionary(couple => couple.share, couple => couple.delta);

                _portfolio.Update(composition, dataFeed, RiskFreeRate, portfolioTime);
                double value = _portfolio.GetValue(dataFeed.SpotList, RiskFreeRate, portfolioTime);
                var output = createOutputData(dataFeed.Date, pricingResults, value);
                _outputData.Add(output);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error during rebalancing: {ex.Message}");
                throw;
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
