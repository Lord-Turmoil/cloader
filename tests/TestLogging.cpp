#include <cloader/cloader.h>

#include <doctest/doctest.h>

#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>

using namespace cloader::details::logging;

namespace
{

// Captures whatever is written to std::cerr while it is alive.
class CaptureStdErr
{
public:
    CaptureStdErr() : _old(std::cerr.rdbuf(_buffer.rdbuf()))
    {
    }

    ~CaptureStdErr()
    {
        std::cerr.rdbuf(_old);
    }

    std::string Str() const
    {
        return _buffer.str();
    }

private:
    std::ostringstream _buffer;
    std::streambuf* _old;
};

} // namespace

// Each of these cases runs in its own process (see tests/CMakeLists.txt) with
// CLOADER_LOG set by ctest, because the level is read once and cached.
// "log-level-debug" also checks that the values are matched case-insensitively.

TEST_CASE("log-level-disabled")
{
    CHECK(CurrentLogLevel() == LogLevel::Disabled);
    CaptureStdErr capture;
    LogError("should not appear");
    CHECK(capture.Str().empty());
}

TEST_CASE("log-level-invalid")
{
    CHECK(CurrentLogLevel() == LogLevel::Disabled);
    CaptureStdErr capture;
    LogError("should not appear");
    CHECK(capture.Str().empty());
}

TEST_CASE("log-level-debug")
{
    CHECK(CurrentLogLevel() == LogLevel::Debug);
    CaptureStdErr capture;
    LogDebug("dbg");
    CHECK(capture.Str() == "(cloader) [DEBUG] dbg\n");
}

TEST_CASE("log-level-info")
{
    CHECK(CurrentLogLevel() == LogLevel::Info);
    CaptureStdErr capture;
    LogDebug("filtered");
    LogInfo("inf");
    LogWarning("warn");
    LogError("err");
    CHECK(capture.Str() == "(cloader) [INFO] inf\n"
                           "(cloader) [WARNING] warn\n"
                           "(cloader) [ERROR] err\n");
}

TEST_CASE("log-level-error")
{
    CHECK(CurrentLogLevel() == LogLevel::Error);
    CaptureStdErr capture;
    LogWarning("filtered");
    LogError("err");
    CHECK(capture.Str() == "(cloader) [ERROR] err\n");
}
