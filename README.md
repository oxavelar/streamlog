# streamlog
A C++ stream (std::cout, std::clog, std::cerr, etc) log redirector to be used together with spdlog.

# example
Here is an example for redirecting the 3 streams of "cout", "clog", "cerr" into a console screen and a logfile:

 ```cpp
 #include "streamlog.hpp"
 
 #define appname "hello_world"
 #define logfile "hello_world.log"
  
auto screen = spdlog::stdout_color_st("console");
auto logger = spdlog::basic_logger_st("logfile", logfile);
 
spdlog::set_pattern("[%H:%M:%S.%e] %L: " appname ": %v");

screen->set_level(spdlog::level::debug);
logger->set_level(spdlog::level::debug);

streamlog redirector_cout(std::cout, streamlog::loglevel::info);
streamlog redirector_clog(std::clog, streamlog::loglevel::debug);
streamlog redirector_cerr(std::cerr, streamlog::loglevel::error);

std::cout << std::nounitbuf;
std::clog << std::nounitbuf;
std::cerr << std::nounitbuf;
```
