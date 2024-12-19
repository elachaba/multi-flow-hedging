using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace FinancialApp.RebalancingOracle
{
    public class FixedRebalancingOracle : IRebalancingOracle
    {
        private readonly int RebalancePeriod;
        private int DaysSinceLastRebalance;


        public FixedRebalancingOracle(int rebalancePeriod)
        {
            if (rebalancePeriod <= 0)
                throw new ArgumentException("Rebalance period must be positive.", nameof(rebalancePeriod));

            RebalancePeriod = rebalancePeriod;
            DaysSinceLastRebalance = 0;
        }

        public bool ShouldRebalance()
        {
            DaysSinceLastRebalance++;

            if (DaysSinceLastRebalance >= RebalancePeriod)
            {
                Reset();
                return true;
            }

            return false;
        }

 
        public void Reset()
        {
            DaysSinceLastRebalance = 0;
        }
    }
}

