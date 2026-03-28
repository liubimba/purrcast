//
// Created by bimba on 11/15/25.
//

#include "launcher.hpp"
#include "argparse/argparse.hpp"

int main(int argc, char** argv)
{
      argparse::ArgumentParser parser("multiroom");

      parser.add_argument("--snapserver.path")
            .help("Path to snapserver binary file")
            .required();
      parser.add_argument("--snapserver.config")
            .help("Path to snapserver config file")
            .default_value("");
      parser.add_argument("--snapclient.path")
            .help("Path to snapclient binary file")
            .required();
      parser.add_argument("--server.path")
            .help("Path to server binary file")
            .required();
      parser.add_argument("--server.port")
            .help("Port to bind for server")
            .required();
      parser.add_argument("--server.static_dir")
            .help("Path to static directory")
            .required();
      parser.add_argument("--monitor.port")
            .help("Port to bind for monitoring")
            .required();
      parser.add_argument("--monitor.address")
            .help("Monitor address to bind")
            .default_value("0.0.0.0");
      parser.add_argument("--monitor.interval")
            .help("Interval between monitoring in milliseconds")
            .default_value("1000");
      parser.add_argument("--test.snapclient.path")
            .help("Path to test snapclient binary file")
            .default_value("--snapclient.path");
      parser.add_argument("--test.enabled")
            .choices("false", "true")
            .help("Enable testing environment")
            .default_value("false");

      try
      {
            parser.parse_args(argc, argv);

            settings settings;

            settings.module.loopback.order_of_loading = MIN_ORDER;

            settings.module.snapclient.path_to_binary = parser.get<std::string>("--snapclient.path");

            settings.module.snapserver.path_to_binary = parser.get<std::string>("--snapserver.path");
            settings.module.snapserver.config = parser.get<std::string>("--snapserver.config");

            settings.module.monitor.interval = std::atoi(parser.get<std::string>("--monitor.interval").c_str());
            settings.module.monitor.address = parser.get<std::string>("--monitor.address");
            settings.module.monitor.port = std::atoi(parser.get<std::string>("--monitor.port").c_str());

            settings.module.server.bin = parser.get<std::string>("--server.path");
            settings.module.server.port = std::atoi(parser.get<std::string>("--server.port").c_str());
            settings.module.server.static_dir = parser.get<std::string>("--server.static_dir");
            settings.module.server.extra_args = absl::StrFormat("--monitor.port %d", settings.module.monitor.port);

            settings.module.test_environment.enabled = parser.get<std::string>("--test.enabled") == "true";
            settings.module.test_environment.loopback.module_name = "test_environment_loopback";
            settings.module.test_environment.loopback.set_as_default = false;
            settings.module.test_environment.loopback.loopback_sink_name = "test-null";
            settings.module.test_environment.loopback.order_of_loading = MIN_ORDER;

            settings.module.test_environment.snapclient.module_name = "test_environment_snapclient";
            if (parser.get<std::string>("--test.snapclient.path") == "--snapclient.path")
            {
                  settings.module.test_environment.snapclient.path_to_binary = settings.module.snapclient.
                                                                                        path_to_binary;
            }
            else
            {
                  settings.module.snapclient.path_to_binary = parser.get<std::string>("--test.snapclient.path");
            }

            launcher launcher;
            launcher.launch(settings);
      }
      catch (const std::exception& e)
      {
            std::cerr << e.what() << std::endl;
            std::cerr << parser;
            std::exit(EXIT_FAILURE);
      }
}
