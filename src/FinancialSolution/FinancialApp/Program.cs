using System;
using System.Text.Json;
using FinancialApp.Hedging;
using MarketData;
using ParameterInfo;
using ParameterInfo.JsonUtils;

namespace FinancialApp
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length != 3)
            {
                Console.WriteLine("Usage: program <financial-param.json> <MarketData.csv> <output.json>");
                Environment.Exit(1);
            }

            try
            {
                // Get the file paths
                string paramFilePath = args[0];
                string marketDataPath = args[1];
                string outputPath = args[2];

                // Financial Parameters
                string jsonContent = File.ReadAllText(paramFilePath);
                TestParameters testParams = JsonIO.FromJson(jsonContent);

                // Market Data
                List<DataFeed> marketData = MarketDataReader.ReadDataFeeds(marketDataPath);

                // Hedging
                var hedger = new Hedger(testParams, marketData);
                var results = hedger.Hedge();

                var jsonOptions = new JsonSerializerOptions
                {
                    WriteIndented = true
                };
                string outputJson = JsonSerializer.Serialize(results, jsonOptions);
                File.WriteAllText(outputPath, outputJson);

                Console.WriteLine("Hedging completed successfully.");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"An error occurred: {ex.Message}");
                Console.WriteLine($"Stack trace: {ex.StackTrace}");
                Environment.Exit(1);
            }
        }
    }
}
