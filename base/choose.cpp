#include "choose.hpp"
#include <regex>
#include <QRegularExpression>
#include <QRandomGenerator>


const std::regex chooseReStd(R"(^((\d+) ?\*).*)");
std::pair<std::string, int> choose(const std::vector<std::string>& opts) {
    std::vector<std::pair<int, int>> choices; // idx, prefix len
    int idx = 0;
    for (const auto& str : opts) {
        std::smatch matches;
        if (std::regex_search(str, matches, chooseReStd)) {
            int amnt = static_cast<int>(std::stoul(matches[2]));
            int pln = matches[1].length();
            for (int i = 0; i < amnt; i++) choices.push_back({idx, pln});
        } else {
            choices.push_back({idx, 0});
        }
        idx++;
    }
    int ln = choices.size();
    if (ln == 0) return {{}, -1};
    int out = QRandomGenerator::global()->bounded(ln);
    return {opts[out].substr(choices[out].second), choices[out].first};
}

const QRegularExpression chooseReQ(R"(^((\d+) ?\*).*)");
std::pair<QString, int> choose(const std::vector<QString>& opts) {
    std::vector<std::pair<int, int>> choices; // idx, prefix len
    int idx = 0;
    for (const auto& str : opts) {
        auto m = chooseReQ.match(str);
        if (m.hasMatch()) {
            uint amnt = m.captured(2).toInt();
            int pln = m.captured(1).length();
            for (uint i = 0; i < amnt; i++) choices.push_back({idx, pln});
        } else {
            choices.push_back({idx, 0});
        }
        idx++;
    }
    int ln = choices.size();
    if (ln == 0) return {{}, -1};
    int out = QRandomGenerator::global()->bounded(ln);
    auto c = choices[out];
    return {opts[c.first].mid(c.second), c.first};
}

std::pair<QString, int> choose(const QStringList& opts) {
    return choose(std::vector(opts.constBegin(), opts.constEnd()));
}
