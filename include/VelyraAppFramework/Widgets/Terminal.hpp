#pragma once

#include <string>
#include <deque>
#include <vector>
#include <mutex>
#include <VelyraUtils/Clock/Clock.hpp>

namespace Velyra::App::Widgets {

    struct TerminalLine {
        TimePoint timestamp;
        std::string content;
        ImVec4 color = ImVec4(0.12f,0.12f,0.12f,1.0f);
    };

    TerminalLine notificationLine(const std::string& content);

    TerminalLine errorLine(const std::string& content);

    TerminalLine successLine(const std::string& content);

    class Terminal {
    public:
        explicit Terminal(Size maxLines = 1000);

        void addLine(const TerminalLine& line);

        void addLines(const std::vector<TerminalLine>& lines);

        void draw();

    private:
        size_t m_MaxLines;
        std::deque<TerminalLine> m_Lines;
        mutable std::mutex m_BufferMutex;

        bool m_AutoScroll = true;
    };

}