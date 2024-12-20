using Grpc.Net.Client;
using GrpcPricing.Protos;
using MarketData;


namespace FinancialApp.PricingClient
{
    public class PricerClient : IPricerClient
    {
        private readonly GrpcChannel _channel;
        private readonly GrpcPricer.GrpcPricerClient _client;
        private bool _disposed;

        public PricingResult PricingResult { get; private set; }

        public PricerClient(string serverAddress = "http://localhost:50051")
        {
            // Configure for development (disable SSL validation)
            var httpHandler = new HttpClientHandler
            {
                ServerCertificateCustomValidationCallback =
                    HttpClientHandler.DangerousAcceptAnyServerCertificateValidator
            };

            // Initialize gRPC channel and client
            _channel = GrpcChannel.ForAddress(serverAddress,
                new GrpcChannelOptions { HttpHandler = httpHandler });
            _client = new GrpcPricer.GrpcPricerClient(_channel);
        }
        public void ComputePriceAndDeltas(DataFeed[] past, bool monitoringDateReached, double time)
        {
            // Create input for gRPC
            var input = CreatePricingInput(past, monitoringDateReached, time);

            // Call the gRPC service
            var output = _client.PriceAndDeltas(input);

            // Store results
            UpdateResults(output);
        }

        private PricingInput CreatePricingInput(DataFeed[] past, bool monitoringDateReached, double time)
        {
            var input = new PricingInput
            {
                MonitoringDateReached = monitoringDateReached,
                Time = time
            };

            foreach (var pastLine in past)
            {
                var line = new PastLines();
                foreach (double price in pastLine.SpotList.Values)
                {
                    line.Value.Add(price);
                }
                input.Past.Add(line);
            }

            return input;
        }

        private void UpdateResults(PricingOutput output)
        {
            PricingResult = new PricingResult(output.Price, output.PriceStdDev, output.Deltas.ToArray(), output.DeltasStdDev.ToArray());
        }

        public ReqInfo Heartbeat()
        {
            return _client.Heartbeat(new Empty());
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!_disposed)
            {
                if (disposing)
                {
                    _channel?.Dispose();
                }
                _disposed = true;
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
    }
}

