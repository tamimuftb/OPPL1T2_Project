#include "HtmlReporter.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>

HtmlReporter::HtmlReporter(const std::string& path, const std::string& playbookName,
                            const std::string& targetOS, const std::string& targetHost)
    : m_path(path), m_playbookName(playbookName),
      m_targetOS(targetOS), m_targetHost(targetHost) {}

std::string HtmlReporter::esc(const std::string& s)
{
    std::string o; o.reserve(s.size());
    for (char c : s) switch(c) {
        case '&': o += "&amp;";  break;
        case '<': o += "&lt;";   break;
        case '>': o += "&gt;";   break;
        case '"': o += "&quot;"; break;
        default:  o += c;
    }
    return o;
}

void HtmlReporter::writeHeader(std::ofstream& f) const
{
    f << R"(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Sentinel-Red Report</title>
<style>
:root{--bg:#0d1117;--surface:#161b22;--border:#30363d;--text:#c9d1d9;
      --muted:#8b949e;--accent:#58a6ff;--red:#f85149;--green:#3fb950;
      --yellow:#d29922;--red-bg:#1c0b0b;--grn-bg:#0b1c0d;}
*{box-sizing:border-box;margin:0;padding:0;}
body{background:var(--bg);color:var(--text);
     font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,monospace;
     line-height:1.6;padding:2rem;}
.wrap{max-width:1000px;margin:0 auto;}
header{border-bottom:1px solid var(--border);padding-bottom:1.5rem;margin-bottom:2rem;}
h1{font-size:2rem;color:var(--accent);letter-spacing:-.5px;}
.subtitle{color:var(--muted);font-size:.9rem;margin-top:.25rem;}
.meta{display:flex;gap:2rem;flex-wrap:wrap;margin:1rem 0;font-size:.85rem;color:var(--muted);}
.meta span b{color:var(--text);}
.stats{display:flex;gap:1rem;flex-wrap:wrap;margin-bottom:2rem;}
.stat{background:var(--surface);border:1px solid var(--border);border-radius:10px;
      padding:1rem 1.5rem;min-width:130px;text-align:center;}
.stat-n{font-size:2.2rem;font-weight:700;line-height:1;}
.stat-l{font-size:.75rem;color:var(--muted);text-transform:uppercase;letter-spacing:.06em;margin-top:.3rem;}
.blue{color:var(--accent);} .red{color:var(--red);} .green{color:var(--green);} .yellow{color:var(--yellow);}
.card{background:var(--surface);border:1px solid var(--border);border-radius:8px;
      padding:1.25rem 1.5rem;margin-bottom:1rem;}
.card.det{border-left:4px solid var(--red);background:var(--red-bg);}
.card.ok {border-left:4px solid var(--green);background:var(--grn-bg);}
.card-head{display:flex;align-items:center;gap:.5rem;flex-wrap:wrap;margin-bottom:.5rem;}
.card-head h3{font-size:1rem;color:var(--text);}
.badge{font-size:.7rem;font-weight:700;padding:.15rem .55rem;border-radius:4px;}
.badge.det{background:var(--red);color:#fff;}
.badge.ok {background:var(--green);color:#000;}
.mitre{font-size:.78rem;color:var(--accent);text-decoration:none;}
.mitre:hover{text-decoration:underline;}
pre{background:#010409;border:1px solid var(--border);border-radius:6px;
    padding:.75rem 1rem;margin-top:.75rem;font-size:.78rem;
    white-space:pre-wrap;word-break:break-all;color:#79c0ff;
    max-height:280px;overflow-y:auto;}
footer{margin-top:3rem;text-align:center;font-size:.8rem;color:var(--muted);
       border-top:1px solid var(--border);padding-top:1rem;}
</style>
</head>
<body><div class="wrap">
)";
}

void HtmlReporter::writeFooter(std::ofstream& f) const
{
    f << "<footer>Sentinel-Red &mdash; Automated TTP Emulation Engine &mdash; "
         "For authorized security testing only</footer>\n"
         "</div></body></html>\n";
}

void HtmlReporter::writeTechnique(std::ofstream& f, const ITechnique& t) const
{
    bool det = t.isDetected();
    std::string mitreUrl = "https://attack.mitre.org/techniques/" + t.getMitreId();
    f << "<div class='card " << (det?"det":"ok") << "'>\n"
      << "  <div class='card-head'>\n"
      << "    <h3>" << esc(t.getName()) << "</h3>\n"
      << "    <span class='badge " << (det?"det":"ok") << "'>"
      << (det?"DETECTED":"CLEAN") << "</span>\n"
      << "    <a class='mitre' href='" << mitreUrl << "' target='_blank'>"
      << esc(t.getMitreId()) << " &#8599;</a>\n"
      << "  </div>\n"
      << "  <pre>" << esc(t.getResult()) << "</pre>\n"
      << "</div>\n";
}

void HtmlReporter::generateReport(const std::vector<std::unique_ptr<ITechnique>>& techniques)
{
    std::ofstream f(m_path);
    if (!f) { std::cerr << "[Report] Cannot open: " << m_path << "\n"; return; }

    writeHeader(f);

    int total = static_cast<int>(techniques.size());
    int det = 0;
    for (const auto& t : techniques) if (t->isDetected()) ++det;
    int clean = total - det;
    double rate = total > 0 ? (det * 100.0 / total) : 0.0;

    std::time_t now = std::time(nullptr);
    std::tm* tm_info = std::localtime(&now);
    std::ostringstream ts;
    ts << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S");

    f << "<header>\n"
      << "  <h1>&#x1F534; Sentinel-Red</h1>\n"
      << "  <div class='subtitle'>Automated TTP Emulation Campaign Report</div>\n"
      << "  <div class='meta'>\n"
      << "    <span><b>Playbook:</b> " << esc(m_playbookName) << "</span>\n"
      << "    <span><b>Target OS:</b> " << esc(m_targetOS) << "</span>\n";
    if (!m_targetHost.empty())
        f << "    <span><b>Target Host:</b> " << esc(m_targetHost) << "</span>\n";
    f << "    <span><b>Run Time:</b> " << ts.str() << "</span>\n"
      << "  </div>\n"
      << "</header>\n\n";

    f << "<div class='stats'>\n"
      << "  <div class='stat'><div class='stat-n blue'>" << total << "</div>"
      << "<div class='stat-l'>Total</div></div>\n"
      << "  <div class='stat'><div class='stat-n green'>" << clean << "</div>"
      << "<div class='stat-l'>Clean</div></div>\n"
      << "  <div class='stat'><div class='stat-n red'>" << det << "</div>"
      << "<div class='stat-l'>Detected</div></div>\n"
      << "  <div class='stat'><div class='stat-n " << (rate>50?"red":"yellow") << "'>"
      << std::fixed << std::setprecision(0) << rate << "%</div>"
      << "<div class='stat-l'>Detection Rate</div></div>\n"
      << "</div>\n\n";

    for (const auto& t : techniques) writeTechnique(f, *t);

    writeFooter(f);
    f.close();
    std::cout << "[Report] Saved: " << m_path << "\n";
}
