#include "TestPch.hpp"

#include <VelyraAppFramework/Widgets/Terminal.hpp>

using namespace Velyra;
using namespace Velyra::App::Widgets;

TEST(TestTerminal, EmptyLineIsKept) {
    Terminal terminal;
    terminal.addLine(notificationLine(""));

    ASSERT_EQ(terminal.get_m_Lines().size(), 1);
    EXPECT_EQ(terminal.get_m_Lines()[0].content, "");
}

TEST(TestTerminal, MultiLineStringIsSplit) {
    Terminal terminal;
    terminal.addLine(errorLine("first\nsecond\n\nfourth"));

    ASSERT_EQ(terminal.get_m_Lines().size(), 4);
    EXPECT_EQ(terminal.get_m_Lines()[0].content, "first");
    EXPECT_EQ(terminal.get_m_Lines()[1].content, "second");
    EXPECT_EQ(terminal.get_m_Lines()[2].content, "");
    EXPECT_EQ(terminal.get_m_Lines()[3].content, "fourth");
}

TEST(TestTerminal, MaxLines) {
    Terminal terminal(3);
    terminal.addLine(successLine("1\n2\n3\n4\n5"));
    terminal.addLine(successLine(""));

    ASSERT_EQ(terminal.get_m_Lines().size(), 3);
    EXPECT_EQ(terminal.get_m_Lines()[0].content, "4");
    EXPECT_EQ(terminal.get_m_Lines()[2].content, "");
}
