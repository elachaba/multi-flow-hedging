using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FinancialApp.Pricing
{
    public class PricingResults
    {
        public double Price { get; set; }
        public Double[] Deltas { get; set; }
        public double PriceStdDev { get; set; }
        public Double[] DeltasStdDev { get; set; }
    }
}
