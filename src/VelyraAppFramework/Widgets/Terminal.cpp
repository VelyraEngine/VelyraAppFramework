#include <Pch.hpp>

#include <VelyraAppFramework/Widgets/Terminal.hpp>
#include <VelyraAppFramework/Styles.hpp>

namespace Velyra::App::Widgets {

    TerminalLine notificationLine(const std::string &content) {
        TerminalLine line;
        line.timestamp = getTime();
        line.content = content;
        line.color = Styles::ColorHighLight;
        return line;
    }

    TerminalLine errorLine(const std::string &content) {
        TerminalLine line;
        line.timestamp = getTime();
        line.content = content;
        line.color = Styles::ColorRed;
        return line;
    }

    TerminalLine successLine(const std::string &content) {
        TerminalLine line;
        line.timestamp = getTime();
        line.content = content;
        line.color = Styles::ColorGreen;
        return line;
    }

    Terminal::Terminal(const Size maxLines) : m_MaxLines(maxLines) {}

    void Terminal::addLine(const TerminalLine& line) {
        std::lock_guard<std::mutex> lock(m_BufferMutex);
        m_Lines.push_back(line);
        if (m_Lines.size() > m_MaxLines) {
            m_Lines.pop_front();
        }
    }

    void Terminal::addLines(const std::vector<TerminalLine>& lines) {
        std::lock_guard<std::mutex> lock(m_BufferMutex);
        for (const auto& line : lines) {
            m_Lines.push_back(line);
            if (m_Lines.size() > m_MaxLines) {
                m_Lines.pop_front();
            }
        }
    }

    void Terminal::draw() {
        std::lock_guard<std::mutex> lock(m_BufferMutex);

        if (ImGui::Button("Clear")) {
            m_Lines.clear();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Auto-Scroll", &m_AutoScroll);

        ImGui::Separator();

        constexpr ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit |
                ImGuiTableFlags_NoBordersInBody |
                ImGuiTableFlags_NoBordersInBodyUntilResize |
                ImGuiTableFlags_NoPadOuterX |
                ImGuiTableFlags_NoPadInnerX |
                ImGuiTableFlags_ScrollY;
        // TODO: Dirty hack to always draw the bottom line
        const ImVec2 size = ImVec2(0, ImGui::GetContentRegionAvail().y - 30.0f);
        if (ImGui::BeginTable("terminal_output", 2, flags, size)) {
            ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 160.0f); // fixed width for line numbers
            ImGui::TableSetupColumn("Text", ImGuiTableColumnFlags_WidthStretch);


            ImGuiListClipper clipper;
            clipper.Begin(static_cast<int>(m_Lines.size()));
            if (m_AutoScroll && !m_Lines.empty()) {
                clipper.IncludeItemByIndex(static_cast<int>(m_Lines.size()) - 1);
            }
            while (clipper.Step()) {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                    const auto& line = m_Lines[i];

                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    const std::string& time = Utils::toString(line.timestamp);
                    ImGui::TextUnformatted(time.c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::ColorConvertFloat4ToU32(line.color));
                    ImGui::TextUnformatted(line.content.c_str());
                }
            }
            if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndTable();
        }

    }

}