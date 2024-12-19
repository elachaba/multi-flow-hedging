using ParameterInfo;



namespace FinancialApp.Pricing
{
    public class Pricer
    {
        public TestParameters TestParams { get; set; }
        
        public Pricer(TestParameters testParameters)
        {
            TestParams = testParameters;
        }

        public PricingResults Price(DateTime date, Double[] spots)
        {
            return new PricingResults();
        }
    }
}
