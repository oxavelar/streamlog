/*
 * MIT License
 *
 * Copyright (c) 2018 Omar Avelar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once
#include <string>
#include <memory>
#include <streambuf>
#include <functional>
#include "spdlog/include/spdlog/spdlog.h"


/*
 * Example for redirecting the 3 streams of "cout", "clog", "cerr"
 * into a console screen and a logfile, make streams buffered:
 *
 *   auto screen = spdlog::stdout_color_st("console");
 *   auto logger = spdlog::basic_logger_st("logfile", cmd.logfile);
 *
 *   spdlog::set_pattern("[%H:%M:%S.%e] %L: " appname ": %v");
 *
 *   screen->set_level(spdlog::level::debug);
 *   logger->set_level(spdlog::level::debug);
 *
 *   streamlog redirector_cout(std::cout, streamlog::loglevel::info);
 *   streamlog redirector_clog(std::clog, streamlog::loglevel::debug);
 *   streamlog redirector_cerr(std::cerr, streamlog::loglevel::error);
 *
 *   std::cout << std::nounitbuf;
 *   std::clog << std::nounitbuf;
 *   std::cerr << std::nounitbuf;
 *
 */
class streamlog: public std::streambuf
{
    public:
        enum class loglevel { info, debug, error };

        explicit streamlog(std::ostream &o, const loglevel &l) : 
            src(o),
            srcbuf(o.rdbuf()),
            level(l),
            screen(spdlog::get("console")),
            logger(spdlog::get("logfile"))
        {
            /* Redirect stream to streamlog */
            stream = std::unique_ptr<std::ostream>(new std::ostream(srcbuf));
            o.rdbuf(this);

            using namespace std::placeholders;

            /* Initialize callbacks and set-up based on level */
            switch(level)
            {
                case loglevel::debug:
                    fn = std::bind(&streamlog::debug, this, _1);
                    break;
                case loglevel::info:
                    fn = std::bind(&streamlog::info,  this, _1);
                    break;
                case loglevel::error:
                    fn = std::bind(&streamlog::error, this, _1);
                    break;
                }
            }

        ~streamlog()
        {
            src.rdbuf(srcbuf);
        }

    public:
        virtual int overflow(int c)
        {
            /* New incoming characters */
            auto s = traits_type::to_char_type(c);
            sbuffer += s;
            return 0;
        }

        int sync(void)
        {
            /* Send console and log */
            if (not sbuffer.empty()) {
                fn(sbuffer);
                sbuffer.clear();
            }

            return 0;
        }

    private:
        std::ostream &src;
        std::unique_ptr<std::ostream> stream;
        std::streambuf * const srcbuf;
        const loglevel &level;
        const std::shared_ptr<spdlog::logger> screen;
        const std::shared_ptr<spdlog::logger> logger;
        std::string sbuffer;
        std::function<void(const std::string &msg)> fn;

    private:
        void debug(const std::string &msg)
        {
            screen->debug("\e[00m" + msg + "\e[00m");
            logger->debug(msg);
        }

        void info(const std::string &msg)
        {
            screen->info("\e[93m" + msg +  "\e[00m");
            logger->info(msg);
        }

        void error(const std::string &msg)
        {
            screen->error("\e[91m" + msg + "\e[00m");
            logger->error(msg);
        }
};
