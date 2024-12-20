using GrpcPricing.Protos;
using MarketData;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FinancialApp.PricingClient
{
    public class PricingResult
    {
        public double Price { get; }
        public double PriceStdDev { get; }
        public IReadOnlyList<Double> Deltas { get; }
        public IReadOnlyList<Double> DeltasStdDev { get; }

        public PricingResult(double price, double priceStdDev, double[] deltas, double[] deltasStdDev)
        {
            Price = price;
            PriceStdDev = priceStdDev;
            Deltas = deltas;
            DeltasStdDev = deltasStdDev;
        }
    }
}
