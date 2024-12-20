using GrpcPricing.Protos;
using MarketData;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FinancialApp.PricingClient
{
    public interface IPricerClient : IDisposable
    {
        void ComputePriceAndDeltas(DataFeed[] past, bool monitoringDateReached, double time);
        ReqInfo Heartbeat();
        PricingResult PricingResult { get; }
    }
}
